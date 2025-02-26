#pragma once
#include "Test.h"

namespace TestModule {

    struct Vertex {
        float Position[3];
        float Color[4];
        float TexCoord[2];
        float TextureIndex;

        void init(float InPos[3], float InTexCoord[2], float InColor[3], float InIndex) 
        {
            std::memcpy(Position, InPos, sizeof(Position));
            std::memcpy(TexCoord, InTexCoord, sizeof(TexCoord));
            std::memcpy(Color, InColor, sizeof(Color));
            TextureIndex = InIndex;
        }
    };

    class TestDynamicDraw : public Test
    {
    public:
        virtual void Init() override;
        virtual void Exit() override;

        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnImGuiRender() override;

    private:
        GLuint m_VAO;   // vertex array
        GLuint m_VBO;   // vertex buffer 
        GLuint m_IB;    // index buffer

        Texture m_T1;
        Texture m_T2;

        Shader m_Shader;

        Vertex m_Vertexs[8];
        Vertex m_OriginVertexs[8];

        glm::vec2 m_BlueTransition = glm::vec2(0, 0);
        glm::vec2 m_RedTransition = glm::vec2(0, 0);
    };
};


