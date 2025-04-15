#version 330 core

in vec2 v_uv;

out vec4 FragColor;

uniform sampler2D u_texture;

const float offset = 1. / 300.;

void kernal()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0., offset),
        vec2(offset, offset),
        vec2(-offset, 0.),
        vec2(0., 0.),
        vec2(offset, 0.),
        vec2(-offset, -offset),
        vec2(0., -offset),
        vec2(offset, -offset)
    );
    float kernal[9] = float[](
        2., 2., 2.,
        2., -15., 2.,
        2., 2., 2.
    );
    //    float kernal[9] = float[](
    //        -1., -1., -1.,
    //        -1., 9., -1.,
    //        -1., -1., -1.
    //    );
    //    float kernal[9] = float[](
    //        1., 1., 1.,
    //        1., -8., 1.,
    //        1., 1., 1.
    //    );
    //    float kernal[9] = float[](
    //        1. / 16., 2. / 16., 1. / 16.,
    //        2. / 16., 4. / 16., 2. / 16.,
    //        1. / 16., 2. / 16., 1. / 16.
    //    );
    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(u_texture, v_uv + offsets[i]));
    vec3 col = vec3(0.);
    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernal[i];
    FragColor = vec4(col, 1.);
}

void main()
{
//    FragColor = vec4(vec3(texture(u_texture, v_uv).r), 1.);
    FragColor = vec4(texture(u_texture, v_uv).rgb, 1.);
//    FragColor.rgb = 1. - FragColor.rgb;

    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.;
//    FragColor = vec4(mix(FragColor.rgb, vec3(average), 1.-average), 1.);
//    FragColor = vec4(mix(FragColor.rgb, 1. - FragColor.rgb, pow(average, 3.)), 1.);

//    if (average > .5)
//    {
////        average = 1. - average;
////        FragColor = vec4(1. - texture(u_texture, v_uv).rgb, 1.);
//    }
//    if (v_uv.x > .5)
//        average = .2126 * FragColor.r + .7152 * FragColor.g + .0722 * FragColor.b;
    FragColor = vec4(vec3(average), 1.);

//    kernal();
}