#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 position;
in vec3 new_normal;

uniform sampler2D texture_diffuse1;
uniform vec4 mat_specular;
uniform float mat_s;
uniform vec3 cam_position;

//Estructura de la luz
struct LightSource{
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
};

LightSource light0 = LightSource(
     vec4(2.0, 2.0, 2.0, 1.0), //Posicion
     vec4(1.0, 1.0, 1.0, 1.0), //Difusividad
     vec4(1.0, 1.0, 1.0, 1.0), //Especularidad
     0.0, 1.0, 0.0
);

vec4 scene_ambient = vec4(0.3, 0.3, 0.3, 1.0);

void main()
{    
	vec3 N = new_normal;
    vec3 totalLight = vec3(scene_ambient) * vec3(0.3, 0.3, 0.3);

    vec3 V = normalize(cam_position - vec3(position));
    vec3 L = vec3(vec3(light0.position) - vec3(position));
    float distance = length(L);

     L = normalize(L);
     float attenuation = 1.0/(light0.constantAttenuation +
                             distance*light0.linearAttenuation+
                             distance*distance*light0.quadraticAttenuation);

    vec3 diffuse_reflection = attenuation * vec3(light0.diffuse) * vec3(texture(texture_diffuse1, TexCoords)) * max(0.0, dot(N,L));
    totalLight = totalLight + diffuse_reflection;
    FragColor = vec4(totalLight, 1.0);
}