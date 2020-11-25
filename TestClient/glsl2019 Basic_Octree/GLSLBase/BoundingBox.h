#pragma once

#include "ShaderProgram.h"
#include "GameObject.h"

class BoundingBox {
public:

    BoundingBox();
    ~BoundingBox();

    void Init();
    void CreateVBO();
    void Draw(GameObject& obj);

private:

    ShaderProgram mBoundingBoxShader;

    GLuint      mVBO;
    GLuint      mIBO;
};
