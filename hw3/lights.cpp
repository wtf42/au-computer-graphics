#include "common.h"
#include "main.h"

void sample_t::init_lights() {
    lights = {
        { // ambient
            vec3(0, 10, 0),
            100.f,
            vec3(0.2f, 0.2f, 0.2f),
            vec3(0, 0, 0),
            vec3(0, 0, 0),
            true
        },
        { // neutral
            vec3(0, 1, 0),
            1.1f,
            vec3(0.1f, 0.1f, 0.1f),
            vec3(0.3f, 0.3f, 0.3f),
            vec3(0.1f, 0.1f, 0.1f),
            true
        },
        { // red
            vec3(0, 0, 0),
            0.3f,
            vec3(0.3f, 0, 0),
            vec3(0.3f, 0, 0),
            vec3(0.3f, 0, 0),
            true
        },
        { // green
            vec3(0, 0, 0),
            0.5f,
            vec3(0, 0.3f, 0),
            vec3(0, 0.3f, 0),
            vec3(0, 0.3f, 0),
            true
        },
        { // blue
            vec3(0, 0, 0),
            0.3f,
            vec3(0, 0, 0.3f),
            vec3(0, 0, 0.3f),
            vec3(0, 0, 0.3f),
            true
        }
    };
}

void sample_t::toggle_light(int idx) {
    if (idx >= 0 && idx < (int)lights.size()) {
        lights[idx].enabled = !lights[idx].enabled;
    }
}

struct light_params_t {
    double A, B, a, b, d;
    vec3 zero, dx, dy;

    void update(light_t &light, double t) {
        light.position = zero + dx * A * sin(a * t + d) + dy * B * sin(b * t);
    }
};

light_params_t p_red{
    1, 1, 0.5, 0.5, pi<double>(),
    vec3(0, 0, 0),
    vec3(0.5, 0, 0),
    vec3(0, 0.5, 0),
}, p_green{
    1, 1, 0.5, 0.5, pi<double>() / 2.0,
    vec3(0, 0, 0),
    vec3(0.5, 0, 0),
    vec3(0, 0.5, 0),
}, p_blue{
    1, 1, 0.5, 1.0, pi<double>() / 2.0,
    vec3(0, 0, 0),
    vec3(0.5, 0, 0),
    vec3(0, 0.5, 0),
};

void sample_t::update_lights() {
    double t = clock() * 1.0 / CLOCKS_PER_SEC;

    p_red.update(lights[2], t);
    p_green.update(lights[3], t);
    p_blue.update(lights[4], t);
}
