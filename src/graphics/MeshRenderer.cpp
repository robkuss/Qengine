#include "MeshRenderer.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include <GL/gl.h>

#include <viewport/scene/Mode.h>
#include "material/texture/Texture.h"
#include "color/Colors.h"

bool MeshRenderer::toggleDiffuse = false;

/** Reinterpret Vertex as GLfloat* */
void vertex3fv(const Vertex& v) {
	glVertex3fv(reinterpret_cast<const float *>(&v));
}

void MeshRenderer::renderVertex(const Vertex& v) {
	glBegin(GL_POINTS);
	vertex3fv(v);
	glEnd();
}

void MeshRenderer::renderEdge(const Edge& e, const Color& firstColor, const Color& secondColor) {
	glBegin(GL_LINES);
	color3f(firstColor);
	vertex3fv(*e.v0);
	color3f(secondColor);
	vertex3fv(*e.v1);
	glEnd();
}


void MeshRenderer::renderVertices(const Mesh& mesh, const std::vector<Vertex>& selectedVertices) {
	glPointSize(4.0f);

	for (const auto& v : mesh.vertices) {
		// Highlight if the Vertex is currently selected
		const auto color = std::ranges::find(selectedVertices, *v) != selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_VERT_COLOR;
		color3f(color);
		renderVertex(*v);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh, const std::vector<Vertex>& selectedVertices) {
	glLineWidth(2.0f);

	for (const auto& edge : mesh.edgeToFaceMap | std::views::keys) {
		// Highlight if either of the 2 Vertices of the Edge are currently selected
		const auto firstColor = std::ranges::find(selectedVertices, *edge.v0)
				!= selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		const auto secondColor = std::ranges::find(selectedVertices, *edge.v1)
				!= selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		renderEdge(edge, firstColor, secondColor);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh, const std::vector<Vertex>& selectedVertices) {
	// Function to draw a triangle with a specified color and transparency
	auto renderTriangle = [&mesh](const Triangle& t) {
		glBegin(GL_TRIANGLES);
		for (const auto& v : {t.v0, t.v1, t.v2}) {
			// Choose the shading mode
			const auto normal = mesh.shadingMode == ShadingMode::FLAT
				? t.normal		// Flat shading
				: v->normal;	// Smooth shading
			glNormal3f(normal.x, normal.y, normal.z);
			glTexCoord2f(static_cast<float>(v->texCoords.x), static_cast<float>(v->texCoords.y));
			vertex3fv(*v);
		}
		glEnd();
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh.diffuse.toGLfloat());
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh.ambient.toGLfloat());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh.specular.toGLfloat());
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh.emission.toGLfloat());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh.shininess);

	for (const auto& t : mesh.triangles) {
		// Highlight if all 3 Vertices of the Triangle are currently selected
		const auto vertices = {*t->v0, *t->v1, *t->v2};
		const auto isSelected = std::ranges::all_of(vertices,
		    [selectedVertices](const auto& vertex) {
			    return std::ranges::find(selectedVertices, vertex)
		    		!= selectedVertices.end();
		    });

		// Draw the mesh with the base color
		glBegin(GL_TRIANGLES);
		renderTriangle(*t);

		if (isSelected) {
			// Disable depth testing to ensure selection color overlays correctly
			glDisable(GL_DEPTH_TEST);

			// Draw the selection color
			if (!toggleDiffuse) {
				toggleDiffuse = true;
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Colors::MESH_SELECT_COLOR.transparent(0.4f).toGLfloat());
			}

			renderTriangle(*t);

			if (toggleDiffuse) {
				toggleDiffuse = false;
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh.diffuse.toGLfloat());
			}

			glEnable(GL_DEPTH_TEST);  // Re-enable depth testing
		}
	}
}


void MeshRenderer::render(const Mesh& mesh, const std::vector<Vertex>& selectedVertices, const Mode& selectionMode, const bool isMeshSelected) {
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	if (mesh.texture) glBindTexture(GL_TEXTURE_2D, mesh.texture->id);

	// Enable backface culling
	glEnable(GL_CULL_FACE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Draw the faces
	renderTriangles(mesh, selectedVertices);

	if (isMeshSelected && selectionMode == EDIT) {
		glDisable(GL_LIGHTING);

		// Draw the edges
		renderEdges(mesh, selectedVertices);

		// Draw the vertices
		renderVertices(mesh, selectedVertices);

		glEnable(GL_LIGHTING);
	}

	// Reset
	glDisable(GL_CULL_FACE);

	glDisable(GL_BLEND);

	if (mesh.texture) glBindTexture(GL_TEXTURE_2D, 0);
}

void MeshRenderer::renderSilhouette(const Mesh& mesh, const Mode& selectionMode, const Vector3& camPos, const bool isMeshSelected) {
	if (isMeshSelected && selectionMode == OBJECT) {
		// Highlight only the outline of the Mesh in Object Mode
		const auto color = Colors::MESH_SELECT_COLOR;
		glLineWidth(4.0f);
		glPointSize(3.0f);

		for (const auto&[first, second]: mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(second, camPos)) {
				renderEdge(first, color, color);

				// Highlight the vertices of the silhouette edges
				renderVertex(*first.v0);
				renderVertex(*first.v1);
			}
		}

		// Reset line width back to default
		glLineWidth(1.0f);
	}
}

bool MeshRenderer::isSilhouetteEdge(
	const std::vector<std::shared_ptr<Triangle>>& edgeAdjFaces,
	const Vector3& camPos
) {
	// Retrieve the triangles adjacent to the edge
	const auto& t1 = *edgeAdjFaces[0];
	const auto& t2 = *edgeAdjFaces[1];

	// Check for degenerate triangles
	if (t1.isDegenerate() || t2.isDegenerate()) return false; // Ignore edges where one or both adjacent triangles are degenerate

	// Check if triangles are front-facing or back-facing
	const bool t1ff = t1.normal.dot(camPos - t1.centroid) > 0;
	const bool t2ff = t2.normal.dot(camPos - t2.centroid) > 0;

	// Edge is a silhouette edge if one triangle is front-facing and the other is back-facing
	return t1ff != t2ff;
}
