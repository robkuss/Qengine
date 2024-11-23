#pragma once

#include "objects/mesh/Mesh.h"

class Cube final : public Mesh {
public:
    /** Construct a Cube Mesh with given side length and position and default scale and rotation */
    explicit Cube(const std::string& name) : Mesh{name, position, scale, rotation} {
        initializeVertices();
        initializeFaceIndices();
        initializeEdges();

        buildEdgeToFaceMap();
    }

    ~Cube() override = default;

private:
    /** Initialize the Cube's vertices based on side length and position */
    void initializeVertices() override {
        // Front face
        vertices.emplace_back(Vector3(-1, -1,  1));  // Bottom-left
        vertices.emplace_back(Vector3( 1, -1,  1));  // Bottom-right
        vertices.emplace_back(Vector3( 1,  1,  1));  // Top-right
        vertices.emplace_back(Vector3(-1,  1,  1));  // Top-left

        // Back face
        vertices.emplace_back(Vector3(-1, -1, -1));  // Bottom-left
        vertices.emplace_back(Vector3( 1, -1, -1));  // Bottom-right
        vertices.emplace_back(Vector3( 1,  1, -1));  // Top-right
        vertices.emplace_back(Vector3(-1,  1, -1));  // Top-left
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

    void initializeEdges() override {
        edgeIndices = {
            // Front face edges
            0, 1, 1, 2, 2, 3, 3, 0,
            // Back face edges
            4, 5, 5, 6, 6, 7, 7, 4,
            // Connect front and back faces
            0, 4, 1, 5, 2, 6, 3, 7
        };
    }
};
