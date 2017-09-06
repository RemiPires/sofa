#include "AssemblyVisitor.h"


#include <SofaBaseLinearSolver/SingleMatrixAccessor.h>
#include <SofaBaseLinearSolver/DefaultMultiMatrixAccessor.h>

#include <sofa/helper/cast.h>
#include "../utils/scoped.h"
#include "../utils/sparse.h"

#include "../constraint/ConstraintValue.h"
#include "../constraint/Stabilization.h"

using std::cerr;
using std::endl;


namespace sofa {
namespace simulation {

using namespace component::linearsolver;
using namespace core::behavior;


AssemblyVisitor::AssemblyVisitor(const core::MechanicalParams* mparams)
	: base( mparams ),
      mparams( mparams ),
	  start_node(0),
	  _processed(0)
{
    mparamsWithoutStiffness = *mparams;
    mparamsWithoutStiffness.setKFactor(0);
}


AssemblyVisitor::~AssemblyVisitor()
{
	if( _processed ) delete _processed;
}


AssemblyVisitor::chunk::chunk()
	: offset(0),
	  size(0),
      C(NULL),
      Ktilde(NULL),
	  mechanical(false),
	  vertex(-1),
      dofs(NULL) {

}

bool AssemblyVisitor::is_master(const chunk& c) const {
    const bool is_independent = c.mechanical && c.map.empty();
    const bool is_toplevel = c.mechanical && (c.dofs->getContext() == root);

    return is_independent || is_toplevel;
}
    

bool AssemblyVisitor::is_compliant(const chunk& c) const { 
    return c.mechanical && notempty(c.C);
}



// pretty prints a mapping
static inline std::string mapping_name(simulation::Node* node) {
    return node->mechanicalMapping->getName() +
        " (class: " + node->mechanicalMapping->getClassName()
        + ", node: " + node->getPathName() + ") ";
}

// mapping informations as a map (parent dofs -> J matrix )
AssemblyVisitor::chunk::map_type AssemblyVisitor::mapping(simulation::Node* node) {
	chunk::map_type res;

	if( !node->mechanicalMapping ) return res;

	using helper::vector;

	assert( node->mechanicalMapping->getTo().size() == 1 &&
	        "only n -> 1 mappings are handled");

    ForceMaskActivate(node->mechanicalMapping->getMechTo());
    ForceMaskActivate(node->mechanicalMapping->getMechFrom());

    const vector<sofa::defaulttype::BaseMatrix*>* js = node->mechanicalMapping->getJs();

    ForceMaskDeactivate(node->mechanicalMapping->getMechTo());


    vector<core::BaseState*> from = node->mechanicalMapping->getFrom();

    for( unsigned i = 0, n = from.size(); i < n; ++i ) {

		// parent dofs
		dofs_type* p = safe_cast<dofs_type>(from[i]);

        // skip non-mechanical dofs
        // or this mechanical object is out of scope (ie not in the sub-graph controled by this solver)
        if( !p || chunks.find( p ) == chunks.end() ) continue;

        if( !notempty((*js)[i]) ) {
            msg_warning("AssemblyVisitor") << "Empty mapping block for " << mapping_name(node) 
                                           << " parent: " << i << " (is mapping Jacobian matrix assembled?)";
            continue;
        }

		// mapping wrt p
        chunk::mapped& c = res[p];

        // getting BaseMatrix pointer
        c.J = (*js)[i];
	}

	return res;
}






// projection matrix
AssemblyVisitor::rmat AssemblyVisitor::proj(simulation::Node* node) {
	assert( node->mechanicalState );

	unsigned size = node->mechanicalState->getMatrixSize();

	// identity matrix TODO alloc ?
    shift_right<rmat>(tmp_p.compressedMatrix, 0, size, size);

	for(unsigned i=0; i<node->projectiveConstraintSet.size(); i++){
		node->projectiveConstraintSet[i]->projectMatrix(&tmp_p, 0);
        isPIdentity = false;
	}

    tmp_p.compressedMatrix.prune(0, 0);
	return tmp_p.compressedMatrix;
}




const defaulttype::BaseMatrix* compliance_impl( const core::MechanicalParams* mparams, BaseForceField* ffield )
{
    const defaulttype::BaseMatrix* c = ffield->getComplianceMatrix(mparams);

    if( notempty(c) )
    {
        return c;
    }
    else
    {
        msg_warning("AssemblyVisitor") << "compliance: "<<ffield->getName()<< "(node="<<ffield->getContext()->getName()<<"): getComplianceMatrix not implemented";
        // TODO inverting stiffness matrix
    }

    return NULL;
}


// compliance matrix
const defaulttype::BaseMatrix* AssemblyVisitor::compliance(simulation::Node* node)
{
    for(unsigned i = 0; i < node->forceField.size(); ++i )
    {
		BaseForceField* ffield = node->forceField[i];

		if( !ffield->isCompliance.getValue() ) continue;

        return compliance_impl( mparams, ffield );
	}

    for(unsigned i = 0; i < node->interactionForceField.size(); ++i )
    {
        BaseInteractionForceField* ffield = node->interactionForceField[i];

        if( !ffield->isCompliance.getValue() ) continue;

        if( ffield->getMechModel1() != ffield->getMechModel2() )
        {
            msg_warning("AssemblyVisitor") << "interactionForceField "<<ffield->getName()<<" cannot be simulated as a compliance.";
        }
        else
        {
            return compliance_impl( mparams, ffield );
        }
    }

    return NULL;
}


// geometric stiffness matrix
const defaulttype::BaseMatrix* AssemblyVisitor::geometricStiffness(simulation::Node* node)
{
//    std::cerr<<"AssemblyVisitor::geometricStiffness "<<node->getName()<<" "<<node->mechanicalMapping->getName()<<std::endl;

    core::BaseMapping* mapping = node->mechanicalMapping;
    if( mapping )
    {
        const sofa::defaulttype::BaseMatrix* k = mapping->getK();
        if( k ) return k;
    }

    return NULL;
}



// interaction forcefied in a node w/o a mechanical state
void AssemblyVisitor::interactionForceField(simulation::Node* node)
{
    for(unsigned i = 0; i < node->interactionForceField.size(); ++i ) {
        BaseInteractionForceField* ffield = node->interactionForceField[i];

        if( ffield->getMechModel1() != ffield->getMechModel2() ) {
            typedef EigenBaseSparseMatrix<SReal> BigSqmat;
            unsigned bigsize = ffield->getMechModel1()->getMatrixSize() + ffield->getMechModel2()->getMatrixSize();
            BigSqmat bigSqmat( bigsize, bigsize );
            DefaultMultiMatrixAccessor accessor;
            accessor.setGlobalMatrix( &bigSqmat );
            accessor.addMechanicalState( ffield->getMechModel1() );
            accessor.addMechanicalState( ffield->getMechModel2() );

            // an interactionFF is always a stiffness
            ffield->addMBKToMatrix( mparams, &accessor );
            bigSqmat.compress();

            if( !zero(bigSqmat.compressedMatrix) ) {
                interactionForceFieldList.push_back( InteractionForceField(bigSqmat.compressedMatrix,ffield) );
            }

//            std::cerr<<"AssemblyVisitor::interactionForceField "<<ffield->getMechModel1()->getName()<<" "<<ffield->getMechModel2()->getName()<<" "<<bigSqmat<<std::endl;
        }
    }
}



// ode matrix
AssemblyVisitor::rmat AssemblyVisitor::odeMatrix(simulation::Node* node)
{
    unsigned size = node->mechanicalState->getMatrixSize();

    typedef EigenBaseSparseMatrix<SReal> Sqmat;
    Sqmat sqmat( size, size );

    for(unsigned i = 0; i < node->interactionForceField.size(); ++i ) {
        BaseInteractionForceField* ffield = node->interactionForceField[i];

        if( ffield->getMechModel1() != ffield->getMechModel2() ) {
//            std::cerr<<SOFA_CLASS_METHOD<<"WARNING: interactionForceField "<<ffield->getName()<<" will be treated as explicit, external forces (interactionForceFields are not handled by Compliant assembly, the same scene should be modelised with MultiMappings)"<<std::endl;

            typedef EigenBaseSparseMatrix<SReal> BigSqmat;
            unsigned bigsize = ffield->getMechModel1()->getMatrixSize() + ffield->getMechModel2()->getMatrixSize();
            BigSqmat bigSqmat( bigsize, bigsize );
            DefaultMultiMatrixAccessor accessor;
            accessor.setGlobalMatrix( &bigSqmat );
            accessor.addMechanicalState( ffield->getMechModel1() );
            accessor.addMechanicalState( ffield->getMechModel2() );

            // an interactionFF is always a stiffness
            ffield->addMBKToMatrix( mparams, &accessor );
            bigSqmat.compress();

            if( !zero(bigSqmat.compressedMatrix) ) {
                interactionForceFieldList.push_back( InteractionForceField(bigSqmat.compressedMatrix,ffield) );
            }

//            std::cerr<<"AssemblyVisitor::odeMatrix "<<ffield->getName()<<" "<<bigSqmat<<std::endl;
        } else {
            // interactionForceFields that work on a unique set of dofs are OK
            SingleMatrixAccessor accessor( &sqmat );

            // when it is a compliant, you need to add M if mass, B but not K
            ffield->addMBKToMatrix( ffield->isCompliance.getValue() ? &mparamsWithoutStiffness : mparams, &accessor );
        }
    }

    // note that mass are included in forcefield
    for(unsigned i = 0; i < node->forceField.size(); ++i ) {
        BaseForceField* ffield = node->forceField[i];

        SingleMatrixAccessor accessor( &sqmat );

        // when it is a compliant, you need to add M if mass, B but not K
        ffield->addMBKToMatrix( ffield->isCompliance.getValue() ? &mparamsWithoutStiffness : mparams, &accessor );
    }

    sqmat.compress();
    return sqmat.compressedMatrix;
}


void AssemblyVisitor::top_down(simulation::Visitor* vis) const {
	assert( !prefix.empty() );

	for(unsigned i = 0, n = prefix.size(); i < n; ++i) {
        simulation::Node* node = down_cast<simulation::Node>(graph[ prefix[i] ].data->dofs->getContext());
		vis->processNodeTopDown( node );
	}

}

void AssemblyVisitor::bottom_up(simulation::Visitor* vis) const {
	assert( !prefix.empty() );

	for(unsigned i = 0, n = prefix.size(); i < n; ++i) {
        simulation::Node* node = down_cast<simulation::Node>(graph[ prefix[ n - 1 - i] ].data->dofs->getContext());
		vis->processNodeBottomUp( node );
	}
	
}





// this is called on the *top-down* traversal, once for each node. we
// simply fetch infos for each dof.
void AssemblyVisitor::fill_prefix(simulation::Node* node) {

    // helper::ScopedAdvancedTimer advancedTimer( "assembly: fill_prefix" );
    
	assert( node->mechanicalState );
    assert( chunks.find( node->mechanicalState ) == chunks.end() && "Did you run the simulation with a DAG traversal?" );

    // should this mstate be ignored?
    {
        // is it empty?
        if( node->mechanicalState->getSize() == 0 ) {
            return;
        }

        // does the mask filter every dofs?
        const sofa::core::behavior::BaseMechanicalState::ForceMask::InternalStorage& mask = node->mechanicalState->forceMask.getEntries();
        if( std::find(mask.begin(), mask.end(), true) == mask.end() ) {
            return;
        }
    }



        
	// fill chunk for current dof
	chunk& c = chunks[ node->mechanicalState ];

	c.size = node->mechanicalState->getMatrixSize();
	c.dofs = node->mechanicalState;

    vertex v; v.data = &c;

	c.H = odeMatrix( node );
//    cerr << "AssemblyVisitor::fill_prefix, c.H = " << endl << dmat(c.H) << endl;

     if( !zero(c.H) ) {
        c.mechanical = true;
     }

    // if the visitor is excecuted from a mapped node, do not look at its mapping
    if( node != start_node ) c.map = mapping( node );
	
	c.vertex = boost::add_vertex(graph);
	graph[c.vertex] = v;

	// independent dofs
	if( c.map.empty() ) {
		c.P = proj( node );
	} else {
		// mapped dofs

        // compliance
		c.C = compliance( node );
        assert(!c.C || c.C->rows() == int(c.size));
        assert(!c.C || c.C->cols() == int(c.size));

        if( notempty(c.C) ) {
			c.mechanical = true;
		}

        // geometric stiffness
        c.Ktilde = geometricStiffness( node );
	}
}



// bottom-up: build dependency graph
void AssemblyVisitor::fill_postfix(simulation::Node* node) {

    // helper::ScopedAdvancedTimer advancedTimer( "assembly: fill_postfix" );

    chunks_type::const_iterator cit = chunks.find( node->mechanicalState );
    // this mstate was ignored in fill_prefix (e.g. it is empty or completely masked out)
    if( cit == chunks.end() ) return;
    const chunk& c = cit->second;

	for(chunk::map_type::const_iterator it = c.map.begin(), end = c.map.end();
	    it != end; ++it) {

        assert( chunks.find( it->first ) != chunks.end() &&
                "this mstate is either non-mechanical or out of the solver's scope and should not have been considered by the mapping "
                "-> sounds like there was a problem during the assembly" );

        const chunk& p = chunks[it->first];

		edge e;
		e.data = &it->second;

		// the edge is child -> parent
		graph_type::edge_descriptor ed = boost::add_edge(c.vertex, p.vertex, graph).first;
		graph[ed] = e;
	}

}




void AssemblyVisitor::chunk::debug() const {
	using namespace std;

	cout << "chunk: " << dofs->getName() << endl
	     << "offset:" << offset << endl
	     << "size: " << size << endl
	     << "H:" << endl << H << endl
	     << "P:" << endl << P << endl
	     << "C:" << endl << C << endl
         << "Ktilde:" << endl << Ktilde << endl
	     << "map: " << endl
		;

	for(map_type::const_iterator mi = map.begin(), me = map.end();
	    mi != me; ++mi) {
		cout << "from: " << mi->first->getName() << endl
		     << "J: " << endl
             << mi->second.J << endl
			;
	}
}

void AssemblyVisitor::debug() const {

	for(chunks_type::const_iterator i = chunks.begin(), e = chunks.end(); i != e; ++i ) {
		i->second.debug();
	}

}



// this is used to propagate mechanical flag upwards mappings (prefix
// in the graph order)
struct AssemblyVisitor::propagation_helper {

	const core::MechanicalParams* mparams;
	graph_type& g;

	propagation_helper(const core::MechanicalParams* mparams,
                       graph_type& g) : mparams(mparams), g(g) {}

    void operator()( unsigned v ) const {

        const chunk* c = g[v].data;

        // if the current child is a mechanical dof
        // or if the current mapping is bringing geometric stiffness
        if( c->mechanical || notempty(c->Ktilde) ) {

            // have a look to all its parents
			for(graph_type::out_edge_range e = boost::out_edges(v, g);
                e.first != e.second; ++e.first) {

				chunk* p = g[ boost::target(*e.first, g) ].data;
                p->mechanical = true; // a parent of a mechanical child is necessarily mechanical
            }

		}
	}

};




struct AssemblyVisitor::prefix_helper {
	prefix_type& res;

	prefix_helper(prefix_type& res) : res(res) {
		res.clear();
	}

	template<class G>
	void operator()(unsigned v, const G& ) const {
		res.push_back( v );
	}

};


template<class Full, class Offsets, class Iterator>
static void process_interaction_forcefields(Full& full, std::size_t size_m, const Offsets& offsets,
                                            Iterator first, Iterator last) {
    // TODO what does this do ?!
    using rmat = AssemblyVisitor::rmat;
    
    for(Iterator it = first; it != last; ++it) {
        it->J.resize( it->H.rows(), size_m );
        
        rmat& Jp0 = full[ it->ff->getMechModel1() ];
        rmat& Jp1 = full[ it->ff->getMechModel2() ];

        if( empty(Jp0) ) {
            auto itoff = offsets.find(it->ff->getMechModel1());
            if( itoff != offsets.end() ) {
                shift_right<rmat>( Jp0, itoff->second, it->ff->getMechModel1()->getMatrixSize(), size_m);
            }
        }
        if( empty(Jp1) ) {
            auto itoff = offsets.find(it->ff->getMechModel2());
            if( itoff != offsets.end() ) {
                shift_right<rmat>( Jp1, itoff->second, it->ff->getMechModel2()->getMatrixSize(), size_m);
            }
        }

        // TODO both have cmat to rmat cast
        if( !empty(Jp0) ) {
            it->J += rmat(shift_left<rmat>( 0, it->ff->getMechModel1()->getMatrixSize(), it->H.rows() ) * Jp0);
        }
        
        if( !empty(Jp1) ) {
            it->J += rmat(shift_left<rmat>( it->ff->getMechModel1()->getMatrixSize(), it->ff->getMechModel2()->getMatrixSize(), it->H.rows() ) * Jp1);
        }
    }
    
}


template<class OutputIterator, class Matrix>
static void copy_triplets(OutputIterator out, const Matrix& matrix, int row_offset = 0, int col_offset = 0,
                          typename Matrix::Scalar factor = 1) {
    for(int outer = 0, end = matrix.outerSize(); outer < end; ++outer) {
        for(typename Matrix::InnerIterator it(matrix, outer); it; ++it) {
            *out++ = {row_offset + it.row(), col_offset + it.col(), factor * it.value()};
        }        
    }    
}


AssemblyVisitor::process_type* AssemblyVisitor::process() const {
    scoped::timer step("assembly: mapping processing");

    process_type* res = new process_type();

    // 1. compute offsets
    unsigned& size_m = res->size_m;
    unsigned& size_c = res->size_c;

	// independent dofs offsets (used for shifting parent)
    offset_type& offsets = res->offset.master;

	unsigned off_m = 0;
	unsigned off_c = 0;

	for(unsigned i = 0, n = prefix.size(); i < n; ++i) {

		const chunk* c = graph[ prefix[i] ].data;

		// independent
		if( is_master(*c) ) {
            offsets[ c->dofs ] = off_m;
			off_m += c->size;
        } else if( is_compliant(*c) ) {
			off_c += c->size;
		}

	}

	// update total sizes
	size_m = off_m;
	size_c = off_c;

    // prefix mapping concatenation and stuff
    std::for_each(prefix.begin(), prefix.end(), process_helper(this, *res, graph) );

    // special treatment for interaction forcefields
    process_interaction_forcefields(res->fullmapping, size_m, offsets,
                                    interactionForceFieldList.begin(), interactionForceFieldList.end());
    
	return res;
}



// this is meant to optimize L^T D L products
inline const AssemblyVisitor::rmat& AssemblyVisitor::ltdl(const rmat& l, const rmat& d) const
{
    // scoped::timer advancedTimer("assembly: ltdl");

    sparse::fast_prod(tmp1, d, l);
    tmp3 = l.transpose();
    sparse::fast_prod(tmp2, tmp3, tmp1);
    
    return tmp2;
}


inline void AssemblyVisitor::add_ltdl(rmat& res, const rmat& l, const rmat& d)  const
{
    // scoped::timer advancedTimer("assembly: ltdl");

    sparse::fast_prod(tmp1, d, l);
    tmp3 = l.transpose();
    sparse::fast_add_prod(res, tmp3, tmp1);
}



typedef AssembledSystem::rmat rmat;


struct add_shifted {

    rmat& result;
    static rmat shift; // shared temporary matrix (note the fact it is static would not help for multithreading)

    add_shifted(rmat& result)
        : result(result) {

        // ideal would be somewhere between n and n^2 (TODO we should add a parameter)
        result.reserve( result.rows() );

    }

    // TODO optimize shift creation
    template<class Matrix>
    void operator()(const Matrix& chunk, unsigned off, SReal factor = 1.0) const {
        shift_right<rmat>(shift, off, chunk.cols(), result.cols(), factor);
        result.middleRows(off, chunk.rows()) = result.middleRows(off, chunk.rows()) + rmat( chunk * shift );
    }

};
rmat add_shifted::shift;


// produce actual system assembly
void AssemblyVisitor::assemble(system_type& res) const {
    scoped::timer step("assembly: build system");
	assert(!chunks.empty() && "need to send a visitor first");

	// assert( !_processed );

	// concatenate mappings and obtain sizes
    _processed = process();

	// result system
    res.reset(_processed->size_m, _processed->size_c);
    
	res.dt = mparams->dt();
    res.isPIdentity = isPIdentity;



    // Geometric Stiffness must be processed first, from mapped dofs to master
    // dofs warning, inverse order is important, to treat mapped dofs before
    // master dofs so mapped dofs can transfer their geometric stiffness to
    // master dofs that will add it to the assembled matrix
    {
        scoped::timer step("assembly: gs processing");
    for( int i = (int)prefix.size()-1 ; i >=0 ; --i ) {

        const chunk& c = *graph[ prefix[i] ].data;
        assert( c.size );

        // only consider mechanical mapped dofs that have geometric stiffness
        if( !c.mechanical || is_master(c) || !c.Ktilde ) continue;

        // Note this is a pointer (no copy for matrices that are already in the
        // right type i.e. EigenBaseSparseMatrix<SReal>)
        helper::OwnershipSPtr<rmat> Ktilde( convertSPtr<rmat>( c.Ktilde ) );

        if( zero( *Ktilde ) ) continue;

        if( boost::out_degree(prefix[i],graph) == 1 ) // simple mapping
        {
//            std::cerr<<"simple mapping "<<c.dofs->getName()<<std::endl;
            // add the geometric stiffness to its only parent that will map it to the master level
            graph_type::out_edge_iterator parentIterator = boost::out_edges(prefix[i],graph).first;
            chunk* p = graph[ boost::target(*parentIterator, graph) ].data;
            p->H += mparams->kFactor() * *Ktilde; // todo how to include rayleigh damping for geometric stiffness?

//            std::cerr<<"Assembly: "<<c.Ktilde<<std::endl;
        }
        else // multimapping
        {
            // directly add the geometric stiffness to the assembled level by
            // mapping with the specific jacobian from master to the (current-1)
            // level


//            std::cerr<<"multimapping "<<c.dofs->getName()<<std::endl;

            // full mapping chunk for geometric stiffness
            const rmat& geometricStiffnessJc = _processed->fullmappinggeometricstiffness[ c.dofs ];



            //std::cerr<<geometricStiffnessJc<<std::endl;
//            std::cerr<<*Ktilde<<std::endl;
//            std::cerr<<Ktilde->nonZeros()<<std::endl;

//            std::cerr<<res.H.rows()<<" "<<geometricStiffnessJc.rows()<<std::endl;

            add_ltdl(res.H, geometricStiffnessJc, mparams->kFactor() * *Ktilde);
        }

    }
    }

    {
        scoped::timer step("assembly: interaction ff");
    // Then add interaction forcefields
    for( InteractionForceFieldList::iterator it=interactionForceFieldList.begin(),
             itend=interactionForceFieldList.end();it!=itend;++it) {
        add_ltdl(res.H, it->J, it->H);
    }
    }
    


	// master/compliant offsets
	unsigned off_m = 0;
	unsigned off_c = 0;


    add_shifted add_H(res.H), add_P(res.P), add_C(res.C);

    using triplet_type = Eigen::Triplet<real>;
    std::vector<triplet_type> Js, Cs;
    
    const SReal c_factor = 1.0 /
        ( res.dt * res.dt * mparams->implicitVelocity() * mparams->implicitPosition() );
    
	// assemble system
    {
        scoped::timer step("assembly: actual assembly");
    for( unsigned i = 0, n = prefix.size() ; i < n ; ++i ) {

		// current chunk
        const chunk& c = *graph[ prefix[i] ].data;
        assert( c.size );

        if( !c.mechanical ) continue;

		// independent dofs: fill mass/stiffness
        if( is_master(c) ) {
            // scoped::timer step("assembly: master");                            
            res.master.push_back( c.dofs );

            if( !zero(c.H) ) add_H(c.H, off_m);
            if( !zero(c.P) ) add_P(c.P, off_m);
            
            off_m += c.size;
		}

		// mapped dofs
		else {

            // full mapping chunk
            const rmat& Jc = _processed->fullmapping[ c.dofs ];

			if( !zero(Jc) ) {
                assert( Jc.cols() == int(_processed->size_m) );
                // scoped::timer step("assembly: add_H");
                // actual response matrix mapping
                if( !zero(c.H) ) add_H(ltdl(Jc, c.H), 0);
            }


			// compliant dofs: fill compliance/phi/lambda
			if( is_compliant(c) ) {
                // scoped::timer step("assembly: compliant");                
				res.compliant.push_back( c.dofs );
				// scoped::timer step("compliant dofs");
				assert( !zero(Jc) );

                // Note this is a pointer (no copy for matrices that are already
                // in the right type i.e. EigenBaseSparseMatrix<SReal>)
                helper::OwnershipSPtr<rmat> C( convertSPtr<rmat>( c.C ) );
                
                    
                // fetch projector and constraint value if any
                AssembledSystem::constraint_type constraint;
                constraint.projector = c.dofs->getContext()->get<component::linearsolver::Constraint>( core::objectmodel::BaseContext::Local );
                constraint.value = c.dofs->getContext()->get<component::odesolver::BaseConstraintValue>( core::objectmodel::BaseContext::Local );

                // by default the manually given ConstraintValue is used
                // otherwise a fallback is used depending on the constraint type
                if( !constraint.value ) {

                    // a non-compliant (hard) bilateral constraint is stabilizable
                    if( zero(*C) /*|| fillWithZeros(*C)*/ ) constraint.value = new component::odesolver::Stabilization( c.dofs );
                    // by default, a compliant (elastic) constraint is not stabilized
                    else constraint.value = new component::odesolver::ConstraintValue( c.dofs );
                    constraint.value->setName("constraint");

                    c.dofs->getContext()->addObject( constraint.value );
                    constraint.value->init();
                }

                res.constraints.push_back( constraint );

				// mapping
                {
                    // scoped::timer step("assembly: set mapping");
                    copy_triplets(std::back_inserter(Js), Jc, off_c);
                    // res.J.middleRows(off_c, c.size) = Jc;
                }
                
                // compliance
                if( !zero( *C ) ) {
                    // scoped::timer step("assembly: add_C");
                    copy_triplets(std::back_inserter(Cs), *C, off_c, off_c, c_factor);                    
                    // add_C(*C, off_c, c_factor);
                }
                
				off_c += c.size;
			}
		}
	}
    }

    res.J.setFromTriplets(Js.begin(), Js.end());
    res.C.setFromTriplets(Cs.begin(), Cs.end());
    
    assert( off_m == _processed->size_m );
    assert( off_c == _processed->size_c );

}

// TODO checks should go to is_master/is_compliant
// // TODO redo
// bool AssemblyVisitor::chunk::check() const {

// 	// let's be paranoid
// 	assert( dofs && size == dofs->getMatrixSize() );

// 	if( master() ) {
//         assert( !notempty(C) );
// 		assert( !empty(P) );

// 		// TODO size checks on M, J, ...

// 	} else {

//         if(notempty(C)) {
//             assert( C->rows() == int(size) );
// 		}

// 	}

// 	return true;
// }


void AssemblyVisitor::clear() {

	chunks.clear();
	prefix.clear();
	graph.clear();

}



Visitor::Result AssemblyVisitor::processNodeTopDown(simulation::Node* node) {
    if( !start_node )
    {
        start_node = node;
        isPIdentity = true;
        root = start_node;
    }

	if( node->mechanicalState ) fill_prefix( node );
    else if( !node->interactionForceField.empty() ) interactionForceField( node );
	return RESULT_CONTINUE;
}

void AssemblyVisitor::processNodeBottomUp(simulation::Node* node) {
	if( node->mechanicalState ) fill_postfix( node );

	// are we finished yo ?
	if( node == start_node ) {

		// backup prefix traversal order
		utils::dfs( graph, prefix_helper( prefix ) );

		// postfix mechanical flags propagation (and geometric stiffness matrices)
        std::for_each(prefix.rbegin(), prefix.rend(), propagation_helper(mparams, graph) );

		// TODO at this point it could be a good thing to prune
		// non-mechanical nodes in the graph, in order to avoid unneeded
		// mapping concatenations, then rebuild the prefix order

		start_node = 0;
	}
}







}
}

