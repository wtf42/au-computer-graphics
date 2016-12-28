#include "main.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

constexpr char obj_path[] = "content/bunny_with_normals.obj";

void sample_t::init_obj() {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(obj_path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error(importer.GetErrorString());
    }
    aiMesh* mesh = scene->mMeshes[0];

    vector<GLfloat> data(mesh->mNumVertices * 6);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        data[i * 6 + 0] = mesh->mVertices[i].x;
        data[i * 6 + 1] = mesh->mVertices[i].y;
        data[i * 6 + 2] = mesh->mVertices[i].z;
        data[i * 6 + 3] = mesh->mNormals[i].x;
        data[i * 6 + 4] = mesh->mNormals[i].y;
        data[i * 6 + 5] = mesh->mNormals[i].z;
    }

    vector<GLuint> faces(mesh->mNumFaces * 3);
    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        faces[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
        faces[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
        faces[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
    }
    mesh_faces_ = faces.size();

    glGenVertexArrays(1, &obj_vao_id_);
    glBindVertexArray(obj_vao_id_);

    glGenBuffers(1, &obj_vbo_id_);
    glBindBuffer(GL_ARRAY_BUFFER, obj_vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);

    glGenBuffers(1, &obj_ebo_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj_ebo_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint), &faces[0], GL_STATIC_DRAW);

    GLuint in_pos = glGetAttribLocation(obj_shader_, "in_pos");
    glEnableVertexAttribArray(in_pos);
    glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    GLuint in_norm = glGetAttribLocation(obj_shader_, "in_norm");
    glEnableVertexAttribArray(in_norm);
    glVertexAttribPointer(in_norm, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
