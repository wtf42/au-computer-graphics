#include "main.h"

#include "common.h"
#include "shader.h"

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

sample_t::sample_t()
{
    TwInit(TW_OPENGL, NULL);

    bar_ = TwNewBar("Parameters");
    TwDefine("Parameters size='500 170' color='70 100 120' valueswidth=220 iconpos=topleft iconified=true");
    
    gbuffer_shader_ = load_shaders("shaders/gbuffer.glslvs", "shaders/gbuffer.glslfs");
    light_shader_ = load_shaders("shaders/light.glslvs", "shaders/light.glslfs");
    combine_shader_ = load_shaders("shaders/combine.glslvs", "shaders/combine.glslfs");
    sphere_shader_ = load_shaders("shaders/sphere.glslvs", "shaders/sphere.glslfs");

    window_width = (int)glutGet(GLUT_WINDOW_WIDTH);
    window_height = (int)glutGet(GLUT_WINDOW_HEIGHT);
    gbuffer_.reset(new gbuffer_t(window_width, window_height));
    lbuffer_.reset(new lbuffer_t(window_width, window_height));

    bunny_.reset(new bunny_t(gbuffer_shader_));
    quad2_.reset(new quad_t(gbuffer_shader_));
    quad_.reset(new quad_t(0));
    sphere_.reset(new sphere_t(0));

    init_lights();

    camera_update();

    TwAddVarRW(bar_, "show spheres", TW_TYPE_BOOLCPP, &show_sphere, "");
    TwAddVarRW(bar_, "show sphere centers", TW_TYPE_BOOLCPP, &show_center, "");
    TwAddVarRW(bar_, "light 0 (ambient)", TW_TYPE_BOOLCPP, &lights[0].enabled, "");
    TwAddVarRW(bar_, "light 1 (neutral)", TW_TYPE_BOOLCPP, &lights[1].enabled, "");
    TwAddVarRW(bar_, "light 2 (red)", TW_TYPE_BOOLCPP, &lights[2].enabled, "");
    TwAddVarRW(bar_, "light 3 (green)", TW_TYPE_BOOLCPP, &lights[3].enabled, "");
    TwAddVarRW(bar_, "light 4 (blue)", TW_TYPE_BOOLCPP, &lights[4].enabled, "");
    TwAddVarCB(bar_, "additional lights", TW_TYPE_UINT32, set_lights_callback, get_lights_callback, this, "");
}

sample_t::~sample_t()
{
    TwDeleteAllBars();
    TwTerminate();
}

void sample_t::scroll(vec2 pos, int dir)
{
    if (drag_started) {
        return;
    }
    fov_ -= dir * 0.1f;
    update_projection();
}

void sample_t::update_projection() {
    proj = perspective(fov_, float(window_width) / window_height, 0.01f, 1000.0f);
}

void sample_t::drag_start(vec2 pos)
{
    drag_start_click = pos;
    drag_started = true;
}

void sample_t::drag_end()
{
    drag_started = false;
}

void sample_t::drag_move(vec2 pos)
{
    if (!drag_started) {
        return;
    }
    vec2 delta = (pos - drag_start_click);
    camera_drag(delta);
    //camera_move(vec2(delta.x, -delta.y) / 10);
}

void sample_t::resize(int width, int height)
{
    window_width = width;
    window_height = height;
    gbuffer_.reset(new gbuffer_t(width, height));
    lbuffer_.reset(new lbuffer_t(width, height));
    update_projection();
}

void sample_t::toggle_spheres()
{
    show_sphere = !show_sphere;
}

void sample_t::toggle_centers()
{
    show_center = !show_center;
}

void sample_t::change_mode()
{
    ++mode;
    mode %= 3;
}

void sample_t::draw_combined()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(combine_shader_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->pos_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_pos"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->norm_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_norm"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->diffuse_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "gbuffer_color"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lbuffer_->light_tex_);
    glUniform1i(glGetUniformLocation(combine_shader_, "lbuffer"), 3);

    glUniform1i(glGetUniformLocation(combine_shader_, "mode"), mode);

    quad_->draw();
}

void sample_t::draw_frame()
{
    update_lights();

    draw_gbuffer();
    draw_lbuffer();
    draw_combined();
    if (show_sphere) {
        draw_spheres();
    }
    if (show_center) {
        draw_sphere_centers();
    }
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

unique_ptr<sample_t> g_sample;
bool view_mode = true;

void toggle_mode() {
    view_mode = !view_mode;
    if (view_mode) {
        glutSetCursor(GLUT_CURSOR_NONE);
        TwDefine("Parameters iconified=true");
    } else {
        glutSetCursor(0);
        TwDefine("Parameters iconified=false");
    }
}

vec2 pt2vec(int x, int y) {
    float w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    return (vec2(x / w, (h - y) / h) - vec2(0.5, 0.5)) * 2.0;
}

// отрисовка кадра
void display_func()
{
    g_sample->draw_frame();
    TwDraw();
    glutSwapBuffers();
}

// Переисовка кадра в отсутствии других сообщений
void idle_func()
{
    glutPostRedisplay();
}

void keyboard_func(unsigned char key, int x, int y)
{
    if (TwEventKeyboardGLUT(key, x, y)) {
        return;
    }

    switch (key) {
    case 27:
        g_sample.reset();
        exit(0);
    case 'w':
        g_sample->camera_key(GLUT_KEY_UP);
        break;
    case 's':
        g_sample->camera_key(GLUT_KEY_DOWN);
        break;
    case 'a':
        g_sample->camera_key(GLUT_KEY_LEFT);
        break;
    case 'd':
        g_sample->camera_key(GLUT_KEY_RIGHT);
        break;
    case 'z':
        g_sample->toggle_spheres();
        break;
    case 'x':
        g_sample->toggle_centers();
        break;
    case 'm':
        g_sample->change_mode();
        break;
    case 'q':
        toggle_mode();
        break;
    }
    if (key >= '0' && key <= '9') {
        g_sample->toggle_light(key - '0');
    }
}

void special_func(int key, int x, int y)
{
    if (TwEventSpecialGLUT(key, x, y)) {
        return;
    }

    switch (key) {
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
        g_sample->camera_key(key);
        break;
    }
}

void mouse_func(int button, int state, int x, int y)
{
    if (TwEventMouseButtonGLUT(button, state, x, y)) {
        return;
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            g_sample->drag_start(pt2vec(x, y));
        } else {
            g_sample->drag_end();
        }
    }
}

void mouse_motion_func(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y)) {
        return;
    }
    g_sample->drag_move(pt2vec(x, y));
}

void passive_motion_func(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y)) {
        return;
    }

    if (view_mode) {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        int cx = w / 2, cy = h / 2;
        if (x != cx && y != cy) {
            vec2 p = pt2vec(x, y);
            vec2 c = pt2vec(cx, cy);
            g_sample->camera_move(vec2(c.x - p.x, p.y - c.y));
            glutWarpPointer(cx, cy);
        }
    }
}

void mouse_wheel_func(int button, int dir, int x, int y)
{
    g_sample->scroll(pt2vec(x, y), dir);
}

// Отработка изменения размеров окна
void reshape_func(int width, int height)
{
    glViewport(0, 0, width, height);
    g_sample->resize(width, height);
    TwWindowSize(width, height);
}

// Очищаем все ресурсы, пока контекст ещё не удалён
void close_func()
{
    g_sample.reset();
}

int main(int argc, char ** argv)
{
    // Размеры окна по-умолчанию
    size_t const default_width = 800;
    size_t const default_height = 600;

    glutInit(&argc, argv);
    glutInitWindowSize(default_width, default_height);
    // Указание формата буфера экрана:
    // - GLUT_DOUBLE - двойная буферизация
    // - GLUT_RGB - 3-ёх компонентный цвет
    // - GLUT_DEPTH - будет использоваться буфер глубины
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    // Создаем контекст версии 3.0
    glutInitContextVersion(3, 3);
    // Контекст будет поддерживать отладку и "устаревшую" функциональность, которой, например, может пользоваться библиотека AntTweakBar
    //glutInitContextFlags   (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    // Указание либо на core либо на compatibility профил
    glutInitContextProfile (GLUT_COMPATIBILITY_PROFILE );
    glutCreateWindow("HW #3 [press 'q' to change mode]");

    // Инициализация указателей на функции OpenGL
    if (glewInit() != GLEW_OK)
    {
        cerr << "GLEW init failed" << endl;
        return 1;
    }

    // Проверка созданности контекста той версии, какой мы запрашивали
    if (!GLEW_VERSION_3_3)
    {
        cerr << "OpenGL 3.3 not supported" << endl;
        return 1;
    }

    // подписываемся на оконные события
    glutReshapeFunc(reshape_func);
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    glutCloseFunc(close_func);
    glutKeyboardFunc(keyboard_func);
    glutSpecialFunc(special_func);

    glutMouseFunc(mouse_func);
    glutMotionFunc(mouse_motion_func);
    glutPassiveMotionFunc(passive_motion_func);
    glutMouseWheelFunc(mouse_wheel_func);

    glutSetCursor(GLUT_CURSOR_NONE);

    TwGLUTModifiersFunc(glutGetModifiers);

    try {
        g_sample.reset(new sample_t());
        glutMainLoop();
    } catch (std::exception const & except) {
        std::cout << except.what() << endl;
        return 1;
    }

    return 0;
}
