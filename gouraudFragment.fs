#version 330 core
out vec4 FragColor;

in vec3 vertColor;    // Interpolated per-vertex lighting result
in vec2 TexCoord;

// Texture
uniform sampler2D texture1;
uniform bool useTexture;
uniform int renderMode;    // 0=material, 1=texture, 2=blended
uniform vec2 uvTiling;

uniform bool useObjectColor;
uniform vec3 objectColor;
uniform float alpha;

void main()
{
    // Simple color mode
    if (useObjectColor)
    {
        FragColor = vec4(objectColor, alpha);
        return;
    }

    vec3 finalColor = vertColor;

    // Apply texture based on renderMode
    if (useTexture && renderMode != 0)
    {
        vec3 texColor = texture(texture1, TexCoord * uvTiling).rgb;

        if (renderMode == 1)
        {
            // Texture only: modulate vertex lighting with texture color
            // We approximate by using texture color as the surface color
            float luminance = dot(vertColor, vec3(0.2126, 0.7152, 0.0722));
            finalColor = texColor * max(luminance * 2.0, 0.3);
        }
        else // renderMode == 2 (blended)
        {
            float luminance = dot(vertColor, vec3(0.2126, 0.7152, 0.0722));
            vec3 texLit = texColor * max(luminance * 2.0, 0.3);
            finalColor = mix(vertColor, texLit, 0.5);
        }
    }

    FragColor = vec4(finalColor, alpha);
}
