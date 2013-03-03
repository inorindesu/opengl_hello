#version 120

varying vec2 UV;

uniform vec3 backColor;
uniform vec3 foreColor;
uniform sampler2D myTexture;

void main()
{
        float textureAlpha = 1 - texture2D(myTexture, UV).r;

        // Do manual alpha blending on square
        // Alpha blending done here will not affect alpha blending between this square
        //  and OpenGL scene
        vec3 finalColor = backColor * (1 - textureAlpha) + foreColor * textureAlpha;

        gl_FragColor = vec4(finalColor, 1.0f);
}
