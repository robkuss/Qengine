#include "Mesh.h"

#include <vector>
#include <ranges>

#include "math/Util.h"
#include "math/matrix/Matrix4.h"


/** Mesh-side transformation
 * (Matrix operations, updating normals)
 */
void Mesh::applyTransformation(const Mode& selectionMode, const Mode& transformMode, const Matrix4& transformation) {
	// Update Object transformation
	switch (transformMode.mode) {
		case Mode::GRAB:   position = vector3(transformation * vector4(position, 1.0f)); break;
		case Mode::SCALE:  scale	= vector3(transformation * vector4(scale, 1.0f));	 break;
		case Mode::ROTATE: {
			rotation = vector3(transformation * vector4(rotation, 0.0f));
			rotationEuler = rotationEuler + transformation.extractEulerAngles();
			break;
		}
		default: break;
	}

	for (const auto& v : vertices) {
		v->position = vector3(transformation * vector4(v->position - position, 1.0f)) + position;
	}

	updateNormals();
}

void Mesh::initializeTriangles() {
	for (int i = 0; i + 2 < faceIndices.size(); i += 3) {
		triangles.push_back(make_shared<Triangle>(
			vertices[faceIndices[i]],
			vertices[faceIndices[i + 1]],
			vertices[faceIndices[i + 2]]
		));
	}
}

/** Build the vertex-to-edge adjacency map for the mesh */
void Mesh::buildVertexToEdgeMap() {
	vertexToEdgeMap.clear();
	vertexToEdgeMap.reserve(vertices.size());	// Reserve space (this helps avoid reallocations)

	// Use shared_ptr to store the vertices in the map
	for (const auto& v : vertices) {
		for (const auto& e: edgeToFaceMap | views::keys) {
			if (e.v0 == v || e.v1 == v) {
				// Store the shared pointer to the vertex in the map
				if (!vertexToEdgeMap.contains(v)) {
					vertexToEdgeMap[v] = vector {e};
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
void Mesh::addEdgeToMap(const Edge& e, const shared_ptr<Triangle>& t) {
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
		v->normal = (v->position - position).normalize();
	}

	// Update face normals
	for (const auto& t : triangles) {
		t->normal = t->faceNormal();
		t->centroid = t->center();
	}
}

void Mesh::setShadingMode(const ShadingMode shadingMode) {
	this->shadingMode = shadingMode;
}

void Mesh::setMaterial(const Color &diffuse, const Color &specular, const Color &emission, const Color &ambient, const float shininess) {
	this->diffuse   = diffuse;
	this->specular  = specular;
	this->emission  = emission;
	this->ambient   = ambient;
	this->shininess = shininess;
}
