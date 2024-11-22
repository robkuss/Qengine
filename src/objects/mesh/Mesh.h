#ifndef MESH_H
#define MESH_H

#include <map>
#include <string>
#include <vector>
#include <utility>  // For std::pair

#include "objects/Object.h"
#include "scene/Mode.h"
#include "math/Triangle.h"
#include "math/matrix/Matrix4.h"

class Mesh : public Object {
public:
	std::vector<Vector3> vertices;
	std::vector<int> faceIndices;
	std::vector<int> edgeIndices;

	// Edge-to-face adjacency information
	std::map<std::pair<int, int>, std::vector<int>> edgeToFaceMap;

	// Constructor & Destructor
	Mesh(const std::string& name, const Vector3& position, const Vector3& scale, const Vector3& rotation)
		: Object{name, position, scale, rotation} {}
	~Mesh() override = default;

	void applyTransformation(Mode::ModeEnum mode, const Matrix4& transformation);
	std::vector<Triangle> getTriangles();
	void buildEdgeToFaceMap();
	[[nodiscard]] Vector3 faceNormal(int faceIndex) const;

private:
	void addEdgeToMap(int v0, int v1, int faceIndex);
};

#endif // MESH_H
