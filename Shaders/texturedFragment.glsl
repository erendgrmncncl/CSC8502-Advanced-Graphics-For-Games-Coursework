#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D secondTex;

in Vertex{
    vec2 texCoord;
    vec4 colour;
} IN;

out vec4 fragColour;

void main(void){
    fragColour = mix(texture(diffuseTex, IN.texCoord), texture(secondTex, IN.texCoord), 0.5);
}