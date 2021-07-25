#version 330 core

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
