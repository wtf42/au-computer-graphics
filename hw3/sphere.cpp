#include "main.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const char sphere_path[] = "content/sphere2.obj";

sphere_t::sphere_t(GLuint shader) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(sphere_path, aiProcess_Triangulate);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error(importer.GetErrorString());
    }
    aiMesh* mesh = scene->mMeshes[0];

    vector<GLfloat> data(mesh->mNumVertices * 3);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        data[i * 3 + 0] = mesh->mVertices[i].x;
        data[i * 3 + 1] = mesh->mVertices[i].y;
        data[i * 3 + 2] = mesh->mVertices[i].z;
    }

    vector<GLuint> faces(mesh->mNumFaces * 3);
    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        faces[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
        faces[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
        faces[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
    }
    faces_ = faces.size();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint), &faces[0], GL_STATIC_DRAW);

    //GLuint in_pos = glGetAttribLocation(shader, "in_pos");
    GLuint in_pos = 0;
    glEnableVertexAttribArray(in_pos);
    glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
}

sphere_t::~sphere_t() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void sphere_t::draw() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, faces_, GL_UNSIGNED_INT, 0);
}
