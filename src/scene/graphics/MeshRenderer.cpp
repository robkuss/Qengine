#include "MeshRenderer.h"

#include "color/Colors.h"	// Also includes <GL/gl.h>

/** Reinterpret Vertex as GLfloat* */
void vertex3fv(Vertex* v) {
	glVertex3fv(reinterpret_cast<float *>(v));
}

void MeshRenderer::render(Mesh mesh, Vector3 camPos, const bool isSelected, const bool isEditMode) {
	const Matrix4 worldMatrix = mesh.position * mesh.rotation * mesh.scale;

	glPushMatrix();

	float worldMatrixF[16];
	worldMatrix.toColumnMajor(worldMatrixF);
	glMultMatrixf(worldMatrixF);

	// Draw the faces in one color
	color3f(MESH_FACE_COLOR);
	glBegin(GL_TRIANGLES);
	for (auto triangle : mesh.getTriangles()) {
		vertex3fv(&triangle.v0);
		vertex3fv(&triangle.v1);
		vertex3fv(&triangle.v2);
	}
	glEnd();

	if (isEditMode) {
		// Draw the edges in a different color
		const Color edgeColor = isSelected ? MESH_SELECT_COLOR : MESH_EDGE_COLOR;
		color3f(edgeColor);
		glBegin(GL_LINES);
		for (int i = 0; i < mesh.edgeIndices.size(); i += 2) {
			Vertex v0 = mesh.vertices[mesh.edgeIndices[i]];
			Vertex v1 = mesh.vertices[mesh.edgeIndices[i + 1]];
			vertex3fv(&v0);
			vertex3fv(&v1);
		}
		glEnd();

		// Draw the vertices in another color
		const Color vertexColor = isSelected ? MESH_SELECT_COLOR : MESH_VERT_COLOR;
		color3f(vertexColor);
		glPointSize(4.0f);
		glBegin(GL_POINTS);
		for (auto vertex : mesh.vertices) {
			vertex3fv(&vertex);
		}
		glEnd();
	} else if (isSelected) {
		// Only highlight outline of the Mesh in Object Mode
		const Color outlineColor = MESH_SELECT_COLOR;
		color3f(outlineColor);
		glLineWidth(4.0f);	// Set a thicker line width for the outline
		for (const auto& [edge, faces] : mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(mesh, faces, camPos)) {
				glBegin(GL_LINES);
				Vertex v0 = mesh.vertices[edge.first];
				Vertex v1 = mesh.vertices[edge.second];
				vertex3fv(&v0);
				vertex3fv(&v1);
				glEnd();

				// Also highlight the vertices of the silhouette edges
				glPointSize(3.0f);
				glBegin(GL_POINTS);
				vertex3fv(&v0);
				vertex3fv(&v1);
				glEnd();
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
	return (dot1 > 0.0f && dot2 < 0.0f) || (dot1 < 0.0f && dot2 > 0.0f);
}

