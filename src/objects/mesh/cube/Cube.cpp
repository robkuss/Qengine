#pragma once

#include <objects/mesh/Mesh.h>

class Cube final : public Mesh {
public:
    /** Construct a Cube Mesh with given side length and position and default scale and rotation */
    explicit Cube(const string& name, const Vector3& position, const float s,
        const Color& color, const shared_ptr<Texture>& texture)
            : Mesh{name, color, texture}, s(s) {
        initializeVertices();
        initializeFaceIndices();

        initializeTriangles();
        buildVertexToEdgeMap();
        buildEdgeToFaceMap();
        updateNormals();

        Mesh::applyTransformation(OBJECT, GRAB, Matrix4::translate(position));
    }

    ~Cube() override = default;

private:
    float s;    // side length

    /** Initialize the Cube's vertices based on side length and position */
    void initializeVertices() override {
        // Front face
        vertices.emplace_back(make_shared<Vertex>(-s/2, -s/2,  s/2, Vector2(0, 0)));  // Bottom-left
        vertices.emplace_back(make_shared<Vertex>( s/2, -s/2,  s/2, Vector2(1, 0)));  // Bottom-right
        vertices.emplace_back(make_shared<Vertex>( s/2,  s/2,  s/2, Vector2(1, 1)));  // Top-right
        vertices.emplace_back(make_shared<Vertex>(-s/2,  s/2,  s/2, Vector2(0, 1)));  // Top-left

        // Back face
        vertices.emplace_back(make_shared<Vertex>(-s/2, -s/2, -s/2, Vector2(0, 0)));  // Bottom-left
        vertices.emplace_back(make_shared<Vertex>( s/2, -s/2, -s/2, Vector2(1, 0)));  // Bottom-right
        vertices.emplace_back(make_shared<Vertex>( s/2,  s/2, -s/2, Vector2(1, 1)));  // Top-right
        vertices.emplace_back(make_shared<Vertex>(-s/2,  s/2, -s/2, Vector2(0, 1)));  // Top-left
    }

    void initializeFaceIndices() override {
        faceIndices = {
            // Front face
            0, 1, 2,  2, 3, 0,
            // Right face
            1, 5, 6,  6, 2, 1,
            // Back face
            5, 4, 7,  7, 6, 5,
            // Left face
            4, 0, 3,  3, 7, 4,
            // Top face
            3, 2, 6,  6, 7, 3,
            // Bottom face
            4, 5, 1,  1, 0, 4
        };
    }
};
