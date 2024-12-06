#pragma once

#include <utility>
#include <vector>

#include <objects/mesh/Mesh.h>

#include "Viewport.h"

class Vector3;
struct Vertex;
struct Triangle;


class MeshRenderer {
public:
	static void render(const Mesh &mesh, const Vector3 &camPos, bool isSelected, bool isEditMode, const Viewport *vp);

private:
	static void renderVertex(const Vertex& v);
	static void renderEdge(const Mesh &mesh, const std::pair<int, int> &e);
	static void renderTriangle(const Mesh &mesh, const Triangle &t);

	static void renderVertices(const Mesh& mesh, const Viewport* vp);
	static void renderEdges(const Mesh& mesh);
	static void renderTriangles(const Mesh &mesh);

	static void renderProjectedVertices(const Mesh &mesh, const Viewport *vp);

	static bool isSilhouetteEdge(const std::vector<Triangle> &triangles, Vector3 camPos);
};
