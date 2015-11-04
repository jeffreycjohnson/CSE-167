#version 150
		in vec4 aPosition;
		in vec3 aNormal;

		uniform mat4 uMV_Matrix;
		uniform mat4 uP_Matrix;

		out vec3 vNormal;

		void main () {
		  vNormal = aNormal;
		  gl_Position = uP_Matrix * uMV_Matrix * aPosition;
		}
