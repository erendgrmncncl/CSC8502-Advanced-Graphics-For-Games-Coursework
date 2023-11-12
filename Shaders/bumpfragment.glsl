#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform vec4 specularColour;
uniform float lightRadius;

in Vertex
{
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfWayDir = normalize(incident + viewDir);

    mat3 TBN = mat3(normalize(IN.tangent),
                    normalize(IN.binormal),
                    normalize(IN.normal));

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
    bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

    float lambert = max(dot(incident, bumpNormal), 0.0f);
    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance/lightRadius, 0.0, 1.0);

    float specFactor = clamp(dot(halfWayDir, bumpNormal) ,0.0, 1.0);
    specFactor = pow(specFactor, 60.0); //Change it to uniform or get if from sampling the gloss texture

    vec3 surface = (diffuse.rgb * lightColour.rgb);
    fragColor.rgb = surface.rgb * lambert * attenuation;
    fragColor.rgb += (specularColour.rgb * specFactor) * attenuation * 0.33; //Optional to to slightly lowering the brighness
    fragColor.rgb += surface * 0.1f; // ambiant light
    fragColor.a = diffuse.a;
}