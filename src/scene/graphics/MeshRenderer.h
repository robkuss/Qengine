#pragma once

#include <utility>
#include <vector>

#include <objects/mesh/Mesh.h>

struct RenderContext;
class Vector3;
struct Vertex;
struct Triangle;


class MeshRenderer {
public:
	static void render(const Mesh &mesh, const RenderContext& context);

private:
	static void renderVertex(const std::shared_ptr<Vertex> &v);
	static void renderEdge(const Mesh &mesh, const std::pair<int, int> &e);
	static void renderTriangle(const Mesh &mesh, const Triangle &t);

	static void renderVertices(const Mesh &mesh, const RenderContext &context);
	static void renderEdges(const Mesh& mesh);
	static void renderTriangles(const Mesh &mesh);

	static bool isSilhouetteEdge(const std::vector<Triangle> &triangles, Vector3 camPos);
};
