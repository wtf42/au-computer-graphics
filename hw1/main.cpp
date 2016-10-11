#include "common.h"
#include "shader.h"
#include "tex.h"

#ifndef APIENTRY
#define APIENTRY
#endif

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

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

private:
    void init_texture();
    void init_vao();

    bool drag_started = false;
    vec2 drag_center;
    vec2 drag_click;

    float center_x = -0.5f, center_y = 0.0f, scale = 1.0f, magic = 1.0f;
    int iterations = 50;

    GLuint vs_, fs_, program_;
    GLuint vao_id_, vx_buf_id_, tex_id_;
    TwBar *bar_;
};

sample_t::sample_t()
{
    TwInit(TW_OPENGL, NULL);

    // Определение "контролов" GUI
    bar_ = TwNewBar("Parameters");
    TwDefine(" Parameters size='500 150' color='70 100 120' valueswidth=220 iconpos=topleft");

    TwAddVarRW(bar_, "center_x", TW_TYPE_FLOAT, &center_x, " min=-10 max=10 step=0.01");
    TwAddVarRW(bar_, "center_y", TW_TYPE_FLOAT, &center_y, " min=-10 max=10 step=0.01");
    TwAddVarRW(bar_, "scale", TW_TYPE_FLOAT, &scale, " min=0 max=100 step=0.01");
    TwAddVarRW(bar_, "iterations", TW_TYPE_INT32, &iterations, " min=1 max=1000 step=1");
    TwAddVarRW(bar_, "magic", TW_TYPE_FLOAT, &magic, " min=0 max=100 step=0.01");

    // Создание шейдеров
    vs_ = create_shader(GL_VERTEX_SHADER, "shaders//hw1.glslvs");
    fs_ = create_shader(GL_FRAGMENT_SHADER, "shaders//hw1.glslfs");
    // Создание программы путём линковки шейдерова
    program_ = create_program(vs_, fs_);

    init_texture();
    init_vao();

    {
        // выключаем отсечение невидимых поверхностей
        glDisable(GL_CULL_FACE);
        // выключаем тест глубины
        glDisable(GL_DEPTH_TEST);
        // очистка буфера кадра
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClearDepth(1);
    }
}

sample_t::~sample_t()
{
    // Удаление русурсов OpenGL
    glDeleteProgram(program_);
    glDeleteShader(vs_);
    glDeleteShader(fs_);
    glDeleteVertexArrays(1, &vao_id_);
    glDeleteBuffers(1, &vx_buf_id_);
    glDeleteTextures(1, &tex_id_);

    TwDeleteAllBars();
    TwTerminate();
}

void sample_t::init_vao()
{
    glGenVertexArrays(1, &vao_id_);
    glBindVertexArray(vao_id_);

    glGenBuffers(1, &vx_buf_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_id_);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint in_pos = glGetAttribLocation(program_, "in_pos");
    glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(in_pos);

    glBindVertexArray(0);
}

void sample_t::init_texture()
{
    tex_id_ = load_tex("content/tex.ppm");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, tex_id_);
    glUniform1i(glGetUniformLocation(program_, "tex"), 0);
}

void sample_t::scroll(vec2 pos, int dir)
{
    float new_scale = pow(1.5f, -dir);
    vec2 old_center(center_x, center_y);
    vec2 new_center = (old_center + pos * scale) - pos * (scale * new_scale);
    center_x = new_center.x;
    center_y = new_center.y;
    scale = scale * new_scale;

    TwRefreshBar(bar_);
}

void sample_t::drag_start(vec2 pos)
{
    drag_center = vec2(center_x, center_y);
    drag_click = pos;
    drag_started = true;
    drag_move(pos);
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
    vec2 old_center(center_x, center_y);
    vec2 new_center = drag_center + drag_click * scale - pos * scale;
    center_x = new_center.x;
    center_y = new_center.y;
}

void sample_t::draw_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_);

    glUniform1f(glGetUniformLocation(program_, "scale"), scale);
    glUniform2f(glGetUniformLocation(program_, "center"), center_x, center_y);
    glUniform1i(glGetUniformLocation(program_, "iter"), iterations);
    glUniform1f(glGetUniformLocation(program_, "magic"), magic);

    glBindVertexArray(vao_id_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

unique_ptr<sample_t> g_sample;

vec2 pt2vec(int x, int y) {
    float const w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float const h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    return (vec2(x / w, (h - y) / h) - vec2(0.5, 0.5)) * 2.0;
}

// отрисовка кадра
void display_func()
{
    // вызов функции отрисовки с передачей ей времени от первого вызова
    g_sample->draw_frame();

    // отрисовка GUI
    TwDraw();

    // смена front и back buffer'а (напоминаю, что у нас используется режим двойной буферизации)
    glutSwapBuffers();
}

// Переисовка кадра в отсутствии других сообщений
void idle_func()
{
    glutPostRedisplay();
}

void keyboard_func(unsigned char button, int x, int y)
{
    if (TwEventKeyboardGLUT(button, x, y))
        return;

    switch (button)
    {
    case 27:
        g_sample.reset();
        exit(0);
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

void mouse_wheel_func(int button, int dir, int x, int y)
{
    g_sample->scroll(pt2vec(x, y), dir);
}

// Отработка изменения размеров окна
void reshape_func(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    glViewport(0, 0, width, height);
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
    size_t const default_width = 600;
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
    glutCreateWindow("HW #1");

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

    glutMouseFunc(mouse_func);
    glutMotionFunc(mouse_motion_func);
    glutMouseWheelFunc(mouse_wheel_func);

    // подписываемся на события для AntTweakBar'а
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
    TwGLUTModifiersFunc(glutGetModifiers);

    // Создание класса-примера
    g_sample.reset(new sample_t());
    // Вход в главный цикл приложения
    glutMainLoop();

    return 0;
}
