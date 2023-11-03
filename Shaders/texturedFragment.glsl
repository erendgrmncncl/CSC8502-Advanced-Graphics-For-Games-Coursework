#version 330 core
uniform float blendRatio;
uniform sampler2D diffuseTex;

in Vertex{
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
    fragColour = texture(diffuseTex, IN.texCoord);
}