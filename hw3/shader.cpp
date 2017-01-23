#include "shader.h"

GLuint create_shader(GLenum shader_type, char const * file_name) {
   ifstream f_in(file_name, std::ios::binary);

   if (!f_in)
      throw std::runtime_error(string("Shader source file ") + file_name + " access error");

   string content((std::istreambuf_iterator<char>(f_in)), std::istreambuf_iterator<char>());

   const GLchar* gl_text = content.c_str();
   GLuint shader = glCreateShader(shader_type);

   glShaderSource(shader, 1, &gl_text, nullptr);
   glCompileShader(shader);

   GLint result;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
   if (!result) {
      int info_log_length;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
      if (info_log_length > 0) {
         string buffer;
         buffer.resize(info_log_length);
         glGetShaderInfoLog(shader, info_log_length, nullptr, &buffer[0]);
         throw std::runtime_error(string("error in shader ") + file_name + ":\n" + buffer);
      }
   }

   return shader;
}

GLuint create_program(GLuint vs, GLuint fs) {
   GLuint program = glCreateProgram();
   glAttachShader(program, vs);
   glAttachShader(program, fs);
   glLinkProgram(program);

   GLint result;
   glGetProgramiv(program, GL_LINK_STATUS, &result);
   if(!result) {
      int info_log_length;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
      if (info_log_length > 0) {
         string buffer;
         buffer.resize(info_log_length);
         glGetProgramInfoLog(program, info_log_length, nullptr, &buffer[0]);
         throw std::runtime_error(buffer);
      }
   }
   return program;
}

GLuint load_shaders(const char* vs_file, const char* fs_file) {
    GLuint vs = create_shader(GL_VERTEX_SHADER, vs_file);
    GLuint fs = create_shader(GL_FRAGMENT_SHADER, fs_file);
    GLuint program = create_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
