#pragma once

#include "math/geometry/Edge.h"
#include "math/vector/Vector3.h"
#include "objects/mesh/Mesh.h"

class MeshRenderer {
public:
	static void render(Mesh &mesh, const Vector3 &camPos, bool isSelected, bool isEditMode);

	static bool isSilhouetteEdge(const std::vector<Triangle> &triangles, Vector3 camPos);

private:
	static void renderVertex(const Vertex& v);
	static void renderEdge(const Edge &e);
	static void renderTriangle(const Triangle& t);

	static void renderVertices(const Mesh& mesh);
	static void renderEdges(const Mesh& mesh);
	static void renderTriangles(const Mesh &mesh);
};
