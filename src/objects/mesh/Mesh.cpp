#include "Mesh.h"

void Mesh::setPosition(const Vector3& translation) {
	this->position = Matrix4::translate(translation);
	for (Vector3& vertex : vertices) {
		vertex = vertex + translation;
	}
}

void Mesh::setScale(const Vector3& scale) {
	this->scale	= Matrix4::scale(scale);
	for (Vector3& vertex : vertices) {
		vertex = vertex * scale;
	}
}

void Mesh::setRotation(const Vector3& rotation) {
	this->rotation = Matrix4::rotateX(rotation.x) * Matrix4::rotateY(rotation.y) * Matrix4::rotateZ(rotation.z);
}

void Mesh::applyTransformation(const Mode::ModeEnum mode, const Matrix4& transformation) {
	const auto oldPos = getPosition();
	const auto oldScale = getScale();

	switch (mode) {
		case Mode::GRAB: {
			position = transformation * position;
			const auto dPos = getPosition() - oldPos;
			for (Vertex& vertex : vertices) {
				vertex += dPos;
			}
			break;
		}
		case Mode::SCALE: {
			scale = transformation * scale;
			const auto dScale = getScale() / oldScale;
			for (Vertex& vertex : vertices) {
				vertex -= oldPos;	// Translate to origin
				vertex *= dScale;
				vertex += oldPos;	// Translate back
			}
			break;
		}
		case Mode::ROTATE: {
			rotation = transformation * rotation;
			for (Vertex& vertex : vertices) {
				vertex -= oldPos; // Translate to origin

				// Transform vertex in place
				const Vector4 temp = transformation * vector4(vertex);
				vertex.x = temp.x;
				vertex.y = temp.y;
				vertex.z = temp.z;

				vertex += oldPos; // Translate back
			}
			break;
		}
		default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
	}
}

Triangle Mesh::getTriangle(const int index) const {
	const Vertex& v0 = vertices[faceIndices[index]];
	const Vertex& v1 = vertices[faceIndices[index + 1]];
	const Vertex& v2 = vertices[faceIndices[index + 2]];

	return {v0, v1, v2};
}

std::vector<Triangle> Mesh::getTriangles() const {
	std::vector<Triangle> triangles;

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		triangles.emplace_back(getTriangle(i));
	}

	return triangles;
}

/** Build the edge-to-face adjacency map for the mesh */
void Mesh::buildEdgeToFaceMap() {
	edgeToFaceMap.clear();

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) { // Iterate in steps of 3
		const auto t = getTriangle(i);

		// Get the 3 vertices that form a Triangle
		const int v0 = faceIndices[i];
		const int v1 = faceIndices[i + 1];
		const int v2 = faceIndices[i + 2];

		// Add edges to the adjacency map (ensure that smaller index comes first for consistency)
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
