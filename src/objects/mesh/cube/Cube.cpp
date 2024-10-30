#ifndef CUBE_C
#define CUBE_C

#include "../Mesh.cpp"
#include "../../../math/vector/Vector3.cpp"

class Cube final : public Mesh {
public:
    float s;  // side length

    // Constructor & Destructor
    Cube(const std::string& name, const Vector3& position, const float s)
            : Mesh{name, position, Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f)}, s{s} {
        initializeVertices();
        initializeFaceIndices();
        initializeEdgeIndices();

        buildEdgeToFaceMap();
    }

    ~Cube() override = default;

private:
    /** Initialize the cube vertices based on side length and position */
    void initializeVertices() {
        // Front face
        vertices.emplace_back(-s / 2 + position.x, -s / 2 + position.y,  s / 2 + position.z);  // Bottom-left
        vertices.emplace_back( s / 2 + position.x, -s / 2 + position.y,  s / 2 + position.z);  // Bottom-right
        vertices.emplace_back( s / 2 + position.x,  s / 2 + position.y,  s / 2 + position.z);  // Top-right
        vertices.emplace_back(-s / 2 + position.x,  s / 2 + position.y,  s / 2 + position.z);  // Top-left

        // Back face
        vertices.emplace_back(-s / 2 + position.x, -s / 2 + position.y, -s / 2 + position.z);  // Bottom-left
        vertices.emplace_back( s / 2 + position.x, -s / 2 + position.y, -s / 2 + position.z);  // Bottom-right
        vertices.emplace_back( s / 2 + position.x,  s / 2 + position.y, -s / 2 + position.z);  // Top-right
        vertices.emplace_back(-s / 2 + position.x,  s / 2 + position.y, -s / 2 + position.z);  // Top-left
    }

    void initializeFaceIndices() {
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

    void initializeEdgeIndices() {
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

#endif // CUBE_C