#pragma once

#include <vector>

#include <objects/mesh/Mesh.h>

#include "color/Color.h"

struct RenderContext;
class Vector3;
struct Vertex;
struct Triangle;


class MeshRenderer {
public:
	static void render(const Mesh &mesh, const RenderContext& context);

private:
	static void renderVertex(const std::shared_ptr<Vertex>& v);
	static void renderEdge(const Edge &e, const Color &firstColor, const Color &secondColor);
	static void renderTriangle(const Mesh &mesh, const Triangle &t, bool isSelected);

	static void renderVertices(const Mesh &mesh, const RenderContext &context);
	static void renderEdges(const Mesh &mesh, const RenderContext &context);
	static void renderTriangles(const Mesh &mesh, const RenderContext &context);

	static bool isSilhouetteEdge(const std::pair<Edge, std::vector<Triangle>> &edgeEntry, const RenderContext &context);
};
