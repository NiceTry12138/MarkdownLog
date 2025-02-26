#pragma once
#include "Test.h"

namespace TestModule {
    class TestTwoBlock : public Test
    {
    public:
        virtual ~TestTwoBlock();

        virtual void Init() override;
        virtual void Exit() override;

        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender() override;
        virtual void OnImGuiRender() override;

    private:
        glm::vec3 m_CameraTransition = glm::vec3(200, 100, 0);

        VertexArray* m_va{ nullptr };
        IndexBuffer* m_ibo{ nullptr };
        VertexBuffer* m_vb{ nullptr };
        Shader* m_shader{ nullptr };
        Texture* m_texture{ nullptr };

        Renderer m_render;
    };
};


