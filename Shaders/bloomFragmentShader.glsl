#version 330 core

in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2D u_sceneTexture;

void main()
{
    // Extract the scene color
    vec3 sceneColor = texture(u_sceneTexture, v_TexCoords).rgb;

    // Bloom parameters (you might need to adjust these)
    float bloomThreshold = 0.8; // Adjust this value to control when the bloom starts
    float bloomIntensity = 1.5; // Adjust this value to control the strength of the bloom

    // Calculate the bloom contribution
    vec3 bloomColor = max(sceneColor - bloomThreshold, 0.0);
    bloomColor = bloomIntensity * bloomColor * bloomColor; // Apply some additional brightness to the bloom

    // Combine the scene color and bloom color
    vec3 finalColor = sceneColor + bloomColor;

    FragColor = vec4(finalColor, 1.0);
}