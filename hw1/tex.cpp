#include "tex.h"
#include "common.h"

#include <string>
#include <tuple>
#include <fstream>
#include <iostream>

using std::string;
using std::tuple;

tuple<string, int, int> read_image(const string& file)
{
    string magic, image;
    int w, h, b;
    std::fstream in(file);
    in >> magic >> w >> h >> b; in.get();
    image.resize(w * h * 3);
    in.read((char*)image.data(), w * h * 3);
    return std::make_tuple(image, w, h);
}

GLuint load_tex(const string& file)
{
    string image;
    int w, h;
    std::tie(image, w, h) = read_image(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_1D, textureID);
    glTexImage1D(GL_TEXTURE_1D, 0, 3, w, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_1D, 0);
    return textureID;
}
