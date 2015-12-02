#version 440
precision mediump float;
in vec4 vPosition;

out vec4 frag_color;

const float PI = 3.14159265359;

uniform sampler2D colorTex; //color texture - rgb: color | a: metalness
uniform sampler2D normalTex; //normal texture - rgb: normal | a: IOR
uniform sampler2D posTex; //position texture - rgb: position | a: roughness

//world space camera position, to get view vector
uniform vec3 cameraPos;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec3 uLightDirection;
uniform float uLightSize = 1.0f;
uniform vec2 uScreenSize;

float GGX_Visibility(float dotProduct, float k) {
	//return 2.0 / (dotProduct + sqrt(k*k + (1 - k*k)*dotProduct*dotProduct)); //More accurate, but slower version

	k=k/2;
	return 1.0 / (dotProduct * (1.0 - k) + k);
}

float GGX_D(float dotNH, float a) {
	a = clamp(a, 0.001, 1.0); //prevent a (and a2) from being too close to zero
	float a2 = a*a;
	float bot = (dotNH * dotNH * (a2 - 1.0) + 1.0);
	return (a2) / (PI * bot * bot);
}

vec3 SpecularBRDF(vec3 lightColor, vec3 normal, vec3 view, vec3 lightDir, float a, vec3 F0, float d) {
		vec3 halfVec = normalize(view + lightDir);
		
		float dotNL = clamp(dot(normal, lightDir), 0.0, 1.0);

		float dotNV = clamp(dot(normal, view), 0.0, 1.0);
		float dotLH = clamp(dot(lightDir, halfVec), 0.0, 1.0);

		vec3 F = F0 + (vec3(1,1,1)-F0) * pow(1-dotLH, 5);

		float k = clamp(a+.36, 0.0, 1.0);
		float G = GGX_Visibility(dotNV, k) * GGX_Visibility(dotNL, k) * d + (1-d);

		return F * lightColor * (G * dotNL);
}

void main () {
  vec4 albedo = texture(colorTex, gl_FragCoord.xy / uScreenSize);
  vec4 pos = texture(posTex, gl_FragCoord.xy / uScreenSize);
  vec4 normal = texture(normalTex, gl_FragCoord.xy / uScreenSize);
  vec3 mat = vec3(albedo.a, pos.w, normal.w);

  vec3 view = normalize(cameraPos - vPosition.xyz);


  mat.y += 0.01; //there seem to be issues with roughness = 0 due to visibility
  float a = sqrt(mat.y);// squaring it makes everything shiny, sqrting it looks like linear roughness

  float IOR = mat.z;
  //F0 is essentially specular color, as well as Fresnel term
  vec3 F0 = vec3(1,1,1) * pow((1.0 - IOR) / (1.0 + IOR), 2);
  F0 = mix(F0, albedo.xyz, mat.r); //interpolate Fresnel with the color as metalness increases (with metalness=1, color => reflection color)
  F0 = mix(vec3(1,1,1) * dot(vec3(.33,.33,.33),F0), F0, mat.r); //my own improvement - could be wrong : desaturates Fresnel as metalness decreases

  vec3 lightDir =  (uLightPosition - pos.xyz).xyz;
  float lightDist = length(lightDir);

  //Spherical light algorithm from http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
  float sphereRadius = uLightSize;
  vec3 reflectedRay = reflect(-view, normal.xyz);
  vec3 centerToRay = dot(lightDir, reflectedRay) * reflectedRay - lightDir;
  lightDir = normalize(lightDir + centerToRay * clamp(sphereRadius / length(centerToRay), 0.0, 1.0));
  //todo normalize based on sphere size


  float power = 1.0 / (lightDist * lightDist + 1.0);
  vec3 diffuseLight = uLightColor * (clamp(dot(lightDir, normal.xyz) * power, 0.0, 1.0));
	
  vec3 halfVec = normalize(view + lightDir);
  float dotNH = clamp(dot(normal.xyz, halfVec), 0.0, 1.0);

  vec3 specColor = GGX_D(dotNH, a) * SpecularBRDF(uLightColor, normal.xyz, view, lightDir, a, F0, 1) * power;

  vec3 diffuseColor = ((1.0-mat.r) * albedo.xyz) * diffuseLight;
  vec3 color = diffuseColor + specColor;
  

  frag_color = vec4(color, 1.0);
}
