#pragma once

#include "common.h"

GLuint create_shader(GLenum shader_type, const char* file_name);
GLuint create_program(GLuint vs, GLuint fs);
GLuint load_shaders(const char* vs_file, const char* fs_file);
