#include "Mesh.h"
#include "math/matrix/Matrix4.h"

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

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		const Triangle& t = getTriangle(i);

		// Add edges to the adjacency map
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

/** Calculate the normal for a face in the Mesh */
Vector3 Mesh::faceNormal(const Triangle& t) {
	// Compute the two edge vectors
	const Vector3 e1 = t.v1 - t.v0;
	const Vector3 e2 = t.v2 - t.v0;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

bool Mesh::isSilhouetteEdge(const std::vector<Triangle>& triangles, const Vector3 camPos) {
	if (triangles.size() == 1) return true;  // If only one face shares this edge, it's on the silhouette

	// Retrieve the normals of the two faces
	const Vector3 normal1 = faceNormal(triangles[0]);
	const Vector3 normal2 = faceNormal(triangles[1]);

	// Use any point from the first face to compute the direction to the camera
	const Vertex pointOnFace = triangles[0].v0; // Arbitrary point on the first face
	const Vector3 camDir = (pointOnFace - camPos).normalize();

	// Compute the dot products of the camera direction with the face normals
	const float dot1 = normal1.dot(camDir);
	const float dot2 = normal2.dot(camDir);

	// If one face is front-facing and the other is back-facing, the edge is part of the silhouette
	return (dot1 > 0 && dot2 < 0) || (dot1 < 0 && dot2 > 0);
}
