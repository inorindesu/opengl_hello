#version 120

varying vec2 UV;

uniform vec3 backColor;
uniform sampler2D myTexture;

void main()
{
        vec3 textureColor = texture2D(myTexture, UV).rgb;
        float textureAlpha = texture2D(myTexture, UV).a;

        // Do manual alpha blending on square
        // Alpha blending done here will not affect alpha blending between this square
        //  and OpenGL scene
        vec3 finalColor = backColor * (1 - textureAlpha) + textureColor * textureAlpha;

        gl_FragColor = vec4(finalColor, 1.0f);
}
