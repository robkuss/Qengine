#include "Mesh.h"

#include "../../math/Util.h"

/** Apply transformations to the object */
void Mesh::applyTransformation(const Mode::ModeEnum mode, const Vector3 transformation) {
	switch (mode) {
		case Mode::GRAB : {
			position = position + transformation;

			// Translate each vertex by the same transformation
			for (Vector3& vertex : vertices) {
				vertex = vertex + transformation;
			}
			break;
		}
		case Mode::SCALE : {
			const Vector3 oldScale = scale;
			scale = transformation;

			// Apply scaling to each vertex based on the new scale
			for (Vector3& vertex : vertices) {
				vertex = (vertex - position) * (scale / oldScale) + position;
			}
			break;
		}
		case Mode::ROTATE : {
			rotation = rotation + transformation;

			// Get the rotation angles in degrees from the transformation vector
			const Vector3 rotationRadians = transformation * PI / 180.f;

			// Calculate the sine and cosine for the rotation angles
			const float sinX = std::sin(rotationRadians.x);
			const float cosX = std::cos(rotationRadians.x);
			const float sinY = std::sin(rotationRadians.y);
			const float cosY = std::cos(rotationRadians.y);
			const float sinZ = std::sin(rotationRadians.z);
			const float cosZ = std::cos(rotationRadians.z);

			// Apply the rotation to each vertex
			for (Vector3& vertex : vertices) {
				// Translate vertex to origin
				Vector3 tmp = vertex - position;

				// Rotate around x-axis
				const float newY1 = tmp.y * cosX - tmp.z * sinX;
				const float newZ1 = tmp.y * sinX + tmp.z * cosX;
				tmp = Vector3(tmp.x, newY1, newZ1);

				// Rotate around y-axis
				const float newX2 = tmp.x * cosY + tmp.z * sinY;
				const float newZ2 = -tmp.x * sinY + tmp.z * cosY;
				tmp = Vector3(newX2, tmp.y, newZ2);

				// Rotate around z-axis
				const float newX3 = tmp.x * cosZ - tmp.y * sinZ;
				const float newY3 = tmp.x * sinZ + tmp.y * cosZ;
				tmp = Vector3(newX3, newY3, tmp.z);

				// Translate back to the original position
				vertex = tmp + position;
			}
			break;
		}
		default : { /* TODO("Not yet implemented") */ }
	}
}

std::vector<Triangle> Mesh::getTriangles() {
	std::vector<Triangle> triangles;

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) { // Iterate in steps of 3
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

	for (int i = 0; i + 2 < faceIndices.size(); i += 3) { // Iterate in steps of 3
		const int faceIndex = i / 3;

		// Get the 3 vertices that form a Triangle
		const int v0 = faceIndices[i];
		const int v1 = faceIndices[i + 1];
		const int v2 = faceIndices[i + 2];

		// Add edges to the adjacency map (ensure that smaller index comes first for consistency)
		addEdgeToMap(v0, v1, faceIndex);
		addEdgeToMap(v1, v2, faceIndex);
		addEdgeToMap(v2, v0, faceIndex);
	}
}

/** Helper function to add an edge to the map */
void Mesh::addEdgeToMap(int v0, int v1, const int faceIndex) {
	// Ensure that the order of the vertices is consistent
	if (const std::pair<int, int> edge = (v0 < v1)
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
	const Vector3 v0 = vertices[faceIndices[faceIndex * 3]];
	const Vector3 v1 = vertices[faceIndices[faceIndex * 3 + 1]];
	const Vector3 v2 = vertices[faceIndices[faceIndex * 3 + 2]];

	// Compute the two edge vectors
	const Vector3 e1 = v1 - v0;
	const Vector3 e2 = v2 - v0;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}
