#pragma once

#include <vector>

#include <objects/mesh/Mesh.h>


class MeshRenderer {
public:
	static void render(const Mesh &mesh, const Mode &selectionMode, bool isMeshSelected);
	static void renderSilhouette(const Mesh &mesh, const Mode &selectionMode, const Vector3 &camPos, bool isMeshSelected);

private:
	static void renderVertex(const Vertex& v);
	static void renderEdge(const Edge& e, const Color& c1, const Color& c2);

	static void renderVertices(const Mesh &mesh);
	static void renderEdges(const Mesh &mesh);
	static void renderTriangles(const Mesh &mesh);

	static bool isSilhouetteEdge(const vector<shared_ptr<Triangle>> &edgeAdjFaces, const Vector3 &camPos);

	static bool toggleDiffuse;
};
