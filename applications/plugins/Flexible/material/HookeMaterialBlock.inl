/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Plugins                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef FLEXIBLE_HookeMaterialBlock_INL
#define FLEXIBLE_HookeMaterialBlock_INL

#include "HookeMaterialBlock.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include "StrainTypes.h"

namespace sofa
{

namespace defaulttype
{

//////////////////////////////////////////////////////////////////////////////////
////  macros
//////////////////////////////////////////////////////////////////////////////////
#define E331(type)  StrainTypes<3,3,1,type>
#define E332(type)  StrainTypes<3,3,2,type>
#define E333(type)  StrainTypes<3,3,3,type>

//////////////////////////////////////////////////////////////////////////////////
////  helpers
//////////////////////////////////////////////////////////////////////////////////


template<class Real>
void getLame(const Real &youngModulus,const Real &poissonRatio,Real &lambda,Real &mu)
{
    lambda= youngModulus*poissonRatio/((1-2*poissonRatio)*(1+poissonRatio));
    mu = youngModulus/(2*(1+poissonRatio));
}



//////////////////////////////////////////////////////////////////////////////////
////  F331
//////////////////////////////////////////////////////////////////////////////////

template<class _Real>
class HookeMaterialBlock< E331(_Real) > :
    public  BaseMaterialBlock< E331(_Real) >
{
public:
    typedef E331(_Real) T;

    typedef BaseMaterialBlock<T> Inherit;
    typedef typename Inherit::Coord Coord;
    typedef typename Inherit::Deriv Deriv;
    typedef typename Inherit::MatBlock MatBlock;
    typedef typename Inherit::Real Real;

    enum { material_dimensions = T::material_dimensions };
    enum { strain_size = T::strain_size };
    enum { spatial_dimensions = T::spatial_dimensions };
    typedef typename T::StrainVec StrainVec;

    /**
      * stress = lambda.trace(strain) I  + 2.mu.strain
      *        = H.strain
      * U = vol*stress.strain/2
      * f = - H.vol.strain - viscosity.vol.strainRate
      * df = (- H.vol.kfactor - viscosity.vol.bfactor) dstrain
      */

    Real lambdaVol;  ///< Lamé first coef * volume
    Real mu2Vol;  ///< Lamé second coef * 2 * volume
    Real viscosityVol;  ///< stress/strain rate  * volume

    void init(const Real &youngModulus,const Real &poissonRatio,const Real &visc,const Real &vol)
    {
        getLame(youngModulus,poissonRatio,lambdaVol,mu2Vol);
        lambdaVol*=vol;
        mu2Vol*=(Real)2.*vol;
        viscosityVol=visc*vol;
    }

    Real getPotentialEnergy(const Coord& x) const
    {
        StrainVec stress=x.getStrain()*mu2Vol;
        Real tce=(x.getStrain()[0]+x.getStrain()[1]+x.getStrain()[2])*lambdaVol;
        for(unsigned int i=0; i<strain_size; i++) stress[i]+=tce;
        Real U=dot(stress,x.getStrain())/(Real)2.;
        return U;
    }

    void addForce( Deriv& f , const Coord& x , const Deriv& v)
    {
        f.getStrain()-=x.getStrain()*mu2Vol + v.getStrain()*viscosityVol;
        Real tce=(x.getStrain()[0]+x.getStrain()[1]+x.getStrain()[2])*lambdaVol;
        for(unsigned int i=0; i<strain_size; i++) f.getStrain()[i]-=tce;
    }

    void addDForce( Deriv&   df , const Deriv&   dx, const double& kfactor, const double& bfactor )
    {
        df.getStrain()-=dx.getStrain()*mu2Vol*kfactor + dx.getStrain()*viscosityVol*bfactor;
        Real tce=(dx.getStrain()[0]+dx.getStrain()[1]+dx.getStrain()[2])*lambdaVol*kfactor;
        for(unsigned int i=0; i<strain_size; i++) df.getStrain()[i]-=tce;
    }


    MatBlock getK()
    {
        MatBlock K;
        return K;
    }

    MatBlock getC()
    {
        MatBlock C;
        return C;
    }
};




} // namespace defaulttype
} // namespace sofa



#endif
