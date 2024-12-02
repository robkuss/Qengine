#pragma once

#include <string>
#include <map>
#include <vector>

#include "objects/Object.h"
#include "scene/Mode.h"
#include "math/geometry/Vertex.h"
#include "math/geometry/Triangle.h"

class Mesh : public Object {
public:
	std::vector<Vertex> vertices = {};
	std::vector<int> faceIndices = {};

	// Edge-to-face adjacency information
	std::map<std::pair<int, int>, std::vector<Triangle>> edgeToFaceMap;

	// Constructor & Destructor
	Mesh(const std::string& name, const Matrix4& position, const Matrix4& scale, const Matrix4& rotation)
		: Object{name, position, scale, rotation} {}
	~Mesh() override = default;

	virtual void initializeVertices()	 = 0;
	virtual void initializeFaceIndices() = 0;

	[[nodiscard]] Triangle getTriangle(int index) const;
	[[nodiscard]] std::vector<Triangle> getTriangles() const;

	void buildEdgeToFaceMap();

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);

	void applyTransformation(Mode::ModeEnum mode, const Matrix4& transformation);

private:
	void addEdgeToMap(int v0, int v1, const Triangle& t);
};
