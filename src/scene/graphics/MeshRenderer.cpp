#include "MeshRenderer.h"

// Ensure proper linkage and calling convention for Windows API functions
#define WINGDIAPI __declspec(dllimport)
#define APIENTRY __stdcall

#include <GL/gl.h>

#include <ranges>

#include <scene/graphics/color/Colors.h>

/** Reinterpret Vertex as GLfloat* */
void vertex3fv(const Vertex& v) {
	glVertex3fv(reinterpret_cast<const float *>(&v));
}

void MeshRenderer::renderVertex(const Vertex& v) {
	glBegin(GL_POINTS);
	vertex3fv(v);
	glEnd();
}

void MeshRenderer::renderEdge(const Mesh& mesh, const std::pair<int, int>& e) {
	glBegin(GL_LINES);
	vertex3fv(mesh.vertices[e.first]);
	vertex3fv(mesh.vertices[e.second]);
	glEnd();
}

void MeshRenderer::renderTriangle(const Mesh& mesh, const Triangle& t) {
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	// Apply material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::MESH_FACE_COLOR.toGLfloat());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Colors::MESH_FACE_COLOR.toGLfloat());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

	glBegin(GL_TRIANGLES);
	if (mesh.shadingMode == ShadingMode::FLAT) {
		// Compute face normals
		const Vector3 normal = Mesh::faceNormal(t);

		// Draw the Triangle with a single normal
		glNormal3f(normal.x, normal.y, normal.z);
		vertex3fv(t.v0);
		vertex3fv(t.v1);
		vertex3fv(t.v2);
	} else {
		// Compute vertex normals
		const Vector3 n0 = mesh.vertexNormal(t.v0);
		const Vector3 n1 = mesh.vertexNormal(t.v1);
		const Vector3 n2 = mesh.vertexNormal(t.v2);

		// Draw the Triangle with per-vertex normals
		glNormal3f(n0.x, n0.y, n0.z);
		vertex3fv(t.v0);
		glNormal3f(n1.x, n1.y, n1.z);
		vertex3fv(t.v1);
		glNormal3f(n2.x, n2.y, n2.z);
		vertex3fv(t.v2);
	}
	glEnd();

	// Disable lighting
	glDisable(GL_LIGHTING);
}

void MeshRenderer::renderVertices(const Mesh& mesh, const Viewport* vp) {
	for (const auto& v : mesh.vertices) {
		glPointSize(4.0f);
		renderVertex(v);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh) {
	for (const auto& edge: mesh.edgeToFaceMap | std::views::keys) {
		renderEdge(mesh, edge);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh) {
	for (const auto& triangle : mesh.getTriangles()) {
		renderTriangle(mesh, triangle);
	}
}

void MeshRenderer::renderProjectedVertices(const Mesh& mesh, const Viewport* vp) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, vp->viewport[2], vp->viewport[3], 0, -1, 1);

	// Switch to the model view matrix to render the text
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_POINTS);
	color3f(Color(255, 0, 0));
	for (const auto& v : mesh.vertices) {
		const auto projV = project(v, vp->viewport, vp->viewMatrix, vp->projMatrix);
		glVertex2f(static_cast<float>(projV.x), static_cast<float>(projV.y));
	}
	glEnd();

	// Restore the model view matrix
	glPopMatrix();

	// Restore the projection matrix (back to 3D perspective)
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Switch back to the model view matrix
	glMatrixMode(GL_MODELVIEW);
}

void MeshRenderer::render(const Mesh& mesh, const Vector3& camPos, const bool isSelected, const bool isEditMode, const Viewport* vp) {
	// Draw the faces
	renderTriangles(mesh);

	if (isEditMode) {
		// Draw the edges
		color3f(isSelected ? Colors::MESH_SELECT_COLOR : Colors::MESH_EDGE_COLOR);
		glLineWidth(2.0f);
		renderEdges(mesh);

		// Draw the vertices
		color3f(isSelected ? Colors::MESH_SELECT_COLOR : Colors::MESH_VERT_COLOR);
		glPointSize(4.0f);
		renderVertices(mesh, vp);

		#ifdef RENDER_PROJECTED_VERTICES
			renderProjectedVertices(mesh, vp);
		#endif
	} else if (isSelected) {
		// Highlight only the outline of the Mesh in Object Mode
		color3f(Colors::MESH_SELECT_COLOR);
		glLineWidth(4.0f);
		glPointSize(3.0f);
		for (const auto& [edge, triangles] : mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(triangles, camPos)) {
				// Highlight the silhouette edges
				renderEdge(mesh, edge);

				// Also highlight the vertices of the silhouette edges
				renderVertex(mesh.vertices[edge.first]);
				renderVertex(mesh.vertices[edge.second]);
			}
		}
	}

	// Reset line width back to default
	glLineWidth(1.0f);
}

bool MeshRenderer::isSilhouetteEdge(const std::vector<Triangle>& triangles, const Vector3 camPos) {
	if (triangles.size() == 1) return true;  // If only one face shares this edge, it's on the silhouette

	// Retrieve the normals of the two faces
	const Vector3 normal1 = Mesh::faceNormal(triangles[0]);
	const Vector3 normal2 = Mesh::faceNormal(triangles[1]);

	// Use any point from the first face to compute the direction to the camera
	const Vertex pointOnFace = triangles[0].v0; // Arbitrary point on the first face
	const Vector3 camDir = (pointOnFace - camPos).normalize();

	// Compute the dot products of the camera direction with the face normals
	const float dot1 = normal1.dot(camDir);
	const float dot2 = normal2.dot(camDir);

	// If one face is front-facing and the other is back-facing, the edge is part of the silhouette
	return (dot1 > 0 && dot2 < 0) || (dot1 < 0 && dot2 > 0);
}
