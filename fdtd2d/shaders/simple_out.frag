#version 330 core

precision highp float;
varying vec2 st;
uniform sampler2D tex;

void main() {
    highp vec4 col = texture2D(tex, st);
    gl_FragColor = col;
}