#pragma once

#include <vector>

#include <objects/mesh/Mesh.h>


class MeshRenderer {
public:
	static void render(const Mesh &mesh, const std::vector<Vertex> &selectedVertices, const Mode &selectionMode, bool isMeshSelected);
	static void renderSilhouette(const Mesh &mesh, const Mode &selectionMode, const Vector3 &camPos, bool isMeshSelected);

private:
	static void renderVertex(const Vertex& v);
	static void renderEdge(const Edge& e, const Color& firstColor, const Color& secondColor);
	// static void renderTriangle(const Mesh& mesh, const Triangle& t, bool isSelected);

	static void renderVertices(const Mesh &mesh, const std::vector<Vertex> &selectedVertices);
	static void renderEdges(const Mesh &mesh, const std::vector<Vertex> &selectedVertices);
	static void renderTriangles(const Mesh &mesh, const std::vector<Vertex> &selectedVertices);

	static bool isSilhouetteEdge(const std::vector<std::shared_ptr<Triangle>> &edgeAdjFaces, const Vector3 &camPos);

	static bool toggleDiffuse;
};
