#include "common.h"
#include "main.h"

gbuffer_t::gbuffer_t(int width, int height) {
    glGenFramebuffers(1, &fb_);
    glGenRenderbuffers(1, &depth_buf_);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);

    glBindRenderbuffer(GL_RENDERBUFFER, depth_buf_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf_);

    pos_tex_ = gen_texture(width, height, GL_RGB32F, GL_RGB, GL_FLOAT);
    norm_tex_ = gen_texture(width, height, GL_RGB32F, GL_RGB, GL_FLOAT);
    diffuse_tex_ = gen_texture(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    specular_tex_ = gen_texture(width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pos_tex_, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, norm_tex_, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, diffuse_tex_, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, specular_tex_, 0);

    static GLenum attachments[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, attachments);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

gbuffer_t::~gbuffer_t() {
    glDeleteTextures(1, &pos_tex_);
    glDeleteTextures(1, &norm_tex_);
    glDeleteTextures(1, &diffuse_tex_);
    glDeleteTextures(1, &specular_tex_);
    glDeleteRenderbuffers(1, &depth_buf_);
    glDeleteFramebuffers(1, &fb_);
}

GLuint gen_texture(int width, int height, GLint internalFormat, GLenum format, GLenum type) {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex_id;
}

void sample_t::draw_gbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_->fb_);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 model;
    model = scale(model, vec3(1, 1, 1) * 3);

    glUseProgram(gbuffer_shader_);

    glUniformMatrix4fv(glGetUniformLocation(gbuffer_shader_, "model"), 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(gbuffer_shader_, "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(gbuffer_shader_, "proj"), 1, GL_FALSE, value_ptr(proj));

    bunny_->draw();

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
