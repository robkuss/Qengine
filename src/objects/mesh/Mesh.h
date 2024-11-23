#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "objects/Object.h"
#include "scene/Mode.h"
#include "math/geometry/Vertex.h"
#include "math/geometry/Edge.h"
#include "math/geometry/Triangle.h"
#include "math/matrix/Matrix4.h"

class Mesh : public Object {
public:
	std::vector<Vertex> vertices = {};
	std::vector<int> edgeIndices = {};
	std::vector<int> faceIndices = {};

	// Edge-to-face adjacency information
	std::map<std::pair<int, int>, std::vector<int>> edgeToFaceMap;

	// Constructor & Destructor
	Mesh(const std::string& name, const Matrix4& position, const Matrix4& scale, const Matrix4& rotation)
		: Object{name, position, scale, rotation} {}
	~Mesh() override = default;

	virtual void initializeVertices()	 = 0;
	virtual void initializeFaceIndices() = 0;
	virtual void initializeEdges()		 = 0;

	void applyTransformation(Mode::ModeEnum mode, const Matrix4& transformation);
	[[nodiscard]] std::vector<Triangle> getTriangles() const;
	void buildEdgeToFaceMap();
	[[nodiscard]] Vector3 faceNormal(int faceIndex) const;

private:
	void addEdgeToMap(int v0, int v1, int faceIndex);
};
