#include "Mesh.h"

#include "../../math/Util.h"

void Mesh::applyTransformation(const Mode::ModeEnum mode, const Matrix4& transformation) {
    switch (mode) {
        case Mode::GRAB: {
        	// Extract translation from the transformation matrix (last column of the matrix)
	        const Vector3 translation(
        		transformation[3],
        		transformation[7],
        		transformation[11]
        	);
        	position = position + translation;

        	// Apply the translation to each vertex
        	for (Vector3& vertex : vertices) {
		        const Vector4 transformedVertex = transformation * vertex.toVector4();
        		vertex = Vector3(transformedVertex.x, transformedVertex.y, transformedVertex.z) + translation;
        	}
            break;
        }
        case Mode::SCALE: {
            // Extract scale values from the matrix
            const Vector3 scaleFactors(
                transformation[0],
                transformation[5],
                transformation[10]
            );
            const Vector3 oldScale = scale;
            scale = scaleFactors;

            // Apply scaling relative to the mesh position
            for (Vector3& vertex : vertices) {
                Vector3 relativePosition = vertex - position;
                relativePosition = Vector3(
                    relativePosition.x * (scale.x / oldScale.x),
                    relativePosition.y * (scale.y / oldScale.y),
                    relativePosition.z * (scale.z / oldScale.z)
                );
                vertex = relativePosition + position;
            }
            break;
        }
        case Mode::ROTATE: {
            rotation = rotation + Vector3(
                transformation[0],
                transformation[5],
                transformation[10]
            );

            // Apply the rotation to each vertex
            for (Vector3& vertex : vertices) {
                Vector3 relativePosition = vertex - position;
                const Vector4 rotatedPosition = transformation * relativePosition.toVector4();
            	vertex = Vector3(
					rotatedPosition.x + position.x,
					rotatedPosition.y + position.y,
					rotatedPosition.z + position.z
				);
            }
            break;
        }
    	case Mode::EXTRUDE: {
        	break;
    	}
    	case Mode::FILL: {
        	break;
    	}
    	case Mode::MERGE: {
        	break;
    	}
    	default: break;
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
