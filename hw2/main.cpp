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
    TwDefine("Parameters size='500 150' color='70 100 120' valueswidth=220 iconpos=topleft iconified=true");

    TwAddVarRW(bar_, "speed1", TW_TYPE_FLOAT, &speed1, " min=0 max=100 step=0.001");
    TwAddVarRW(bar_, "speed2", TW_TYPE_FLOAT, &speed2, " min=0 max=100 step=0.001");
    TwAddVarRW(bar_, "speed3", TW_TYPE_FLOAT, &speed3, " min=0 max=100 step=0.001");

    box_vs_ = create_shader(GL_VERTEX_SHADER, "shaders/box.glslvs");
    box_fs_ = create_shader(GL_FRAGMENT_SHADER, "shaders/box.glslfs");
    box_shader_ = create_program(box_vs_, box_fs_);

    obj_vs_ = create_shader(GL_VERTEX_SHADER, "shaders/obj.glslvs");
    obj_fs_ = create_shader(GL_FRAGMENT_SHADER, "shaders/obj.glslfs");
    obj_shader_ = create_program(obj_vs_, obj_fs_);

    init_texture();
    init_box();
    init_obj();

    camera_update();

    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClearDepth(1);
    }
}

sample_t::~sample_t()
{
    glDeleteVertexArrays(1, &box_vao_id_);
    glDeleteBuffers(1, &box_buf_id_);
    glDeleteTextures(1, &tex_id_);

    glDeleteProgram(box_shader_);
    glDeleteShader(box_vs_);
    glDeleteShader(box_fs_);

    glDeleteVertexArrays(1, &obj_vao_id_);
    glDeleteBuffers(1, &obj_vbo_id_);
    glDeleteBuffers(1, &obj_ebo_id_);

    glDeleteProgram(obj_shader_);
    glDeleteShader(obj_vs_);
    glDeleteShader(obj_fs_);

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
    float w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    proj = perspective(fov_, w / h, 0.01f, 100.0f);
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
}

void sample_t::resize(vec2 size)
{
    update_projection();
}

void sample_t::draw_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LESS);

    glUseProgram(obj_shader_);
    glUniformMatrix4fv(glGetUniformLocation(obj_shader_, "model"), 1, GL_FALSE, value_ptr(mat4()));
    glUniformMatrix4fv(glGetUniformLocation(obj_shader_, "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(obj_shader_, "proj"), 1, GL_FALSE, value_ptr(proj));
    glUniform3fv(glGetUniformLocation(obj_shader_, "camera"), 1, value_ptr(cam_position_));

    glBindVertexArray(obj_vao_id_);
    glDrawElements(GL_TRIANGLES, mesh_faces_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LEQUAL);

    glUseProgram(box_shader_);
    glUniformMatrix4fv(glGetUniformLocation(box_shader_, "view"), 1, GL_FALSE, value_ptr(mat4(mat3(view))));
    glUniformMatrix4fv(glGetUniformLocation(box_shader_, "proj"), 1, GL_FALSE, value_ptr(proj));

    glBindVertexArray(box_vao_id_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

unique_ptr<sample_t> g_sample;

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

void mouse_wheel_func(int button, int dir, int x, int y)
{
    g_sample->scroll(pt2vec(x, y), dir);
}

// Отработка изменения размеров окна
void reshape_func(int width, int height)
{
    glViewport(0, 0, width, height);
    g_sample->resize(vec2(width, height));
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
    glutInitContextVersion(3, 0);
    // Контекст будет поддерживать отладку и "устаревшую" функциональность, которой, например, может пользоваться библиотека AntTweakBar
    //glutInitContextFlags   (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    // Указание либо на core либо на compatibility профил
    //glutInitContextProfile (GLUT_COMPATIBILITY_PROFILE );
    glutCreateWindow("HW #2");

    // Инициализация указателей на функции OpenGL
    if (glewInit() != GLEW_OK)
    {
        cerr << "GLEW init failed" << endl;
        return 1;
    }

    // Проверка созданности контекста той версии, какой мы запрашивали
    if (!GLEW_VERSION_3_0)
    {
        cerr << "OpenGL 3.0 not supported" << endl;
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
