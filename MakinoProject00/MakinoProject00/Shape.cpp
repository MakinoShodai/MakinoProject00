#include "Shape.h"

// Vertex of shape
struct ShapeVertex {
    Vector3f pos;    // Position
    Vector2f uv;     // Texture coordinate
    Vector3f normal; // Normal
};

// Initialize mesh buffers of each shape
CMeshBufferFull CShapeMesh::ms_shape[static_cast<UINT>(ShapeKind::Max)] = {};

// Texture shape
CTexShape::CTexShape(CGameObject* owner, GraphicsLayer layer, ShapeKind kind, const std::wstring& texFilePath)
    : ACGraphicsComponent(GraphicsComponentType::TexShape, owner, layer)
    , m_kind(kind) {
    // Initialize mesh
    InitializeMesh(1, 1);
    const CMeshBufferFull& shapeMesh = CShapeMesh::GetMesh(kind);
    SetMeshInfo(0, shapeMesh.GetVertexWeakPtr(), shapeMesh.GetIndexWeakPtr(), shapeMesh.GetTopologyType());

    // Set standard texture
    SetTexture(0, 0, CTexture(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"), texFilePath));
}

// Color only shape
CColorOnlyShape::CColorOnlyShape(CGameObject* owner, GraphicsLayer layer, ShapeKind kind, const Colorf& color)
    : ACGraphicsComponent(GraphicsComponentType::ColorShape, owner, layer)
    , m_kind(kind) {
    // Initialize mesh
    InitializeMesh(1, 0);
    const CMeshBufferFull& shapeMesh = CShapeMesh::GetMesh(kind);
    SetMeshInfo(0, shapeMesh.GetVertexWeakPtr(), shapeMesh.GetIndexWeakPtr(), shapeMesh.GetTopologyType());

    // Set color
    SetColor(color);
}

// Initialize
void CShapeMesh::Initialize() {
    CreateSphere();
    CreateBox();
    CreateCapsule();
}

// Create mesh buffer of sphere
void CShapeMesh::CreateSphere() {
    // Radius
    const float radius = 0.5f;

    const USHORT slices = 24;
    const USHORT stacks = 24;

    std::vector<ShapeVertex> vertices;
    std::vector<USHORT> indices;

    // Create vertices and normal vectors
    for (USHORT stack = 0; stack <= stacks; ++stack) {
        float phi = (float)stack / stacks * Utl::PI;
        for (USHORT slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * Utl::PI * 2.0f;
            ShapeVertex vertex;

            vertex.pos.x() = radius * std::sinf(phi) * std::cosf(theta);
            vertex.pos.y() = radius * std::cosf(phi);
            vertex.pos.z() = radius * std::sinf(phi) * std::sinf(theta);

            vertex.normal.x() = vertex.pos.x();
            vertex.normal.y() = vertex.pos.y();
            vertex.normal.z() = vertex.pos.z();

            vertex.uv.x() = (float)slice / slices;
            vertex.uv.y() = (float)stack / stacks;

            vertices.push_back(vertex);
        }
    }

    // Create indices
    for (USHORT stack = 0; stack < stacks; ++stack) {
        for (USHORT slice = 0; slice < slices; ++slice) {
            USHORT first = (stack * (slices + 1)) + slice;
            USHORT second = first + slices + 1;

            // Specify first triangle's index
            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            // Specify next triangle's index
            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second + 1);
        }
    }

    // Create mesh buffer
    ms_shape[static_cast<UINT>(ShapeKind::Sphere)].GetVertexEntity()->Create<ShapeVertex>(&vertices[0], (UINT)vertices.size());
    ms_shape[static_cast<UINT>(ShapeKind::Sphere)].GetIndexEntity()->Create<USHORT>(&indices[0], (UINT)indices.size());
    ms_shape[static_cast<UINT>(ShapeKind::Sphere)].SetInfo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// Create mesh buffer of box
void CShapeMesh::CreateBox() {
    ShapeVertex vertices[] = {
        // -X face
        {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }},
        {{ -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }},
        {{ -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }},
        {{ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }},
        // +X face
        {{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, {  1.0f, 0.0f, 0.0f }},
        {{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, {  1.0f, 0.0f, 0.0f }},
        {{  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, {  1.0f, 0.0f, 0.0f }},
        {{  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, {  1.0f, 0.0f, 0.0f }},
        // -Y face
        {{  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }},
        {{ -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }},
        {{  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }},
        {{ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }},
        // +Y face
        {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f,  1.0f, 0.0f }},
        {{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f,  1.0f, 0.0f }},
        {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f,  1.0f, 0.0f }},
        {{  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f,  1.0f, 0.0f }},
        // -Z face
        {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }},
        {{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }},
        {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }},
        {{  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }},
        // +Z face
        {{  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }},
        {{ -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f }},
        {{  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }},
        {{ -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f }},
    };

    // Index buffer
    USHORT indices[] = {
         0, 1, 2,  1, 3, 2,    // -X face
         4, 5, 6,  5, 7, 6,    // +X face
         8, 9,10,  9,11,10,    // -Y face
        12,13,14, 13,15,14,    // +Y face
        16,17,18, 17,19,18,    // -Z face
        20,21,22, 21,23,22,    // +Z face
    };

    // Create mesh buffer
    ms_shape[static_cast<UINT>(ShapeKind::Box)].GetVertexEntity()->Create<ShapeVertex>(vertices, 24);
    ms_shape[static_cast<UINT>(ShapeKind::Box)].GetIndexEntity()->Create<USHORT>(indices, 36);
    ms_shape[static_cast<UINT>(ShapeKind::Box)].SetInfo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// Create mesh buffer of capsule
void CShapeMesh::CreateCapsule() {
    // Radius and height
    const float radius = 0.5f;
    const float height = 1.0f;

    const USHORT slices = 24;
    const USHORT stacks = 24;

    std::vector<ShapeVertex> vertices;
    std::vector<USHORT> indices;

    // Create vertices and normals for the upper hemisphere portion
    for (USHORT stack = 0; stack <= stacks / 2; ++stack) {
        float phi = (float)stack / stacks * Utl::PI;
        for (USHORT slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * Utl::PI * 2.0f;
            ShapeVertex vertex;

            vertex.pos[0] = radius * std::sinf(phi) * std::cosf(theta);
            vertex.pos[1] = radius * std::cosf(phi) + height / 2;
            vertex.pos[2] = radius * std::sinf(phi) * std::sinf(theta);

            vertex.normal[0] = vertex.pos[0];
            vertex.normal[1] = vertex.pos[1] - height / 2;
            vertex.normal[2] = vertex.pos[2];

            vertex.uv[0] = (float)slice / slices;
            vertex.uv[1] = (float)stack / stacks / 2;

            vertices.push_back(vertex);
        }
    }

    // Create vertices and normals for the lower hemisphere portion
    for (USHORT stack = stacks / 2 + 1; stack <= stacks; ++stack) {
        float phi = (float)stack / stacks * Utl::PI;
        for (USHORT slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * Utl::PI * 2.0f;
            ShapeVertex vertex;

            vertex.pos[0] = radius * std::sinf(phi) * std::cosf(theta);
            vertex.pos[1] = radius * std::cosf(phi) - height / 2;
            vertex.pos[2] = radius * std::sinf(phi) * std::sinf(theta);

            vertex.normal[0] = vertex.pos[0];
            vertex.normal[1] = vertex.pos[1] + height / 2;
            vertex.normal[2] = vertex.pos[2];

            vertex.uv[0] = (float)slice / slices;
            vertex.uv[1] = (float)stack / stacks / 2 + 0.5f;

            vertices.push_back(vertex);
        }
    }

    // Create indices
    for (USHORT stack = 0; stack < stacks; ++stack) {
        for (USHORT slice = 0; slice < slices; ++slice) {
            USHORT first = (stack * (slices + 1)) + slice;
            USHORT second = first + slices + 1;

            // Specify first triangle's index
            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            // Specify next triangle's index
            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second + 1);
        }
    }

    // Create mesh buffer
    ms_shape[static_cast<UINT>(ShapeKind::Capsule)].GetVertexEntity()->Create<ShapeVertex>(&vertices[0], (UINT)vertices.size());
    ms_shape[static_cast<UINT>(ShapeKind::Capsule)].GetIndexEntity()->Create<USHORT>(&indices[0], (UINT)indices.size());
    ms_shape[static_cast<UINT>(ShapeKind::Capsule)].SetInfo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
