#include "main.h"

float horizontalAngle = pi<float>();
//float verticalAngle = 0.0f;
float verticalAngle = -pi<float>() / 3;

vec3 get_direction() {
    return vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
}

vec3 get_right() {
    return vec3(
        sin(horizontalAngle - half_pi<float>()),
        0,
        cos(horizontalAngle - half_pi<float>())
    );
}

void sample_t::camera_move(vec2 dir) {
    horizontalAngle += speed1 * dir.x;
    verticalAngle += speed1 * dir.y;

    camera_update();
}

void sample_t::camera_key(int key) {
    switch (key) {
    case GLUT_KEY_UP:
        cam_position_ += get_direction() * speed2;
        break;
    case GLUT_KEY_DOWN:
        cam_position_ -= get_direction() * speed2;
        break;
    case GLUT_KEY_LEFT:
        cam_position_ -= get_right() * speed2;
        break;
    case GLUT_KEY_RIGHT:
        cam_position_ += get_right() * speed2;
        break;
    }

    camera_update();
}

void sample_t::camera_drag(vec2 delta) {
    vec3 right = get_right();
    vec3 up = cross(right, get_direction());

    cam_position_ += right * delta.x * speed3;
    cam_position_ += up * delta.y * speed3;

    camera_update();
}

void sample_t::camera_update() {
    vec3 direction = get_direction();
    vec3 up = cross(get_right(), direction);

    view = lookAt(cam_position_, cam_position_ + direction, up);
}
