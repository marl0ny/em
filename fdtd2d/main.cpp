#include "gl_wrappers.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include "shaders.hpp"
#ifdef __EMSCRIPTEN__
#include <functional>
#include <emscripten.h>
#endif


const double LX = 2.0;
const double LY = 2.0;
const double DT = 0.001;
const double EPSILON = 1.0;
const double MU = 1.0;


struct {
    double x, y;
    double dx, dy;
    bool pressed = false;
    bool released = false;
    int w, h;
    void update(GLFWwindow *window) {
        double x_prev = x;
        double y_prev = y;
        glfwGetFramebufferSize(window, &w, &h);
        glfwGetCursorPos(window, &x, &y);
        x = x/(double)w;
        y = 1.0 - y/(double)h;
        this->dx = x - x_prev;
        this->dy = y - y_prev;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            pressed = true;
        } else {
            if (released) released = false;
            if (pressed) released = true;
            pressed = false;
        }
    }
} left_click;


template <typename T>
void button_update(GLFWwindow *window, int button_key, 
                    T &param, T new_val) {
    if (glfwGetKey(window, button_key) == GLFW_PRESS) {
        param = new_val;
    }
}


#ifdef __EMSCRIPTEN__
std::function<void ()> loop;
void browser_loop();
void browser_loop() {
    loop();
}
#endif


int main(void) {

    const int w = 512, h = 512;
    int aspect = 1;
    GLFWwindow *window = init_window(aspect*w, aspect*h);
    glViewport(0, 0, w, h);
    init_glew();

    GLuint vertex_shader = make_vertex_shader(quad_shader_source);
    GLuint frag_shader = make_fragment_shader(simple_out_shader_source);
    GLuint view_shader = make_fragment_shader(view_shader_source);
    GLuint e_field_shader = make_fragment_shader(e_field_shader_source);
    GLuint h_field_shader = make_fragment_shader(h_field_shader_source);
    GLuint wavepacket_shader = make_fragment_shader(wavepacket_shader_source);

    GLuint view_program = make_program(vertex_shader, view_shader);
    GLuint e_field_program = make_program(vertex_shader, e_field_shader);
    GLuint h_field_program = make_program(vertex_shader, h_field_shader);
    GLuint copy_program = make_program(vertex_shader, frag_shader);
    GLuint wavepacket_progam = make_program(vertex_shader, wavepacket_shader);

    Quad q0 = Quad::make_frame(aspect*w, aspect*h);
    Quad j0 = Quad::make_float_frame(w, h);
    Quad e_field1 = Quad::make_float_frame(w, h);
    Quad e_field2 = Quad::make_float_frame(w, h);
    Quad h_field1 = Quad::make_float_frame(w, h);
    Quad h_field2 = Quad::make_float_frame(w, h);
    Quad *e_fields[2] = {&e_field1, &e_field2};
    Quad *h_fields[2] = {&h_field1, &h_field2};

    j0.set_program(wavepacket_progam);
    j0.bind();
    j0.set_float_uniforms(
        {{"dx", LX/w}, {"dy", LY/h}, {"bx", 0.5}, {"by", 0.5},
         {"sx", 0.01},  {"sy", 0.01}, {"amp", 150.0}, {"rez", 1.0}
        }
    );
    j0.draw();
    unbind();

    auto clear = [&](Quad *q) {
        q->set_program(copy_program);
        q->bind();
        q->set_int_uniform("tex", Quad::get_blank());
        q->draw();
        unbind();
    };

    auto step = [&]() {
        e_fields[1]->set_program(e_field_program);
        e_fields[1]->bind();
        e_fields[1]->set_int_uniforms(
            {{"texEField", e_fields[0]->get_value()}, 
             {"texHField", h_fields[0]->get_value()}, 
             {"texJ", j0.get_value()}}
        );
        e_fields[1]->set_float_uniforms(
            {{"dt", DT}, {"dx", LX/w}, {"dy", LY/h},
             {"w", LX}, {"h", LY},
             {"epsilon", EPSILON}, {"sigma", 0.0}}
        );
        e_fields[1]->draw();
        unbind();
        std::swap(e_fields[0], e_fields[1]);
        h_fields[1]->set_program(h_field_program);
        h_fields[1]->bind();
        h_fields[1]->set_int_uniforms(
            {{"texEField", e_fields[0]->get_value()}, 
             {"texHField", h_fields[0]->get_value()}, 
             {"texSource", Quad::get_blank()}}
        );
        h_fields[1]->set_float_uniforms(
            {{"dt", DT}, {"dx", LX/w}, {"dy", LY/h},
             {"w", LX}, {"h", LY},
             {"mu", MU}, {"sigma", 0.0}}
        );
        h_fields[1]->draw();
        unbind();
        std::swap(h_fields[0], h_fields[1]);
    };

    std::cout << GL_MAX_TEXTURE_SIZE << std::endl;
    std::cout << GL_MAX_3D_TEXTURE_SIZE << std::endl;

    double t = 0.0;
    bool erase = false;
    double f = 60.0;
    double bx = 0.5, by = 0.5;
    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {
        if (left_click.pressed) {
            bx = left_click.x;
            by = left_click.y;
        }
        if (erase) {
            for (auto &u: e_fields) clear(u);
            for (auto &h: h_fields) clear(h);
            erase = false;
        }
        t += DT;
        j0.set_program(wavepacket_progam);
        j0.bind();
        j0.set_float_uniforms(
            {{"dx", LX/w}, {"dy", LY/h}, {"bx", bx}, {"by", by},
            {"sx", 0.01},  {"sy", 0.01}, {"amp", 100.0*cos(f*t)}, {"rez", 1.0}
            }
        );
        j0.draw();
        unbind();
        glViewport(0, 0, aspect*w, aspect*h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int _ = 0; _ < 1; _++) step();
        q0.set_program(view_program);
        q0.bind();
        q0.set_int_uniform("tex", e_fields[0]->get_value());
        q0.draw();
        unbind();
        glfwPollEvents();
        left_click.update(window);
        button_update(window, GLFW_KEY_UP, f, f+1);
        button_update(window, GLFW_KEY_DOWN, f, f-1);
        button_update(window, GLFW_KEY_E, erase, true);
        glfwSwapBuffers(window);
        glViewport(0, 0, w, h);
    };
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(browser_loop, 0, true);
    #else
    while(!glfwWindowShouldClose(window)) {
        loop();
    }
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}
