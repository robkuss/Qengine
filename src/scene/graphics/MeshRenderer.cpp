#include "MeshRenderer.h"

#include <ranges>

#include "color/Colors.h"	// Also includes <GL/gl.h>

/** Reinterpret Vertex as GLfloat* */
void vertex3fv(const Vertex& v) {
	glVertex3fv(reinterpret_cast<const float *>(&v));
}

void MeshRenderer::renderVertex(const Vertex& v) {
	glBegin(GL_POINTS);
	vertex3fv(v);
	glEnd();
}

void MeshRenderer::renderEdge(const Edge& e) {
	glBegin(GL_LINES);
	vertex3fv(e.v0);
	vertex3fv(e.v1);
	glEnd();
}

void MeshRenderer::renderTriangle(const Triangle& t) {
	// Compute and apply the normal for the Triangle
	const Vector3 normal = faceNormal(t);
	glNormal3f(normal.x, normal.y, normal.z);

	// Apply material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::MESH_FACE_COLOR.toGLfloat());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Colors::MESH_FACE_COLOR.toGLfloat());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

	// Draw the Triangle
	glBegin(GL_TRIANGLES);
	vertex3fv(t.v0);
	vertex3fv(t.v1);
	vertex3fv(t.v2);
	glEnd();
}

void MeshRenderer::renderVertices(const Mesh& mesh) {
	for (const auto& vertex : mesh.vertices) {
		renderVertex(vertex);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh) {
	for (const auto& edge: mesh.edgeToFaceMap | std::views::keys) {
		renderEdge(edge);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh) {
	for (const auto& triangle : mesh.getTriangles()) {
		renderTriangle(triangle);
	}
}

void MeshRenderer::render(Mesh& mesh, const Vector3& camPos, const bool isSelected, const bool isEditMode) {
	// Draw the faces (with flat shading)
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	renderTriangles(mesh);
	glDisable(GL_LIGHTING);

	if (isEditMode) {
		// Draw the edges
		color3f(isSelected ? Colors::MESH_SELECT_COLOR : Colors::MESH_EDGE_COLOR);
		renderEdges(mesh);

		// Draw the vertices
		color3f(isSelected ? Colors::MESH_SELECT_COLOR : Colors::MESH_VERT_COLOR);
		glPointSize(4.0f);
		renderVertices(mesh);
	} else if (isSelected) {
		// Highlight only the outline of the Mesh in Object Mode
		color3f(Colors::MESH_SELECT_COLOR);
		glLineWidth(4.0f);
		glPointSize(3.0f);
		for (const auto& [edge, triangles] : mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(triangles, camPos)) {
				// Highlight the silhouette edges
				renderEdge(edge);

				// Also highlight the vertices of the silhouette edges
				renderVertex(edge.v0);
				renderVertex(edge.v1);
			}
		}

		// Reset line width back to default
		glLineWidth(1.0f);
	}
}

bool MeshRenderer::isSilhouetteEdge(const std::vector<Triangle>& triangles, const Vector3 camPos) {
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
