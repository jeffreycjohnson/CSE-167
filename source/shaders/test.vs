#version 330
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

		uniform mat4 uMV_Matrix;
		uniform mat4 uP_Matrix;

		out vec3 vNormal;
        out vec2 vTexCoord;

		void main () {
		  vNormal = aNormal;
          vTexCoord = aTexCoord;
		  gl_Position = uP_Matrix * uMV_Matrix * aPosition;
		}
