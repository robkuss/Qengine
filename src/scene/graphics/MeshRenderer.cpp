#include "MeshRenderer.h"

#include <GL/gl.h>

#include <ranges>

#include "scene/graphics/color/Colors.h"
#include "scene/RenderContext.h"

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
            vertex3fv(*t.v0);
            vertex3fv(*t.v1);
            vertex3fv(*t.v2);
        } else {
            const Vector3 n0 = t.v0->normal;
            const Vector3 n1 = t.v1->normal;
            const Vector3 n2 = t.v2->normal;

            glNormal3f(n0.x, n0.y, n0.z);
            vertex3fv(*t.v0);
            glNormal3f(n1.x, n1.y, n1.z);
            vertex3fv(*t.v1);
            glNormal3f(n2.x, n2.y, n2.z);
            vertex3fv(*t.v2);
        }
        glEnd();
    };

    // Enable lighting
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT1);
    //glEnable(GL_LIGHT2);

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
}

void MeshRenderer::renderVertices(const Mesh& mesh, const RenderContext& context) {
	glPointSize(4.0f);

	for (const auto& v : mesh.vertices) {
		// Highlight if the Vertex is currently selected
		const auto color = std::ranges::find(context.selectedVertices, *v) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_VERT_COLOR;
		color3f(color);
		renderVertex(*v);
	}
}

void MeshRenderer::renderEdges(const Mesh& mesh, const RenderContext& context) {
	glLineWidth(2.0f);

	for (const auto& edge : mesh.edgeToFaceMap | std::views::keys) {
		// Highlight if either of the 2 Vertices of the Edge are currently selected
		const auto firstColor = std::ranges::find(context.selectedVertices, *edge.v0) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		const auto secondColor = std::ranges::find(context.selectedVertices, *edge.v1) != context.selectedVertices.end()
			? Colors::MESH_SELECT_COLOR
			: Colors::MESH_EDGE_COLOR;
		renderEdge(edge, firstColor, secondColor);
	}
}

void MeshRenderer::renderTriangles(const Mesh& mesh, const RenderContext& context) {
	for (const auto& triangle : mesh.getTriangles()) {
		// Highlight if all 3 Vertices of the Triangle are currently selected
		const auto vertices = {*triangle.v0, *triangle.v1, *triangle.v2};
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

	if (isSelected) {
		if (context.selectionMode == EDIT) {
			// Draw the edges
			renderEdges(mesh, context);

			// Draw the vertices
			renderVertices(mesh, context);

		} else {
			// Highlight only the outline of the Mesh in Object Mode
			const auto color = Colors::MESH_SELECT_COLOR;
			glLineWidth(4.0f);
			glPointSize(3.0f);
			for (const auto& edgeEntry : mesh.edgeToFaceMap) {
				if (isSilhouetteEdge(edgeEntry, context)) {
					// Highlight the silhouette edges
					renderEdge(edgeEntry.first, color, color);

					// Also highlight the vertices of the silhouette edges
					renderVertex(*edgeEntry.first.v0);
					renderVertex(*edgeEntry.first.v1);
				}
			}
		}
	}

	// Reset line width back to default
	glLineWidth(1.0f);
}

bool MeshRenderer::isSilhouetteEdge(const std::pair<Edge, std::vector<Triangle>>& edgeEntry, const RenderContext& context) {
	// Retrieve the triangles adjacent to the edge
	const Triangle& tri1 = edgeEntry.second[0];
	const Triangle& tri2 = edgeEntry.second[1];

	// Compute face normals
	const Vector3 normal1 = Mesh::faceNormal(tri1);
	const Vector3 normal2 = Mesh::faceNormal(tri2);

	// Calculate camera view direction for each triangle
	const Vector3 viewDir1 = (context.camPos - tri1.centroid()).normalize();
	const Vector3 viewDir2 = (context.camPos - tri2.centroid()).normalize();

	// Check if triangles are front-facing or back-facing
	const bool tri1FrontFacing = normal1.dot(viewDir1) > 0;
	const bool tri2FrontFacing = normal2.dot(viewDir2) > 0;

	// Edge is a silhouette edge if one triangle is front-facing and the other is back-facing
	return tri1FrontFacing != tri2FrontFacing;
}
