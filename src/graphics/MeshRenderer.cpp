#include "MeshRenderer.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include <GL/gl.h>

#include "viewport/scene/Mode.h"
#include "material/texture/Texture.h"
#include "color/Colors.h"

bool MeshRenderer::toggleDiffuse = false;


/** Reinterpret Vertex as GLfloat* */
void vertex3fv(const Vertex& v) {
	glVertex3fv(reinterpret_cast<const float *>(&v));
}

void MeshRenderer::renderVertex(const Vertex& v) {
	vertex3fv(v);
}

void MeshRenderer::renderEdge(const Edge& e, const Color& c1, const Color& c2) {
	color3f(c1);
	vertex3fv(*e.v0);
	color3f(c2);
	vertex3fv(*e.v1);
}


void MeshRenderer::renderVertices(const Mesh& mesh) {
	glPointSize(4.0f);

	for (const auto& v : mesh.vertices) {
		// Highlight if the Vertex is currently selected
		const auto color = v->isSelected
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_VERT_COLOR;
		color3f(color);
		renderVertex(*v);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh) {
	glLineWidth(2.0f);

	for (const auto& edge : mesh.edgeToFaceMap | views::keys) {
		// Highlight if either of the 2 Vertices of the Edge are currently selected
		const auto firstColor = edge.v0->isSelected
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		const auto secondColor = edge.v1->isSelected
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		renderEdge(edge, firstColor, secondColor);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh) {
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
		const auto isSelected = t->v0->isSelected && t->v1->isSelected && t->v2->isSelected;

		// Draw the mesh with the base color
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


void MeshRenderer::render(const Mesh& mesh, const Mode& selectionMode, const bool isMeshSelected) {
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	if (mesh.texture) glBindTexture(GL_TEXTURE_2D, mesh.texture->id);

	// Enable backface culling
	glEnable(GL_CULL_FACE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Draw the faces
	renderTriangles(mesh);

	if (isMeshSelected && selectionMode == EDIT) {
		glDisable(GL_LIGHTING);

		// Draw the edges
		glBegin(GL_LINES);
		renderEdges(mesh);
		glEnd();

		// Draw the vertices
		glBegin(GL_POINTS);
		renderVertices(mesh);
		glEnd();

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
				glBegin(GL_LINES);
				renderEdge(first, color, color);
				glEnd();

				// Highlight the vertices of the silhouette edges
				glBegin(GL_POINTS);
				renderVertex(*first.v0);
				renderVertex(*first.v1);
				glEnd();
			}
		}

		// Reset line width back to default
		glLineWidth(1.0f);
	}
}

bool MeshRenderer::isSilhouetteEdge(
	const vector<shared_ptr<Triangle>>& edgeAdjFaces,
	const Vector3& camPos
) {
	const auto& t1 = *edgeAdjFaces[0];
	const auto& t2 = *edgeAdjFaces[1];

	const bool t1degen = t1.isDegenerate();
	const bool t2degen = t2.isDegenerate();

	if (t1degen && t2degen) return false; // Ignore edges fully enclosed by degenerate triangles

	const bool t1ff = !t1degen && t1.normal.dot(camPos - t1.centroid) > 0.0f;
	const bool t2ff = !t2degen && t2.normal.dot(camPos - t2.centroid) > 0.0f;

	return t1degen || t2degen	// If only one triangle is degenerate...
		? !(t1ff || t2ff)		// ...Edge is silhouette if the non-degenerate Triangle is back-facing
		:   t1ff != t2ff;		// ...otherwise, Edge is silhouette if one Triangle is front-facing and the other is back-facing
}
