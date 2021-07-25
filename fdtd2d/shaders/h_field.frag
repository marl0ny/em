#version 330 core

precision highp float;
varying vec2 st;
uniform float dt;
uniform float dx;
uniform float dy;
uniform float w;
uniform float h;
uniform float mu;
uniform float sigma;
uniform sampler2D texEField;
uniform sampler2D texHField;
uniform sampler2D texSource;

void main() {
    vec3 dEdx = texture2D(texEField, vec2(st.x, st.y-0.5*dy/h)).xyz/dx
                - texture2D(texEField, vec2(st.x-dx/w, st.y-0.5*dy/h)).xyz/dx;
    vec3 dEdy = texture2D(texEField, vec2(st.x-0.5*dx/w, st.y)).xyz/dy
                - texture2D(texEField, vec2(st.x-0.5*dx/w, st.y-dy/h)).xyz/dy;
    vec3 curlE = vec3(dEdy.z, -dEdx.z, dEdx.y - dEdy.x);
    vec3 source = texture2D(texSource, st).xyz;
    vec3 prevH = texture2D(texHField, st).xyz;
    float s = 1.0/(1.0 + 0.5*sigma*dt/mu);
    vec3 nextH = s*((-dt/mu)*(curlE + source) + (1.0 - 0.5*sigma*dt/mu)*prevH);
    gl_FragColor = vec4(nextH, 1.0);
}