#pragma once

#include "math/vector/Vector3.h"
#include "objects/mesh/Mesh.h"

class MeshRenderer {
public:
	static void render(Mesh mesh, Vector3 camPos, bool isSelected, bool isEditMode);

private:
	static bool isSilhouetteEdge(const Mesh& mesh, const std::vector<int>& faces, Vector3 camPos);

	static void renderVertex(Vertex v);
	static void renderEdge(Vertex v0, Vertex v1);
	static void renderTriangle(Triangle &triangle);

	static void renderVertices(const Mesh &mesh);
	static void renderEdges(const Mesh &mesh);
	static void renderTriangles(const Mesh &mesh);
};
