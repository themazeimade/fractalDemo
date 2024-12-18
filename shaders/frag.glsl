#version 330 core
in vec2 TexCoord;

// uniform float ourValue;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform vec2 tex1_scale;
uniform vec2 tex1_offset;
uniform vec2 tex2_scale;
uniform vec2 tex2_offset;
uniform float alpha;
// uniform vec2 center;

out vec4 FragColor;
// vec3 rgb2hsv(vec3 c)
// {
//     vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
//
//     float d = q.x - min(q.w, q.y);
//     float e = 1.0e-10;
//     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
// }
//
// vec3 hsv2rgb(vec3 c)
// {
//     vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
//     vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
//     return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
// }

void main()
{
    vec2 scaleCenter = vec2(0.5, 0.5);
    //
    vec2 tex1Coord = ((TexCoord - scaleCenter) / tex1_scale) + scaleCenter + tex1_offset;
    // vec2 tex1Coord = ((TexCoord - scaleCenter) / tex1_scale)  + tex1_offset;
    // vec2 tex1Coord = TexCoord;
    //
    vec2 tex2Coord = ((TexCoord - scaleCenter) / tex2_scale) + scaleCenter + tex2_offset;
    // vec2 tex2Coord = ((TexCoord - scaleCenter) / tex2_scale)  + tex2_offset;
    // vec2 tex2Coord = TexCoord;
    //
    vec4 background = texture(tex1, tex1Coord);
    vec4 foreground = texture(tex2, tex2Coord);

    vec3 blendedColor = foreground.rgb * alpha + background.rgb * (1.0 - alpha);

    // FragColor = texture(tex1, TexCoord);
    // FragColor = vec4(0.0,1.0,0.0,0.2);
    // blend shit
    FragColor = vec4(blendedColor, 1.0);
    // vec3 hsvObject = rgb2hsv(ourColor);
    // hsvObject.z = ourValue;
    // vec3 outtie = hsv2rgb(hsvObject);
    // FragColor = vec4(outtie, 1.0f);
}
