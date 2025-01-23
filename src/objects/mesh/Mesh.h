#pragma once

#include <unordered_map>
#include <memory>

#include "objects/Object.h"
#include "math/geometry/Edge.h"
#include "math/geometry/Triangle.h"

#include <graphics/color/Colors.h>


class Texture;

enum class ShadingMode {
	FLAT,
	SMOOTH
};

class Mesh : public Object {
public:
	std::vector<std::shared_ptr<Vertex>> vertices		= {};
	std::vector<std::shared_ptr<Triangle>> triangles	= {};
	std::vector<int> faceIndices						= {};

	std::shared_ptr<Texture> texture					= nullptr;
	ShadingMode shadingMode								= ShadingMode::FLAT;
	Color color;

	// Constructor & Destructor
	Mesh(const std::string& name, const Color& color, const std::shared_ptr<Texture>& texture) : Object{name}, texture(texture), color(color) {}
	~Mesh() override = default;

	void buildEdgeToFaceMap();
	void buildVertexToEdgeMap();

	void applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation) override;

	void initializeTriangles();
	void updateNormals() const;

	void setShadingMode(ShadingMode shadingMode);

private:
	friend class MeshRenderer;

	// adjacency information
	std::unordered_map<Edge, std::vector<std::shared_ptr<Triangle>>> edgeToFaceMap;
	std::unordered_map<std::shared_ptr<Vertex>, std::vector<Edge>> vertexToEdgeMap;

	virtual void initializeVertices()    = 0;
	virtual void initializeFaceIndices() = 0;

	void addEdgeToMap(const Edge &e, const std::shared_ptr<Triangle> &t);

	void setColor(const Color &color);

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);
};
