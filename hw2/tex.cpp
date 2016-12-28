#include "main.h"

#include <FreeImage.h>

GLuint load_cubemap_tex(vector<const char*> files)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int idx = 0;
    for (const char* file : files) {
        FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(file, 0), file);
        FreeImage_FlipVertical(bitmap);
        FIBITMAP *pImage = FreeImage_ConvertTo24Bits(bitmap);
        void* buf = FreeImage_GetBits(pImage);
        int nWidth = FreeImage_GetWidth(pImage);
        int nHeight = FreeImage_GetHeight(pImage);

        GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx++;
        glTexImage2D(side, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, buf);

        FreeImage_Unload(pImage);
        FreeImage_Unload(bitmap);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void sample_t::init_texture()
{
    tex_id_ = load_cubemap_tex({
        "content/right.jpg",
        "content/left.jpg",
        "content/top.jpg",
        "content/bottom.jpg",
        "content/back.jpg",
        "content/front.jpg",
    });
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id_);
    glUniform1i(glGetUniformLocation(box_shader_, "box"), 0);
}
