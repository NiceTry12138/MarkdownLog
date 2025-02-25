#pragma once
#include "Test.h"

namespace TestModule {
    class TestRenderTexture : public Test
    {
    public:
        virtual ~TestRenderTexture();

        virtual void Init() override;
        virtual void Exit() override;

        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnImGuiRender() override;

    private:
        glm::vec3 m_TransitaionA = glm::vec3(200, 100, 0);
        glm::vec3 m_TransitaionB = glm::vec3(400, 300, 0);
        
        VertexArray* m_va{ nullptr };
        IndexBuffer* m_ibo{ nullptr };
        VertexBuffer* m_vb{ nullptr };
        Shader* m_shader{ nullptr };
        Texture* m_texture{ nullptr };

        Renderer m_render;
    };
};

