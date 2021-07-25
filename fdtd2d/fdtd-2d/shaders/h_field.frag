#version 330 core

precision highp float;

varying vec2 st;
uniform float mu;
uniform float sigma;
uniform float dt;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform sampler2D sourceTex;
uniform sampler2D EFieldTex;
uniform sampler2D HFieldTex;

void main() {
    float du = dx/width;
    float dv = dy/height;
    vec3 dEdx = (texture2D(EFieldTex, vec2(st.x + du, st.y)).xyz
                 - texture2D(EFieldTex, vec2(st.x, st.y).xyz))/dx;
    vec3 dEdy = (texture2D(EFieldTex, vec2(st.x, st.y + dv)).xyz
                 - texture2D(EFieldTex, vec2(st.x, st.y).xyz))/dy;
    vec3 curlE = vec3(-dEdy.z, dEdx.z, dEdx.y - dEdy.x);
    vec3 source = texture2D(source, st).xyz;
    vec3 H1 = texture2D(HFieldTex, st).xyz;
    vec3 H2 = dt*(curlE - (source + sigma*H1))/mu + H1;
}