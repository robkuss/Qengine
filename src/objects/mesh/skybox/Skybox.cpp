#pragma once

#include "../Mesh.h"

class Skybox final : public Mesh {
public:
	explicit Skybox(const std::string& name, const Color& color, const std::shared_ptr<Texture> &texture) : Mesh{name, color, texture} {
		initializeVertices();
		initializeFaceIndices();

		initializeTriangles();
		buildVertexToEdgeMap();
		buildEdgeToFaceMap();
		updateNormals();

		Mesh::applyTransformation(OBJECT, GRAB, Matrix4::translate(position));
	}

	~Skybox() override = default;

private:
	void initializeVertices() override {
		vertices.reserve(24);

		// +y
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, -1}, {0, -1, 0}, {2 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, 1}, {0, -1, 0}, {2 / 3.f, 2 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, 1}, {0, -1, 0}, {1 / 3.f, 2 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, -1}, {0, -1, 0}, {1 / 3.f, 1 / 2.f})));

		// +z
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, 1}, {0, 0, -1}, {2 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, 1}, {0, 0, -1}, {3 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, 1}, {0, 0, -1}, {3 / 3.f, 2 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, 1}, {0, 0, -1}, {2 / 3.f, 2 / 2.f})));

		// -x
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, 1}, {1, 0, 0}, {0 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, -1}, {1, 0, 0}, {1 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, -1}, {1, 0, 0}, {1 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, 1}, {1, 0, 0}, {0 / 3.f, 1 / 2.f})));

		// -y
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, -1}, {0, 1, 0}, {1 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, 1}, {0, 1, 0}, {1 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, 1}, {0, 1, 0}, {2 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, -1}, {0, 1, 0}, {2 / 3.f, 1 / 2.f})));

		// +x
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, -1}, {-1, 0, 0}, {0 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, 1}, {-1, 0, 0}, {1 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, 1}, {-1, 0, 0}, {1 / 3.f, 2 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, -1}, {-1, 0, 0}, {0 / 3.f, 2 / 2.f})));

		// -z
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, -1, -1}, {0, 0, 1}, {2 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, -1, -1}, {0, 0, 1}, {3 / 3.f, 0 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({1, 1, -1}, {0, 0, 1}, {3 / 3.f, 1 / 2.f})));
		vertices.emplace_back(std::make_shared<Vertex>(Vertex({-1, 1, -1}, {0, 0, 1}, {2 / 3.f, 1 / 2.f})));
	}

	void initializeFaceIndices() override {
		faceIndices = {
			// +y (vertices 0-3)
			0, 1, 2,  // Triangle 1
			0, 2, 3,  // Triangle 2

			// +z (vertices 4-7)
			4, 5, 6,  // Triangle 1
			4, 6, 7,  // Triangle 2

			// -x (vertices 8-11)
			8, 9, 10, // Triangle 1
			8, 10, 11,// Triangle 2

			// -y (vertices 12-15)
			12, 13, 14, // Triangle 1
			12, 14, 15, // Triangle 2

			// +x (vertices 16-19)
			16, 17, 18, // Triangle 1
			16, 18, 19, // Triangle 2

			// -z (vertices 20-23)
			20, 21, 22, // Triangle 1
			20, 22, 23  // Triangle 2
		};
	}
};