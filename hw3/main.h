#pragma once

#include "common.h"

struct gbuffer_t;
struct lbuffer_t;
struct obj_t;
struct light_t;

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
    void resize(int width, int height);
    void change_mode();

    void camera_key(int key);
    void camera_move(vec2 delta);
    void camera_drag(vec2 delta);
    void camera_update();
    void toggle_spheres();
    void toggle_centers();
    void toggle_light(int idx);

private:
    void update_projection();

    bool drag_started = false;
    vec2 drag_start_click;

    float speed1 = 0.4f;
    float speed2 = 0.05f;
    float speed3 = 0.005f;
    bool show_sphere = false;
    bool show_center = true;
    int mode = 0;

    int window_width, window_height;
    //vec3 cam_position_ = vec3(0, 0, 1.0);
    vec3 cam_position_ = vec3(0, 1.0, 1.0);
    float fov_ = 45.0f;
    mat4 view;
    mat4 proj;
    float gamma = 2.2f;

    vector<light_t> lights;
    void init_lights();
    void update_lights();
    size_t get_additional_lights();
    void set_additional_lights(size_t count);
    static void TW_CALL get_lights_callback(void *value, void *clientData);
    static void TW_CALL set_lights_callback(const void *value, void *clientData);

    GLuint gbuffer_shader_;
    GLuint light_shader_;
    GLuint combine_shader_;
    GLuint sphere_shader_;

    unique_ptr<gbuffer_t> gbuffer_;
    unique_ptr<lbuffer_t> lbuffer_;
    unique_ptr<obj_t> quad_;
    unique_ptr<obj_t> quad2_;
    unique_ptr<obj_t> sphere_;
    unique_ptr<obj_t> bunny_;

    void draw_gbuffer();
    void draw_lbuffer();
    void draw_combined();
    void draw_spheres();
    void draw_sphere_centers();

    TwBar *bar_;
};

GLuint gen_texture(int width, int height, GLint internalFormat, GLenum format, GLenum type);

struct gbuffer_t {
    gbuffer_t(int width, int height);
    ~gbuffer_t();

    GLuint fb_;
    GLuint pos_tex_;
    GLuint norm_tex_;
    GLuint diffuse_tex_;
    GLuint specular_tex_;
private:
    GLuint depth_buf_;
};

struct lbuffer_t {
    lbuffer_t(int width, int height);
    ~lbuffer_t();

    GLuint fb_;
    GLuint light_tex_;
private:
    GLuint depth_buf_;
};


struct obj_t {
    obj_t(){}
    virtual ~obj_t(){}
    virtual void draw(){}
};

struct bunny_t : obj_t {
    bunny_t(GLuint shader);
    ~bunny_t();
    void draw();
private:
    GLuint vao_, vbo_, ebo_;
    unsigned faces_;
};

struct quad_t : obj_t {
    quad_t(GLuint shader);
    ~quad_t();
    void draw();
private:
    GLuint vao_, vbo_;
};

struct quad2_t : obj_t {
    quad2_t(GLuint shader);
    ~quad2_t();
    void draw();
private:
    GLuint vao_, vbo_;
};

struct sphere_t : obj_t {
    sphere_t(GLuint shader);
    ~sphere_t();
    void draw();
private:
    GLuint vao_, vbo_, ebo_;
    unsigned faces_;
};

struct light_t {
    vec3 position;
    GLfloat size;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;

    struct position_params_t {
        double A, B, a, b, d;
        vec3 zero, dx, dy;
    } position_params;
    void update_position(double t);
};
