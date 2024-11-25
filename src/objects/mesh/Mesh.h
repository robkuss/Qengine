#pragma once

#include <map>
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
	std::vector<Vertex>	vertices = {};
	std::vector<int> faceIndices = {};

	// Edge-to-face adjacency information
	std::map<Edge, std::vector<Triangle>> edgeToFaceMap;

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

	static Vector3 faceNormal(const Triangle& t) ;
	static bool isSilhouetteEdge(const std::vector<Triangle>& triangles, Vector3 camPos) ;

private:
	void addEdgeToMap(const Edge &edge, const Triangle &t);
};
