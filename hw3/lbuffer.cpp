#include "common.h"
#include "main.h"

lbuffer_t::lbuffer_t(int width, int height) {
    glGenFramebuffers(1, &fb_);
    glGenRenderbuffers(1, &depth_buf_);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);

    glBindRenderbuffer(GL_RENDERBUFFER, depth_buf_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf_);

    light_tex_ = gen_texture(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, light_tex_, 0);

    static GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

lbuffer_t::~lbuffer_t() {
    glDeleteTextures(1, &light_tex_);
    glDeleteRenderbuffers(1, &depth_buf_);
    glDeleteFramebuffers(1, &fb_);
}


void sample_t::draw_lbuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_->fb_);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(light_shader_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->pos_tex_);
    glUniform1i(glGetUniformLocation(light_shader_, "gbuffer_pos"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->norm_tex_);
    glUniform1i(glGetUniformLocation(light_shader_, "gbuffer_norm"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->diffuse_tex_);
    glUniform1i(glGetUniformLocation(light_shader_, "gbuffer_diffuse"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gbuffer_->specular_tex_);
    glUniform1i(glGetUniformLocation(light_shader_, "gbuffer_specular"), 3);

    vec2 window_size(window_width, window_height);
    glUniform2fv(glGetUniformLocation(light_shader_, "window"), 1, value_ptr(window_size));
    glUniform3fv(glGetUniformLocation(light_shader_, "camera"), 1, value_ptr(cam_position_));
    glUniform1f(glGetUniformLocation(light_shader_, "gamma"), gamma);

    for (const light_t& light : lights) {
        if (!light.enabled) continue;

        mat4 model;
        model = translate(model, light.position);
        model = scale(model, vec3(1, 1, 1) * light.size);
        mat4 mvp = proj * view * model;

        glUniformMatrix4fv(glGetUniformLocation(light_shader_, "mvp"), 1, GL_FALSE, value_ptr(mvp));

        glUniform3fv(glGetUniformLocation(light_shader_, "light.position"), 1, value_ptr(light.position));
        glUniform1f(glGetUniformLocation(light_shader_, "light.size"), light.size);
        glUniform3fv(glGetUniformLocation(light_shader_, "light.ambient"), 1, value_ptr(light.ambient));
        glUniform3fv(glGetUniformLocation(light_shader_, "light.diffuse"), 1, value_ptr(light.diffuse));
        glUniform3fv(glGetUniformLocation(light_shader_, "light.specular"), 1, value_ptr(light.specular));

        sphere_->draw();
    }


    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void sample_t::draw_spheres() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glUseProgram(sphere_shader_);

    for (const light_t& light : lights) {
        if (!light.enabled) continue;

        mat4 model;
        model = translate(model, light.position);
        model = scale(model, vec3(1, 1, 1) * light.size);
        mat4 mvp = proj * view * model;

        vec3 color = light.diffuse * 0.5;

        glUniformMatrix4fv(glGetUniformLocation(sphere_shader_, "mvp"), 1, GL_FALSE, value_ptr(mvp));
        glUniform3fv(glGetUniformLocation(sphere_shader_, "color"), 1, value_ptr(color));

        sphere_->draw();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void sample_t::draw_sphere_centers() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glUseProgram(sphere_shader_);

    for (const light_t& light : lights) {
        if (!light.enabled) continue;

        mat4 model;
        model = translate(model, light.position);
        model = scale(model, vec3(1, 1, 1) * 0.01);
        mat4 mvp = proj * view * model;

        vec3 color = light.diffuse;

        glUniformMatrix4fv(glGetUniformLocation(sphere_shader_, "mvp"), 1, GL_FALSE, value_ptr(mvp));
        glUniform3fv(glGetUniformLocation(sphere_shader_, "color"), 1, value_ptr(color));

        sphere_->draw();
    }
}
