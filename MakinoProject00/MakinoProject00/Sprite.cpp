#include "Sprite.h"

// Initialize static member
CUniquePtrWeakable<CVertexBuffer> ACSprite::ms_vertexBuffer = nullptr;
CUniquePtrWeakable<CIndexBuffer> ACSprite::ms_indexBuffer = nullptr;

// Constructor
ACSprite::ACSprite(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer, const std::wstring& texFilePath)
    : ACGraphicsComponent(type, owner, layer)
    , m_resolutionRatio(1.0f)
    , m_width(0) 
    , m_height(0) { 

    // The number of mesh is 1 and the number of texture per mesh is 1
    InitializeMesh(1, 1);

    // Add texture data
    SetTexture(0, 0, CTexture(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"), texFilePath));

    // Set mesh information
    SetMeshInfo(0, ms_vertexBuffer.GetWeakPtr(), ms_indexBuffer.GetWeakPtr(), D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Calculate resolution ratio
    auto handle = GetTexture(0, 0)->GetHandle();
    if (handle->IsInstance()) {
        m_width = handle->GetWidth();
        m_height = handle->GetHeight();
        m_resolutionRatio = (float)m_width / (float)m_height;
    }
}

// Initialize shader class
void ACSprite::Initialize() {
    // Create vertices
    struct Vertex {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };
    Vertex vertices[] = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}}, // Left bottom
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}}, // Left top
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}}, // Right bottom
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}}, // Right bottom
    };
    ms_vertexBuffer = CUniquePtrWeakable<CVertexBuffer>::Make();
    ms_vertexBuffer->Create<Vertex>(vertices, 4);

    // Create indices
    USHORT indices[] = {
        0, 1, 2,
        2, 1, 3
    };
    ms_indexBuffer = CUniquePtrWeakable<CIndexBuffer>::Make();
    ms_indexBuffer->Create<USHORT>(indices, 6);
}
