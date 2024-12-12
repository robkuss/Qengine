#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "objects/Object.h"
#include "scene/Mode.h"

#include "math/geometry/Vertex.h"
#include "math/geometry/Edge.h"
#include "math/geometry/Triangle.h"

#include <scene/graphics/color/Colors.h>


enum class ShadingMode {
	FLAT,
	SMOOTH
};

class Mesh : public Object {
public:
	std::vector<std::shared_ptr<Vertex>> vertices = {};
	std::vector<std::shared_ptr<Triangle>> triangles = {};
	std::vector<int> faceIndices = {};
	Color color;

	// Constructor & Destructor
	Mesh(const std::string& name, const Color& color) : Object{name}, color(color) {}
	~Mesh() override = default;

	void buildEdgeToFaceMap();
	void buildVertexToEdgeMap();

	void applyTransformation(const Mode& selectionMode, const Mode& transformMode, const Matrix4 &transformation) override;

	void initializeTriangles();
	void updateNormals() const;

private:
	friend class SceneManager;
	friend class MeshRenderer;

	// adjacency information
	std::unordered_map<Edge, std::vector<std::shared_ptr<Triangle>>> edgeToFaceMap;
	std::unordered_map<std::shared_ptr<Vertex>, std::vector<Edge>> vertexToEdgeMap;

	ShadingMode shadingMode = ShadingMode::FLAT;

	virtual void initializeVertices()    = 0;
	virtual void initializeFaceIndices() = 0;

	void addEdgeToMap(const Edge &e, const std::shared_ptr<Triangle> &t);

	void setColor(const Color &color);

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);

	void setShadingMode(ShadingMode shadingMode);
};
