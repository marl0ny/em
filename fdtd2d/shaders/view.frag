#version 330 core

precision highp float;
varying vec2 st;
uniform sampler2D tex;


void main() {
    vec4 col = texture2D(tex, st);
    gl_FragColor = vec4(-col.b + col.r, col.g, col.b - col.r, 1.0);
}