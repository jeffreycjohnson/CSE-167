#version 330
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

		uniform mat4 uM_Matrix;
		uniform mat4 uV_Matrix;
		uniform mat4 uP_Matrix;
		
        out vec4 vPosition;
		out vec3 vNormal;
        out vec2 vTexCoord;

		void main () {
		  vNormal = mat3(uM_Matrix) * aNormal;
          vTexCoord = aTexCoord;
          vPosition = uM_Matrix * aPosition;
		  gl_Position = uP_Matrix * uV_Matrix * vPosition;
		}
