#include "Mesh.h"

#include <ranges>

void Mesh::setPosition(const Vector3& translation) {
	this->position = Matrix4::translate(translation);
	for (auto& vertex : vertices) {
		*vertex += translation;
	}
}

void Mesh::setScale(const Vector3& scale) {
	this->scale	= Matrix4::scale(scale);
	for (auto& vertex : vertices) {
		*vertex *= scale;
	}
}

void Mesh::setRotation(const Vector3& rotation) {
	this->rotation = Matrix4::rotateX(rotation.x) * Matrix4::rotateY(rotation.y) * Matrix4::rotateZ(rotation.z);
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

	// Update vertex data
	for (auto& vertex : vertices) {
		switch (mode.mode) {
			case Mode::GRAB:   *vertex = *vertex + getPosition() - oldPos; break;
			case Mode::SCALE:  *vertex = oldPos + (*vertex - oldPos) * (getScale() / oldScale); break;
			case Mode::ROTATE: *vertex = oldPos + vector3(transformation * vector4(*vertex - oldPos)); break;
			default: ;
		}
	}
}

Triangle Mesh::getTriangle(const int index) const {
	const auto v0 = vertices[faceIndices[index]];
	const auto v1 = vertices[faceIndices[index + 1]];
	const auto v2 = vertices[faceIndices[index + 2]];

	return {*v0, *v1, *v2};
}

std::vector<Triangle> Mesh::getTriangles() const {
	std::vector<Triangle> triangles;

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		triangles.emplace_back(getTriangle(i));
	}

	return triangles;
}

/** Build the vertex-to-edge adjacency map for the mesh */
void Mesh::buildVertexToEdgeMap() {
	vertexToEdgeMap.clear();

	// Use shared_ptr to store the vertices in the map
	for (const auto& v : vertices) {
		for (const auto& e: edgeToFaceMap | std::views::keys) {
			if (vertices[e.first] == v || vertices[e.second] == v) {
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

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) { // Iterate in steps of 3
		// Get the 3 vertices that form a Triangle
		const int v0 = faceIndices[i];
		const int v1 = faceIndices[i + 1];
		const int v2 = faceIndices[i + 2];

		const Triangle t = getTriangle(i);

		// Add edges to the adjacency map
		addEdgeToMap(v0, v1, t);
		addEdgeToMap(v1, v2, t);
		addEdgeToMap(v2, v0, t);
	}
}

/** Helper function to add an edge to the map */
void Mesh::addEdgeToMap(int v0, int v1, const Triangle& t) {
	// Ensure that the order of the vertices is consistent
	if (const std::pair<int, int> edge = v0 < v1
				? std::make_pair(v0, v1)
				: std::make_pair(v1, v0);
			!edgeToFaceMap.contains(edge)) {
		// If the edge is not in the map, initialize it with a new vector containing faceIndex
		edgeToFaceMap[edge] = std::vector {t};
	} else {
		// If the edge already exists, add faceIndex to the existing vector
		edgeToFaceMap[edge].push_back(t);
	}
}

void Mesh::setShadingMode(const ShadingMode shadingMode) {
	this->shadingMode = shadingMode;
}


// Helpers

/** Calculate the normal for a face in the Mesh */
Vector3 Mesh::faceNormal(const Triangle& t) {
	// Compute the two edge vectors
	const Vector3 e1 = t.v1 - t.v0;
	const Vector3 e2 = t.v2 - t.v0;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

/** Calculate the normal for a vertex in the Mesh */
Vector3 Mesh::vertexNormal(const Vertex& v) const {
	Vector3 accNormal(0.0f, 0.0f, 0.0f);

	for (Triangle t : getTriangles()) {
		// Check if the current triangle contains the vertex
		if (v == t.v0 || v == t.v1 || v == t.v2) {
			// Accumulate the face normal
			accNormal = accNormal + faceNormal(t);
		}
	}

	// Normalize the accumulated normal
	return accNormal.normalize();
}