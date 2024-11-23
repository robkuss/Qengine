#include "Mesh.h"
#include "math/matrix/Matrix4.h"

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
	switch (mode) {
		case Mode::GRAB: {
			const Vector3 oldPos = getPosition();
			this->position = transformation * this->position;
			for (Vector3& vertex : vertices) {
				vertex = vertex + (getPosition() - oldPos);
			}
			break;
		}
		case Mode::SCALE: {
			const Vector3 oldScale = getScale();
			this->scale = transformation * this->scale;
			for (Vector3& vertex : vertices) {
				vertex = (vertex - getPosition()) * (getScale() / oldScale) + getPosition();
			}
			break;
		}
		case Mode::ROTATE: {
			// Apply the rotation to each vertex
			this->rotation = transformation * this->rotation;
			for (Vector3& vertex : vertices) {
				vertex = vector3(transformation * vector4(vertex - getPosition())) + getPosition();
			}
			break;
		}
		default: break;
	}
}

std::vector<Triangle> Mesh::getTriangles() const {
	std::vector<Triangle> triangles;

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		triangles.emplace_back(
			vertices[faceIndices[i]],
			vertices[faceIndices[i + 1]],
			vertices[faceIndices[i + 2]]
		);
	}

	return triangles;
}

/** Build the edge-to-face adjacency map for the mesh */
void Mesh::buildEdgeToFaceMap() {
	edgeToFaceMap.clear();

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		const int faceIndex = i / 3;

		const int v0 = faceIndices[i];
		const int v1 = faceIndices[i + 1];
		const int v2 = faceIndices[i + 2];

		// Add edges to the adjacency map (ensure that smaller index comes first for consistency)
		addEdgeToMap(v0, v1, faceIndex);
		addEdgeToMap(v1, v2, faceIndex);
		addEdgeToMap(v2, v0, faceIndex);
	}
}

void Mesh::addEdgeToMap(int v0, int v1, const int faceIndex) {
	// Ensure that the order of the vertices is consistent
	if (const std::pair<int, int> edge = v0 < v1
			? std::make_pair(v0, v1)
			: std::make_pair(v1, v0);
			!edgeToFaceMap.contains(edge)) {
		// If the edge is not in the map, initialize it with a new vector containing faceIndex
		edgeToFaceMap[edge] = std::vector {faceIndex};
	} else {
		// If the edge already exists, add faceIndex to the existing vector
		edgeToFaceMap[edge].push_back(faceIndex);
	}
}

/** Calculate the normal for a face in the Mesh */
Vector3 Mesh::faceNormal(const int faceIndex) const {
	const Vertex v0 = vertices[faceIndices[faceIndex * 3]];
	const Vertex v1 = vertices[faceIndices[faceIndex * 3 + 1]];
	const Vertex v2 = vertices[faceIndices[faceIndex * 3 + 2]];

	// Compute the two edge vectors
	const Vector3 e1 = v1 - v0;
	const Vector3 e2 = v2 - v0;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}
