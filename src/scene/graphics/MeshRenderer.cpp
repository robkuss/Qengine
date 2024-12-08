#include "MeshRenderer.h"

#include <GL/gl.h>

#include <ranges>

#include "scene/graphics/color/Colors.h"
#include "scene/RenderContext.h"

/** Reinterpret Vertex as GLfloat* */
void vertex3fv(const std::shared_ptr<Vertex>& v) {
	glVertex3fv(reinterpret_cast<const float *>(&*v));
}

void MeshRenderer::renderVertex(const std::shared_ptr<Vertex>& v) {
	glBegin(GL_POINTS);
	vertex3fv(v);
	glEnd();
}

void MeshRenderer::renderEdge(const Mesh& mesh, const std::pair<int, int>& e, const Color& firstColor, const Color& secondColor) {
	glBegin(GL_LINES);
	color3f(firstColor);
	vertex3fv(mesh.vertices[e.first]);
	color3f(secondColor);
	vertex3fv(mesh.vertices[e.second]);
	glEnd();
}

void MeshRenderer::renderTriangle(const Mesh& mesh, const Triangle& t, const bool isSelected) {
    // Function to draw a triangle with a specified color and transparency
    auto drawWithColor = [&mesh, &t](const Color& color) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color.toGLfloat());
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color.toGLfloat());
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

        glBegin(GL_TRIANGLES);
        // Choose the shading mode
        if (mesh.shadingMode == ShadingMode::FLAT) {
            const Vector3 normal = Mesh::faceNormal(t);
            glNormal3f(normal.x, normal.y, normal.z);
            vertex3fv(t.v0);
            vertex3fv(t.v1);
            vertex3fv(t.v2);
        } else {
            const Vector3 n0 = mesh.vertexNormal(t.v0);
            const Vector3 n1 = mesh.vertexNormal(t.v1);
            const Vector3 n2 = mesh.vertexNormal(t.v2);

            glNormal3f(n0.x, n0.y, n0.z);
            vertex3fv(t.v0);
            glNormal3f(n1.x, n1.y, n1.z);
            vertex3fv(t.v1);
            glNormal3f(n2.x, n2.y, n2.z);
            vertex3fv(t.v2);
        }
        glEnd();
    };

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
	glEnable(GL_CULL_FACE);   // Enable face culling to avoid rendering back faces

    // Draw the mesh with the base color
    drawWithColor(Colors::MESH_FACE_COLOR);

    if (isSelected) {
    	// Disable depth testing to ensure selection color overlays correctly
    	glDisable(GL_DEPTH_TEST);

        // Enable blending only for the selection color
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw the selection color
        drawWithColor(Colors::MESH_SELECT_COLOR.transparent(0.3f));

        // Disable blending after drawing the selection color
        glDisable(GL_BLEND);
    	glEnable(GL_DEPTH_TEST);  // Re-enable depth testing
    }

    // Disable lighting after rendering
    glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);  // Disable face culling when done
}

void MeshRenderer::renderVertices(const Mesh& mesh, const RenderContext& context) {
	glPointSize(4.0f);

	for (const auto& v : mesh.vertices) {
		// Highlight if the Vertex is currently selected
		const auto color = std::ranges::find(context.selectedVertices, v) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_VERT_COLOR;
		color3f(color);
		renderVertex(v);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh, const RenderContext& context) {
	glLineWidth(2.0f);

	for (const auto& edge : mesh.edgeToFaceMap | std::views::keys) {
		// Highlight if either of the 2 Vertices of the Edge are currently selected
		const auto firstColor = std::ranges::find(context.selectedVertices, mesh.vertices[edge.first]) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		const auto secondColor = std::ranges::find(context.selectedVertices, mesh.vertices[edge.second]) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		renderEdge(mesh, edge, firstColor, secondColor);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh, const RenderContext& context) {
	for (const auto& triangle : mesh.getTriangles()) {
		// Highlight if all 3 Vertices of the Triangle are currently selected
		const auto vertices = {triangle.v0, triangle.v1, triangle.v2};
		const auto isSelected = std::ranges::all_of(vertices,
		    [&context](const auto& vertex) {
			    return std::ranges::find(context.selectedVertices, vertex) != context.selectedVertices.end();
		    });
		renderTriangle(mesh, triangle, isSelected);
	}
}

void MeshRenderer::render(const Mesh& mesh, const RenderContext& context) {
	const bool isSelected = std::ranges::find_if(
		context.selectedObjects,
		[&mesh](const std::shared_ptr<Object>& obj) {
			const auto meshPtr = dynamic_cast<const Mesh*>(obj.get());
			return meshPtr != nullptr && *meshPtr == mesh;
		}
	) != context.selectedObjects.end();

	// Draw the faces
	renderTriangles(mesh, context);

	if (context.selectionMode == EDIT) {
		// Draw the edges
		renderEdges(mesh, context);

		// Draw the vertices
		renderVertices(mesh, context);

	} else if (isSelected) {
		// Highlight only the outline of the Mesh in Object Mode
		const auto color = Colors::MESH_SELECT_COLOR;
		glLineWidth(4.0f);
		glPointSize(3.0f);
		for (const auto& [edge, triangles] : mesh.edgeToFaceMap) {
			if (isSilhouetteEdge(triangles, context.camPos)) {
				// Highlight the silhouette edges
				renderEdge(mesh, edge, color, color);

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
	const Vertex pointOnFace = *triangles[0].v0; // Arbitrary point on the first face
	const Vector3 camDir = (pointOnFace - camPos).normalize();

	// Compute the dot products of the camera direction with the face normals
	const float dot1 = normal1.dot(camDir);
	const float dot2 = normal2.dot(camDir);

	// If one face is front-facing and the other is back-facing, the edge is part of the silhouette
	return (dot1 > 0 && dot2 < 0) || (dot1 < 0 && dot2 > 0);
}
