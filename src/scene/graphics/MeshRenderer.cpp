#include "MeshRenderer.h"

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

void MeshRenderer::renderEdge(const Vertex &v0, const Vertex &v1) {
	glBegin(GL_LINES);
	vertex3fv(v0);
	vertex3fv(v1);
	glEnd();
}

void MeshRenderer::renderTriangle(const Triangle& t) {
	glBegin(GL_TRIANGLES);
	vertex3fv(t.v0);
	vertex3fv(t.v1);
	vertex3fv(t.v2);
	glEnd();
}

void MeshRenderer::renderVertices(const Mesh& mesh) {
	for (auto vertex : mesh.vertices) {
		renderVertex(vertex);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh) {
	for (int i = 0; i < mesh.edgeIndices.size(); i += 2) {
		renderEdge(
			mesh.vertices[mesh.edgeIndices[i]],
			mesh.vertices[mesh.edgeIndices[i + 1]]
		);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh) {
	for (auto triangle : mesh.getTriangles()) {
		renderTriangle(triangle);
	}
}

void MeshRenderer::render(const Mesh& mesh, const Vector3 camPos, const bool isSelected, const bool isEditMode) {
	glPushMatrix();

	const Matrix4 worldMatrix = mesh.position * mesh.rotation * mesh.scale;
	float worldMatrixF[16];
	worldMatrix.toColumnMajor(worldMatrixF);
	glMultMatrixf(worldMatrixF);

	// Draw the faces
	color3f(MESH_FACE_COLOR);
	renderTriangles(mesh);

	if (isEditMode) {
		// Draw the edges
		color3f(isSelected ? MESH_SELECT_COLOR : MESH_EDGE_COLOR);
		renderEdges(mesh);

		// Draw the vertices
		color3f(isSelected ? MESH_SELECT_COLOR : MESH_VERT_COLOR);
		glPointSize(4.0f);
		renderVertices(mesh);

	} else if (isSelected) {
		// Highlight only the outline of the Mesh in Object Mode
		color3f(MESH_SELECT_COLOR);
		glLineWidth(4.0f);
		glPointSize(3.0f);
		for (const auto& [edge, faces] : mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(mesh, faces, camPos)) {
				// Highlight the silhouette edges
				renderEdge(
					mesh.vertices[edge.first],
					mesh.vertices[edge.second]
				);

				// Also highlight the vertices of the silhouette edges
				renderVertex(mesh.vertices[edge.first]);
				renderVertex(mesh.vertices[edge.second]);
			}
		}

		// Reset line width back to default
		glLineWidth(1.0f);
	}

	glPopMatrix();
}

bool MeshRenderer::isSilhouetteEdge(const Mesh& mesh, const std::vector<int>& faces, const Vector3 camPos) {
	if (faces.size() == 1) return true;	  // If only one face shares this edge, it's on the silhouette

	// Get the normals of the two faces
	const Vector3 normal1 = mesh.faceNormal(faces[0]);
	const Vector3 normal2 = mesh.faceNormal(faces[1]);

	// Use any point from the first face to compute the direction to the camera
	const Vertex pointOnFace = mesh.vertices[mesh.faceIndices[faces[0] * 3]];
	const Vector3 camDir = (pointOnFace - camPos).normalize();

	// Compute the dot products of the camera direction with the face normals
	const float dot1 = normal1.dot(camDir);
	const float dot2 = normal2.dot(camDir);

	// If one face is front-facing and the other is back-facing, the edge is part of the silhouette
	return (dot1 > 0 && dot2 < 0) || (dot1 < 0 && dot2 > 0);
}

