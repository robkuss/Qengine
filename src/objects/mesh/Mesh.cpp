#include "Mesh.h"

void Mesh::setPosition(const Vector3& translation) {
	this->position = Matrix4::translate(translation);
	for (Vector3& vertex : vertices) {
		vertex = vertex + translation;
	}
	buildEdgeToFaceMap();	// TODO: This should not be necessary here
}

void Mesh::setScale(const Vector3& scale) {
	this->scale	= Matrix4::scale(scale);
	for (Vector3& vertex : vertices) {
		vertex = vertex * scale;
	}
	buildEdgeToFaceMap();	// TODO: This should not be necessary here
}

void Mesh::setRotation(const Vector3& rotation) {
	this->rotation = Matrix4::rotateX(rotation.x) * Matrix4::rotateY(rotation.y) * Matrix4::rotateZ(rotation.z);
	buildEdgeToFaceMap();	// TODO: This should not be necessary here
}

void Mesh::applyTransformation(const Mode::ModeEnum mode, const Matrix4& transformation) {
	const auto oldPos = getPosition();
	const auto oldScale = getScale();

	switch (mode) {
		case Mode::GRAB: {
			position = transformation * position;
			const auto dPos = getPosition() - oldPos;
			for (Vector3& vertex : vertices) {
				vertex = vertex + dPos;
			}
			break;
		}
		case Mode::SCALE: {
			scale = transformation * scale;
			const auto dScale = getScale() / oldScale;
			for (Vector3& vertex : vertices) {
				vertex = (vertex - oldPos) * dScale + oldPos;
			}
			break;
		}
		case Mode::ROTATE: {
			rotation = transformation * rotation;
			for (Vector3& vertex : vertices) {
				vertex = vector3(transformation * vector4(vertex - oldPos)) + oldPos;
			}
			break;
		}
		default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
	}
	buildEdgeToFaceMap();	// TODO: This should not be necessary here
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

	for (const Triangle& t : getTriangles()) {
		addEdgeToMap(Edge(t.v0, t.v1), t);
		addEdgeToMap(Edge(t.v1, t.v2), t);
		addEdgeToMap(Edge(t.v2, t.v0), t);
	}
}

void Mesh::addEdgeToMap(const Edge& edge, const Triangle& t) {
	if (!edgeToFaceMap.contains(edge)) {
		// If the edge is not in the map, initialize it with a new vector containing the face
		edgeToFaceMap[edge] = std::vector {t};
	} else {
		// If the edge already exists, add face to the existing vector
		edgeToFaceMap[edge].push_back(t);
	}
}
