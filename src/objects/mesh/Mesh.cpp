#include "Mesh.h"

#include <vector>
#include <ranges>
#include <future>
#include <algorithm>

// Function that will process a batch of vertices
void transformBatch(const std::vector<std::shared_ptr<Vertex>>& verticesBatch, const Mode& mode, const Vector3& oldPos, const Vector3& dPos, const Vector3& dScale, const Matrix4& transformation) {
	for (auto& v : verticesBatch) {
		switch (mode.mode) {
			case Mode::GRAB:
				v->position = v->position + dPos;
			break;
			case Mode::SCALE:
				v->position = oldPos + (v->position - oldPos) * dScale;
			break;
			case Mode::ROTATE:
				v->position = oldPos + vector3(transformation * vector4(v->position - oldPos));
			break;
			default:
				break;
		}
	}
}

void Mesh::applyTransformation(const Mode& mode, const Matrix4& transformation) {
	const auto oldPos	= getPosition();
	const auto oldScale	= getScale();

	// Update Object transformation
	switch (mode.mode) {
		case Mode::GRAB:   position = transformation * position; break;
		case Mode::SCALE:  scale	= transformation * scale;	 break;
		case Mode::ROTATE: rotation = transformation * rotation; break;
		default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
	}

	const auto dPos = getPosition() - oldPos;
	const auto dScale = getScale() / oldScale;

	// Define the batch size
	constexpr size_t batchSize = 1000;
	const size_t numBatches = (vertices.size() + batchSize - 1) / batchSize;

	// Vector to store futures for parallel processing
	std::vector<std::future<void>> futures;

	// Process vertices in batches
	for (size_t batch = 0; batch < numBatches; ++batch) {
		const size_t startIdx = batch * batchSize;
		const size_t endIdx = std::min(startIdx + batchSize, vertices.size());

		// Create a sub-vector (batch) of vertices to process
		std::vector batchVertices(vertices.begin() + startIdx, vertices.begin() + endIdx); // NOLINT(*-narrowing-conversions)

		// Launch the batch processing in a separate thread or async task
		futures.push_back(std::async(std::launch::async, transformBatch, batchVertices, std::cref(mode), std::cref(oldPos), std::cref(dPos), std::cref(dScale), std::cref(transformation)));
	}

	// Wait for all batch processing tasks to complete
	for (auto& future : futures) {
		future.get();
	}

	updateNormals();
}

void Mesh::initializeTriangles() {
	triangles.clear();

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		const auto v0 = vertices[faceIndices[i]];
		const auto v1 = vertices[faceIndices[i + 1]];
		const auto v2 = vertices[faceIndices[i + 2]];

		triangles.emplace_back(std::make_shared<Triangle>(v0, v1, v2));
	}
}

/** Build the vertex-to-edge adjacency map for the mesh */
void Mesh::buildVertexToEdgeMap() {
	vertexToEdgeMap.clear();
	vertexToEdgeMap.reserve(vertices.size());	// Reserve space (this helps avoid reallocations)

	// Use shared_ptr to store the vertices in the map
	for (const auto& v : vertices) {
		for (const auto& e: edgeToFaceMap | std::views::keys) {
			if (e.v0 == v || e.v1 == v) {
				// Store the shared pointer to the vertex in the map
				if (!vertexToEdgeMap.contains(v)) {
					vertexToEdgeMap[v] = std::vector {e};
				} else {
					vertexToEdgeMap[v].push_back(e);
				}
			}
		}
	}
}

/** Build the edge-to-face adjacency map for the mesh */
void Mesh::buildEdgeToFaceMap() {
	edgeToFaceMap.clear();
	edgeToFaceMap.reserve(triangles.size() * 3);  // Reserve space (this helps avoid reallocations)

	for (const auto& t : triangles) {
		// Add edges to the adjacency map
		addEdgeToMap(Edge(t->v0, t->v1), t);
		addEdgeToMap(Edge(t->v1, t->v2), t);
		addEdgeToMap(Edge(t->v2, t->v0), t);
	}
}

/** Helper function to add an edge to the map */
void Mesh::addEdgeToMap(const Edge& e, const std::shared_ptr<Triangle>& t) {
	if (const auto it = edgeToFaceMap.find(e); it == edgeToFaceMap.end()) {
		// If edge is not found, initialize the vector with the triangle
		edgeToFaceMap[e] = {t};
	} else {
		// If edge is found, add the triangle to the vector
		it->second.push_back(t);
	}
}

void Mesh::updateNormals() const {
	// Update vertex normals
	for (const auto& v : vertices) {
		v->normal = (v->position - getPosition()).normalize();
	}

	// Update face normals
	for (const auto& t : triangles) {
		t->normal = faceNormal(*t);
		t->centroid = centroid(*t);
	}
}

void Mesh::setShadingMode(const ShadingMode shadingMode) {
	this->shadingMode = shadingMode;
}
