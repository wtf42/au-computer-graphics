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
            true,
            {}
        },
        { // neutral
            vec3(0, 1, 0),
            1.1f,
            vec3(0, 0, 0),
            vec3(0.3f, 0.3f, 0.3f),
            vec3(0.1f, 0.1f, 0.1f),
            false,
            {}
        },
        { // red
            vec3(0, 0, 0),
            0.3f,
            vec3(0, 0, 0),
            vec3(0.8f, 0, 0),
            vec3(1.0f, 0, 0),
            false,
            {
                1, 1, 0.5, 0.5, pi<double>(),
                vec3(0, 0, 0),
                vec3(0.5, 0, 0),
                vec3(0, 0.5, 0),
            }
        },
        { // green
            vec3(0, 0, 0),
            0.5f,
            vec3(0, 0, 0),
            vec3(0, 0.8f, 0),
            vec3(0, 1.0f, 0),
            false,
            {
                1, 1, 0.5, 0.5, pi<double>() / 2.0,
                vec3(0, 0, 0),
                vec3(0.5, 0, 0),
                vec3(0, 0.5, 0),
            }
        },
        { // blue
            vec3(0, 0, 0),
            0.3f,
            vec3(0, 0, 0),
            vec3(0, 0, 0.8f),
            vec3(0, 0, 1.0f),
            false,
            {
                1, 1, 0.5, 1.0, pi<double>() / 2.0,
                vec3(0, 0, 0),
                vec3(0.5, 0, 0),
                vec3(0, 0.5, 0),
            }
        }
    };
    set_additional_lights(10);
}

void light_t::update_position(double t) {
    const position_params_t& p = position_params;
    position = p.zero + p.dx * p.A * sin(p.a * t + p.d) + p.dy * p.B * sin(p.b * t);
}

void sample_t::toggle_light(int idx) {
    if (idx >= 0 && idx < (int)lights.size()) {
        lights[idx].enabled = !lights[idx].enabled;
    }
}

void sample_t::update_lights() {
    double t = clock() * 1.0 / CLOCKS_PER_SEC;

    lights[2].update_position(t);
    lights[3].update_position(t);
    lights[4].update_position(t);

    for (size_t i = 5; i < lights.size(); ++i) {
        lights[i].update_position(t);
    }
}

size_t sample_t::get_additional_lights() {
    return lights.size() - 5;
}

float rnd() {
    return rand() * 1.0f / RAND_MAX;
}
void sample_t::set_additional_lights(size_t count) {
    lights.resize(5 + count);
    for (size_t i = 5; i < lights.size(); ++i) {
        vec3 color = vec3(rnd(), rnd(), rnd());
        color /= max(color.r, color.g, color.b);
        lights[i] = {
            vec3(0, 0, 0),
            0.6f,
            vec3(),
            color * 1.0,
            color * 1.0,
            true,
            {
                1, 1, lights_speed * rnd(), lights_speed * rnd(), pi<double>() / 2.0 * rnd(),
                vec3(rnd() / 2, lights_height, rnd() / 2),
                vec3(0.5, 0, 0),
                vec3(0, 0, 0.5),
            }
        };
    }
}

void TW_CALL sample_t::get_lights_callback(void *value, void *clientData) {
    *static_cast<unsigned*>(value) = static_cast<sample_t*>(clientData)->get_additional_lights();
}

void TW_CALL sample_t::set_lights_callback(const void *value, void *clientData) {
    static_cast<sample_t*>(clientData)->set_additional_lights(*static_cast<const unsigned*>(value));
}
