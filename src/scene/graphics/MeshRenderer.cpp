#ifndef MESHRENDERER_C
#define MESHRENDERER_C

#include "color/Colors.h"

#include <GL/gl.h>


class MeshRenderer {
public:
	static void render(Mesh, Vector3, bool, bool);

private:
	static bool isSilhouetteEdge(const Mesh&, const std::vector<int>&, Vector3);
};

void MeshRenderer::render(Mesh mesh, Vector3 camPos, const bool isSelected, const bool isEditMode) {
	// Draw the faces in one color
	color3f(MESH_FACE_COLOR);
	glBegin(GL_TRIANGLES);
	for (const auto triangle : mesh.getTriangles()) {
		glVertex3f(triangle.v0.x, triangle.v0.y, triangle.v0.z);
		glVertex3f(triangle.v1.x, triangle.v1.y, triangle.v1.z);
		glVertex3f(triangle.v2.x, triangle.v2.y, triangle.v2.z);
	}
	glEnd();

	if (isEditMode) {
		// Draw the edges in a different color
		const Color edgeColor = isSelected ? MESH_SELECT_COLOR : MESH_EDGE_COLOR;
		color3f(edgeColor);
		glBegin(GL_LINES);
		for (int i = 0; i < mesh.edgeIndices.size(); i += 2) {
			const Vector3 v0 = mesh.vertices[mesh.edgeIndices[i]];
			const Vector3 v1 = mesh.vertices[mesh.edgeIndices[i + 1]];
			glVertex3f(v0.x, v0.y, v0.z);
			glVertex3f(v1.x, v1.y, v1.z);
		}
		glEnd();

		// Draw the vertices in another color
		const Color vertexColor = isSelected ? MESH_SELECT_COLOR : MESH_VERT_COLOR;
		color3f(vertexColor);
		glPointSize(4.0f);
		glBegin(GL_POINTS);
		for (const auto vertex : mesh.vertices) {
			glVertex3f(vertex.x, vertex.y, vertex.z);
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
				const Vector3 v0 = mesh.vertices[edge.first];
				const Vector3 v1 = mesh.vertices[edge.second];
				glVertex3f(v0.x, v0.y, v0.z);
				glVertex3f(v1.x, v1.y, v1.z);
				glEnd();

				// Also highlight the vertices of the silhouette edges
				glPointSize(3.0f);
				glBegin(GL_POINTS);
				glVertex3f(v0.x, v0.y, v0.z);
				glVertex3f(v1.x, v1.y, v1.z);
				glEnd();
			}
		}

		// Reset line width back to default
		glLineWidth(1.0f);
	}
}

bool MeshRenderer::isSilhouetteEdge(const Mesh& mesh, const std::vector<int>& faces, const Vector3 camPos) {
	if (faces.size() == 1) return true;	  // If only one face shares this edge, it's on the silhouette

	// Get the normals of the two faces
	const Vector3 normal1 = mesh.faceNormal(faces[0]);
	const Vector3 normal2 = mesh.faceNormal(faces[1]);

	// Use any point from the first face to compute the direction to the camera
	const Vector3 pointOnFace = mesh.vertices[mesh.faceIndices[faces[0] * 3]];
	const Vector3 camDir = (pointOnFace - camPos).normalize();

	// Compute the dot products of the camera direction with the face normals
	const float dot1 = normal1.dot(camDir);
	const float dot2 = normal2.dot(camDir);

	// If one face is front-facing and the other is back-facing, the edge is part of the silhouette
	return (dot1 > 0.0f && dot2 < 0.0f) || (dot1 < 0.0f && dot2 > 0.0f);
}

#endif //MESHRENDERER_C
