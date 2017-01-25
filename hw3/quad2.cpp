#include "common.h"
#include "main.h"

const GLfloat quad2_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

quad2_t::quad2_t(GLuint shader) {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad2_buffer_data), quad2_buffer_data, GL_STATIC_DRAW);

    GLuint in_pos = glGetAttribLocation(shader, "in_pos");
    glEnableVertexAttribArray(in_pos);
    glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    GLuint in_norm = glGetAttribLocation(shader, "in_norm");
    glEnableVertexAttribArray(in_norm);
    glVertexAttribPointer(in_norm, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

quad2_t::~quad2_t() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void quad2_t::draw() {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
