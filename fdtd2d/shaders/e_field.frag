#version 330 core

precision highp float;
varying vec2 st;
uniform float dt;
uniform float dx;
uniform float dy;
uniform float w;
uniform float h;
uniform float epsilon;
uniform float sigma;
uniform sampler2D texEField;
uniform sampler2D texHField;
uniform sampler2D texJ;

void main() {
    vec3 dHdx = texture2D(texHField, vec2(st.x+dx/w, st.y+0.5*dy/h)).xyz/dx
                 - texture2D(texHField, vec2(st.x, st.y+0.5*dy/h)).xyz/dx;
    vec3 dHdy = texture2D(texHField, vec2(st.x+0.5*dx/w, st.y+dy/h)).xyz/dy
                - texture2D(texHField, vec2(st.x+0.5*dx/w, st.y)).xyz/dy;
    vec3 curlH = vec3(dHdy.z, -dHdx.z, dHdx.y - dHdy.x);
    vec3 current = texture2D(texJ, st).xyz;
    vec3 prevE = texture2D(texEField, st).xyz;
    float s = 1.0/(1.0 + 0.5*dt*sigma/epsilon);
    vec3 nextE = s*((dt/epsilon)*(curlH - current)
                     + (1.0 - 0.5*dt*sigma/epsilon)*prevE);
    gl_FragColor = vec4(nextE, 1.0);
}