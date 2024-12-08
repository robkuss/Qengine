#pragma once

#include <string>
#include <map>
#include <vector>

#include "objects/Object.h"
#include "scene/Mode.h"

#include "math/geometry/Vertex.h"
#include "math/geometry/Edge.h"
#include "math/geometry/Triangle.h"


enum class ShadingMode {
	FLAT,
	SMOOTH
};

class Mesh : public Object {
public:
	std::vector<std::shared_ptr<Vertex>> vertices = {};
	std::vector<int> faceIndices = {};

	// Constructor & Destructor
	Mesh(const std::string& name, const Matrix4& position, const Matrix4& scale, const Matrix4& rotation)
		: Object{name, position, scale, rotation} {}
	~Mesh() override = default;

	void buildEdgeToFaceMap();
	void buildVertexToEdgeMap();

	[[nodiscard]] std::vector<Triangle> getTriangles() const;

private:
	friend class SceneManager;
	friend class MeshRenderer;

	// adjacency information
	std::map<Edge, std::vector<Triangle>> edgeToFaceMap;
	std::map<std::shared_ptr<Vertex>, std::vector<Edge>> vertexToEdgeMap;

	ShadingMode shadingMode = ShadingMode::FLAT;

	virtual void initializeVertices()	 = 0;
	virtual void initializeFaceIndices() = 0;

	static Vector3 faceNormal(const Triangle& t);
	[[nodiscard]] Vector3 vertexNormal(const std::shared_ptr<Vertex> &v) const;

	[[nodiscard]] Triangle getTriangle(int index) const;

	void addEdgeToMap(const Edge &e, const Triangle &t);

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);

	void applyTransformation(const Mode& mode, const Matrix4& transformation);

	void setShadingMode(ShadingMode shadingMode);
};
