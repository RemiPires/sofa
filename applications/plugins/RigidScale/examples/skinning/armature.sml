<?xml version="1.0" ?>
<!DOCTYPE sml SYSTEM "sml.dtd">
<sml name="armature">
    <units length="m" mass="kg" time="s"/>
    <!--Meshes-->
    <mesh id="Cube_mesh">
        <source format="obj">./obj/armature_Cube_mesh.obj</source>
        <group id="Bone">
            <index>8 9 10 11 136 168 169 171 234 235 252 253 254 255 256 257 258 259 260 261 262 263 264 265 266 267 268 269 270 271 272 273 274 275 276 277 </index>
            <data name="weight" type="float">1.000000 1.000000 1.000000 1.000000 0.064300 0.216403 0.775814 0.070596 0.051586 0.058879 0.999999 0.999999 0.999999 0.999999 0.999990 0.999995 0.999996 0.999994 0.999910 0.999963 0.999972 0.999949 0.999184 0.999726 0.999794 0.999596 0.992492 0.998122 0.998509 0.996833 0.929916 0.988713 0.989352 0.975973 0.873295 0.923391 </data>
        </group>
        <group id="Bone_001">
            <index>82 83 126 127 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 168 172 173 235 238 </index>
            <data name="weight" type="float">0.054809 0.052102 0.068431 0.078622 0.178397 0.768241 0.204785 0.721100 0.896768 0.961893 0.902513 0.948389 0.985665 0.992295 0.986747 0.991070 0.985418 0.987892 0.989452 0.990327 0.897906 0.915980 0.933273 0.939206 0.070483 0.004368 0.082679 0.070612 0.060435 </data>
        </group>
        <group id="Bone_002">
            <index>52 53 68 69 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 127 130 154 155 </index>
            <data name="weight" type="float">0.076800 0.090627 0.075896 0.088708 0.218384 0.782391 0.924664 0.888857 0.911428 0.969699 0.988086 0.984226 0.988338 0.994333 0.996720 0.996002 0.985085 0.987683 0.988015 0.989002 0.887515 0.908867 0.906945 0.919723 0.080126 0.061309 0.058084 0.052315 </data>
        </group>
        <group id="Bone_008">
            <index>40 41 42 43 54 55 68 69 70 71 72 73 74 75 76 77 78 79 98 99 </index>
            <data name="weight" type="float">0.999416 0.999346 0.999159 0.998993 0.126971 0.144163 0.914002 0.892346 0.811783 0.765844 0.985688 0.983595 0.973177 0.968112 0.997676 0.997474 0.996136 0.995564 0.083973 0.077348 </data>
        </group>
        <group id="Bone_009">
            <index>44 45 46 47 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 70 71 75 96 97 278 279 280 281 </index>
            <data name="weight" type="float">0.484396 0.512530 0.775981 0.211928 0.907286 0.892443 0.863427 0.851992 0.989405 0.988544 0.985887 0.984575 0.992691 0.995076 0.994453 0.988027 0.939957 0.965336 0.964561 0.884950 0.168094 0.219317 0.007934 0.100225 0.076569 0.025880 0.065324 0.132842 0.027745 </data>
        </group>
        <group id="Bone_010">
            <index>44 45 46 47 48 49 50 51 64 65 66 67 278 279 280 281 282 283 284 285 286 287 288 289 </index>
            <data name="weight" type="float">0.515600 0.487468 0.223991 0.788051 0.999880 0.999813 0.999762 0.999850 0.059938 0.019113 0.020601 0.114891 0.962059 0.934674 0.867153 0.961122 0.995296 0.992228 0.986224 0.994756 0.999414 0.999081 0.998543 0.999324 </data>
        </group>
        <group id="Bone_012">
            <index>80 81 83 84 85 102 103 113 126 127 128 129 130 131 132 133 134 135 152 153 </index>
            <data name="weight" type="float">0.767691 0.199626 0.058943 0.086068 0.004504 0.139985 0.096625 0.003824 0.907969 0.840762 0.984506 0.980078 0.926309 0.964966 0.961592 0.962378 0.885893 0.862647 0.092200 0.067480 </data>
        </group>
        <group id="Bone_014">
            <index>32 33 34 35 100 101 102 103 104 112 113 114 115 116 117 118 119 120 121 122 123 124 125 </index>
            <data name="weight" type="float">0.999957 0.999928 0.999876 0.999919 0.783826 0.168817 0.013547 0.078881 0.078262 0.944079 0.880342 0.990679 0.978185 0.917922 0.971560 0.998629 0.997214 0.991952 0.996606 0.999811 0.999659 0.999196 0.999603 </data>
        </group>
        <group id="Bone_015">
            <index>36 37 38 39 100 101 102 103 104 105 106 107 108 109 110 111 112 113 116 117 134 135 </index>
            <data name="weight" type="float">0.998756 0.999280 0.999495 0.999254 0.202822 0.817342 0.827514 0.824203 0.919485 0.974312 0.979566 0.972363 0.991932 0.996712 0.997524 0.996531 0.012171 0.092699 0.080425 0.003799 0.103059 0.120988 </data>
        </group>
        <group id="Bone_003">
            <index>24 25 26 27 137 138 139 142 143 218 219 220 221 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 276 277 </index>
            <data name="weight" type="float">0.485794 0.513160 0.786710 0.210061 0.057207 0.767241 0.202442 0.089783 0.007065 0.035264 0.057142 0.105840 0.016080 0.921595 0.863046 0.989603 0.985155 0.932463 0.975536 0.997989 0.997512 0.993040 0.996167 0.993658 0.995201 0.995292 0.989732 0.944753 0.958837 0.968022 0.898836 0.059028 0.050904 </data>
        </group>
        <group id="Bone_004">
            <index>24 25 26 27 202 203 210 211 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 248 249 250 251 </index>
            <data name="weight" type="float">0.514198 0.486832 0.213265 0.789917 0.060951 0.052818 0.078765 0.075487 0.957225 0.942715 0.894052 0.966853 0.993696 0.991967 0.988284 0.994701 0.986724 0.986761 0.991790 0.991981 0.882313 0.884639 0.942119 0.937949 0.055227 0.032269 0.013825 0.101123 </data>
        </group>
        <group id="Bone_005">
            <index>20 21 22 23 204 205 208 210 211 212 213 214 215 216 217 230 231 </index>
            <data name="weight" type="float">0.997926 0.997650 0.996748 0.996799 0.243802 0.147668 0.008313 0.901548 0.898436 0.794669 0.829198 0.988863 0.988378 0.979496 0.982302 0.108188 0.105742 </data>
        </group>
        <group id="Bone_006">
            <index>16 17 18 19 190 191 192 193 202 203 204 205 206 207 208 209 211 212 213 232 233 </index>
            <data name="weight" type="float">0.488068 0.514207 0.780217 0.221576 0.039368 0.058160 0.106019 0.020692 0.915513 0.926489 0.739178 0.831760 0.924975 0.939823 0.929280 0.856261 0.001495 0.191732 0.159552 0.049868 0.053240 </data>
        </group>
        <group id="Bone_007">
            <index>12 13 14 15 16 17 18 19 190 191 192 193 194 195 196 197 198 199 200 201 206 207 208 209 </index>
            <data name="weight" type="float">0.999875 0.999828 0.999796 0.999860 0.511245 0.485268 0.214604 0.774953 0.955202 0.941658 0.893267 0.964097 0.994695 0.992995 0.988711 0.995163 0.999362 0.999163 0.998782 0.999374 0.064434 0.049289 0.029268 0.121773 </data>
        </group>
        <group id="Bone_011">
            <index>0 1 2 3 4 5 6 7 136 137 139 140 141 157 158 159 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 272 276 </index>
            <data name="weight" type="float">0.073033 0.075670 0.312174 0.244146 0.926618 0.923981 0.685206 0.753794 0.738123 0.156286 0.063051 0.089898 0.003218 0.000700 0.081599 0.066082 0.702715 0.193547 0.952214 0.921866 0.967439 0.907152 0.993387 0.990892 0.994895 0.989317 0.998623 0.998383 0.998101 0.997742 0.996182 0.995880 0.989587 0.991202 0.974146 0.972457 0.910354 0.927483 0.062114 0.059954 </data>
        </group>
        <group id="Bone_013">
            <index>0 1 2 3 4 5 6 7 28 29 30 31 156 157 158 159 160 161 162 163 164 165 166 167 186 187 188 189 </index>
            <data name="weight" type="float">0.926967 0.924329 0.687824 0.755852 0.073377 0.076014 0.314789 0.246201 0.999908 0.999894 0.999831 0.999834 0.976218 0.974650 0.918401 0.933918 0.996774 0.996524 0.991119 0.992540 0.999573 0.999539 0.999018 0.999148 0.001676 0.005051 0.089631 0.072497 </data>
        </group>
    </mesh>
    <!-- Skinning -->
    <solid id="solid_Cube">
        <position>0 0 0 0 0 0 1</position>
        <mesh id="Cube_mesh"/>
        <skinning group="Bone" mesh="Cube_mesh" solid="Bone" weight="weight"/>
        <skinning group="Bone_001" mesh="Cube_mesh" solid="Bone_001" weight="weight"/>
        <skinning group="Bone_002" mesh="Cube_mesh" solid="Bone_002" weight="weight"/>
        <skinning group="Bone_008" mesh="Cube_mesh" solid="Bone_008" weight="weight"/>
        <skinning group="Bone_009" mesh="Cube_mesh" solid="Bone_009" weight="weight"/>
        <skinning group="Bone_010" mesh="Cube_mesh" solid="Bone_010" weight="weight"/>
        <skinning group="Bone_012" mesh="Cube_mesh" solid="Bone_012" weight="weight"/>
        <skinning group="Bone_014" mesh="Cube_mesh" solid="Bone_014" weight="weight"/>
        <skinning group="Bone_015" mesh="Cube_mesh" solid="Bone_015" weight="weight"/>
        <skinning group="Bone_003" mesh="Cube_mesh" solid="Bone_003" weight="weight"/>
        <skinning group="Bone_004" mesh="Cube_mesh" solid="Bone_004" weight="weight"/>
        <skinning group="Bone_005" mesh="Cube_mesh" solid="Bone_005" weight="weight"/>
        <skinning group="Bone_006" mesh="Cube_mesh" solid="Bone_006" weight="weight"/>
        <skinning group="Bone_007" mesh="Cube_mesh" solid="Bone_007" weight="weight"/>
        <skinning group="Bone_011" mesh="Cube_mesh" solid="Bone_011" weight="weight"/>
        <skinning group="Bone_013" mesh="Cube_mesh" solid="Bone_013" weight="weight"/>
    </solid>
    <!-- Armature -->
    <solid id="Bone">
        <tag>armature</tag>
        <position>0.000000 0.000000 -0.000000 -0.707107 0.000000 0.000000 0.707107</position>
    </solid>
    <solid id="Bone_001">
        <tag>armature</tag>
        <position>0.000000 1.000000 -0.000000 -0.671784 -0.220694 0.220694 0.671784</position>
        <parent>Bone</parent>
    </solid>
    <solid id="Bone_002">
        <tag>armature</tag>
        <position>0.417630 1.567027 0.000000 -0.697892 -0.113781 0.113781 0.697892</position>
        <parent>Bone_001</parent>
    </solid>
    <solid id="Bone_008">
        <tag>armature</tag>
        <position>0.648516 2.256287 0.000000 -0.695679 0.126614 -0.126614 0.695679</position>
        <parent>Bone_002</parent>
    </solid>
    <solid id="Bone_009">
        <tag>armature</tag>
        <position>0.648516 2.256287 0.000000 -0.561019 -0.430416 0.430416 0.561019</position>
        <parent>Bone_002</parent>
    </solid>
    <solid id="Bone_010">
        <tag>armature</tag>
        <position>1.323243 2.437192 0.000000 -0.703919 0.067070 -0.067070 0.703919</position>
        <parent>Bone_009</parent>
    </solid>
    <solid id="Bone_012">
        <tag>armature</tag>
        <position>0.417630 1.567027 0.000000 -0.512972 -0.486683 0.486683 0.512971</position>
        <parent>Bone_001</parent>
    </solid>
    <solid id="Bone_014">
        <tag>armature</tag>
        <position>0.975014 1.596363 0.000000 -0.618600 -0.342542 0.342542 0.618599</position>
        <parent>Bone_012</parent>
    </solid>
    <solid id="Bone_015">
        <tag>armature</tag>
        <position>0.975014 1.596363 0.000000 -0.704035 -0.065838 0.065838 0.704035</position>
        <parent>Bone_012</parent>
    </solid>
    <solid id="Bone_003">
        <tag>armature</tag>
        <position>0.000000 1.000000 -0.000000 -0.677124 0.203722 -0.203722 0.677124</position>
        <parent>Bone</parent>
    </solid>
    <solid id="Bone_004">
        <tag>armature</tag>
        <position>-0.444793 1.672283 0.000000 -0.702841 -0.077553 0.077553 0.702841</position>
        <parent>Bone_003</parent>
    </solid>
    <solid id="Bone_005">
        <tag>armature</tag>
        <position>-0.302188 2.310613 0.000000 -0.639871 0.300941 -0.300941 0.639871</position>
        <parent>Bone_004</parent>
    </solid>
    <solid id="Bone_006">
        <tag>armature</tag>
        <position>-0.302188 2.310613 0.000000 -0.667768 -0.232563 0.232563 0.667768</position>
        <parent>Bone_004</parent>
    </solid>
    <solid id="Bone_007">
        <tag>armature</tag>
        <position>-0.081489 2.589033 0.000000 -0.706715 0.023531 -0.023531 0.706715</position>
        <parent>Bone_006</parent>
    </solid>
    <solid id="Bone_011">
        <tag>armature</tag>
        <position>0.000000 1.000000 -0.000000 -0.482695 -0.516726 0.516726 0.482695</position>
        <parent>Bone</parent>
    </solid>
    <solid id="Bone_013">
        <tag>armature</tag>
        <position>0.860522 0.941328 -0.000000 -0.682218 -0.185952 0.185952 0.682218</position>
        <parent>Bone_011</parent>
    </solid>
</sml>
