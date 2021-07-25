#ifndef _SHADERS_H_
#define _SHADERS_H_
const char *view_shader_source = R"(

precision highp float;
varying vec2 st;
uniform sampler2D tex;


void main() {
    vec4 col = texture2D(tex, st);
    gl_FragColor = vec4(-col.b + col.r, col.g, col.b - col.r, 1.0);
})";
const char *quad_shader_source = R"(

precision highp float;
attribute vec3 position;
varying vec2 st;

void main () {
    gl_Position = vec4(position.xyz, 1.0);
    st = vec2(0.5, 0.5) + position.xy/2.0;
}
)";
const char *e_field_shader_source = R"(

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
})";
const char *simple_out_shader_source = R"(

precision highp float;
varying vec2 st;
uniform sampler2D tex;

void main() {
    highp vec4 col = texture2D(tex, st);
    gl_FragColor = col;
})";
const char *wavepacket_shader_source = R"(

precision highp float;
varying vec2 st;
uniform float dx;
uniform float dy;
uniform float bx;
uniform float by;
uniform float px;
uniform float py;
uniform float sx;
uniform float sy;
uniform float amp;
uniform float rex;
uniform float rey;
uniform float rez;
uniform float imx;
uniform float imy;
uniform float imz;

const float sqrt2 = 1.4142135623730951;
const float sqrtpi = 1.7724538509055159;
const float pi = 3.141592653589793;

void main() {
    float x = st.x;
    float y = st.y;
    float u = ((x - bx)/(sx*sqrt2));
    float v = ((y - by)/(sy*sqrt2));
    float re = amp*exp(-u*u - v*v)*cos(2.0*pi*(px*x + py*y));
    float im = amp*exp(-u*u - v*v)*sin(2.0*pi*(px*x + py*y));
    gl_FragColor = vec4(rex*re + imx*im, 
                        rey*re + imy*im, 
                        rez*re + imz*im, 
                        1.0);
}
)";
const char *h_field_shader_source = R"(

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
})";
#endif