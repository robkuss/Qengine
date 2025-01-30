#pragma once

#include "objects/mesh/Mesh.h"


class Sphere final : public Mesh {
public:
    /** Construct a Sphere Mesh with given radius, position, and segment count */
    explicit Sphere(const string& name, const Vector3& position, const float radius, const int segments, const int rings,
        const Color& color, const shared_ptr<Texture>& texture)
            : Mesh{name, color, texture}, radius(radius), segments(segments), rings(rings) {
        initializeVertices();
        initializeFaceIndices();

        initializeTriangles();
        buildVertexToEdgeMap();
        buildEdgeToFaceMap();
        updateNormals();

        Mesh::applyTransformation(OBJECT, GRAB, Matrix4::translate(position));
    }

    ~Sphere() override = default;

private:
    float radius;   // Sphere radius
    int segments;   // Number of vertical slices
    int rings;      // Number of horizontal slices

    /** Initialize the Sphere's Vertices based on radius, segments, and rings */
    void initializeVertices() override {
        // Top pole (north pole)
        vertices.emplace_back(make_shared<Vertex>(0.0f, 0.0f, radius, Vector2{0.5f, 1.0f}));

        // Latitude rings
        for (int ring = 1; ring < rings; ++ring) {
            const auto theta = static_cast<float>(PI * ring / rings); // Latitude angle
            const float sinTheta = sin(theta);
            const float cosTheta = cos(theta);
            for (int seg = 0; seg < segments + 1; ++seg) {
                const auto phi = seg == segments ? 0 : static_cast<float>(2 * PI * seg / segments); // Longitude angle
                const float sinPhi = sin(phi);
                const float cosPhi = cos(phi);

                float x = radius * sinTheta * cosPhi;
                float y = radius * sinTheta * sinPhi;
                float z = radius * cosTheta;

                // Calculate texture coordinates
                const float u = static_cast<float>(seg) / static_cast<float>(segments);
                const float v = 1.0f - static_cast<float>(ring) / static_cast<float>(rings); // Invert v to fix upside-down texture

                vertices.emplace_back(make_shared<Vertex>(x, y, z, Vector2{u, v}));
            }
        }

        // Bottom pole (south pole)
        vertices.emplace_back(make_shared<Vertex>(0.0f, 0.0f, -radius, Vector2{0.5f, 0.0f}));
    }

    /** Initialize the Sphere's face indices to form the Mesh */
    void initializeFaceIndices() override {
        // Top pole faces
        for (int seg = 0; seg < segments + 1; ++seg) {
            const int nextSeg = (seg + 1) % (segments + 1);
            faceIndices.push_back(1 + seg);       // First vertex
            faceIndices.push_back(1 + nextSeg);   // Second vertex
            faceIndices.push_back(0);             // Top pole vertex
        }

        // Middle faces
        for (int ring = 0; ring < rings - 2; ++ring) {
            for (int seg = 0; seg < segments + 1; ++seg) {
                const int nextSeg = (seg + 1) % (segments + 1);
                int current       = 1 + ring * (segments + 1) + seg;
                int next          = 1 + ring * (segments + 1) + nextSeg;
                int below         = 1 + (ring + 1) * (segments + 1) + seg;
                int belowNext     = 1 + (ring + 1) * (segments + 1) + nextSeg;

                // First triangle
                faceIndices.push_back(current);
                faceIndices.push_back(below);
                faceIndices.push_back(next);

                // Second triangle
                faceIndices.push_back(next);
                faceIndices.push_back(below);
                faceIndices.push_back(belowNext);
            }
        }

        // Bottom pole faces
        const int bottomPoleIndex = static_cast<int>(vertices.size()) - 1;
        for (int seg = 0; seg < segments + 1; ++seg) {
            const int nextSeg = (seg + 1) % (segments + 1);
            faceIndices.push_back(bottomPoleIndex - (segments + 1) + nextSeg); // First vertex
            faceIndices.push_back(bottomPoleIndex - (segments + 1) + seg);     // Second vertex
            faceIndices.push_back(bottomPoleIndex);                         // Bottom pole vertex
        }
    }
};