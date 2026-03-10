#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
//in vec3 color;

//in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;

//uniform sampler2D tex0;
uniform vec4 color;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambient;
uniform vec3 viewPos;
uniform float shininess;
uniform float specularStrength;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightDir);
    // diffuse (Lambert)
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular (Phong)
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * color.rgb;
    FragColor = vec4(result, color.a);
//   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//   FragColor = texture(tex0, texCoord);
}