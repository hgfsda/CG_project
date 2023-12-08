#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;    // 조명 위치
uniform vec3 lightColor;  // 조명 색상
uniform vec3 objectColor; // 객체 색상
uniform vec3 viewPos;     // 카메라 위치

void main ()
{
    vec3 ambientLight = vec3(0.5);
    vec3 ambient = ambientLight * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffuseLight = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseLight * lightColor;

    int shininess = 128;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularLight = max(dot(viewDir, reflectDir), 0.0);
    specularLight = pow(specularLight, shininess);
    vec3 specular = specularLight * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}