#pragma once

#include "common.h"

class sample_t
{
public:
    sample_t();
    ~sample_t();

    void draw_frame();
    void scroll(vec2 pos, int dir);
    void drag_start(vec2 pos);
    void drag_move(vec2 pos);
    void drag_end();
    void resize(vec2 size);

    void camera_key(int key);
    void camera_move(vec2 delta);
    void camera_drag(vec2 delta);
    void camera_update();

private:
    void init_texture();
    void init_box();
    void init_obj();

    void update_projection();

    bool drag_started = false;
    vec2 drag_start_click;

    float speed1 = 0.4f;
    float speed2 = 0.05f;
    float speed3 = 0.005f;

    float fov_ = 45.0f;
    vec3 cam_position_ = vec3(0, 0.1, 0.5);
    mat4 view;
    mat4 proj;

    GLuint box_vs_, box_fs_, box_shader_;
    GLuint obj_vs_, obj_fs_, obj_shader_;
    GLuint tex_id_;
    GLuint box_vao_id_, box_buf_id_;
    GLuint obj_vao_id_, obj_vbo_id_, obj_ebo_id_;
    unsigned mesh_faces_;

    TwBar *bar_;
};
