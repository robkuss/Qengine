#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

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
	vector<shared_ptr<Vertex>> vertices		= {};
	vector<shared_ptr<Triangle>> triangles	= {};
	vector<int> faceIndices					= {};

	shared_ptr<Texture> texture				= nullptr;
	ShadingMode shadingMode					= ShadingMode::FLAT;
	Color color;

	// Constructor & Destructor
	Mesh(const string& name, const Color& color, const shared_ptr<Texture>& texture) : Object{name}, texture(texture), color(color) {}
	~Mesh() override = default;

	void buildEdgeToFaceMap();
	void buildVertexToEdgeMap();

	void applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation) override;

	void initializeTriangles();
	void updateNormals() const;

	void setShadingMode(ShadingMode shadingMode);
	void setMaterial(const Color &diffuse, const Color &specular, const Color &emission, const Color &ambient, float shininess);

protected:
	Color diffuse   = Colors::WHITE;
	Color specular  = Colors::WHITE;
	Color emission  = Colors::BLACK;
	Color ambient   = Colors::WHITE;
	float shininess = 30.0f;

private:
	friend class MeshRenderer;

	// adjacency information
	unordered_map<Edge, vector<shared_ptr<Triangle>>> edgeToFaceMap;
	unordered_map<shared_ptr<Vertex>, vector<Edge>> vertexToEdgeMap;

	virtual void initializeVertices()    = 0;
	virtual void initializeFaceIndices() = 0;

	void addEdgeToMap(const Edge &e, const shared_ptr<Triangle> &t);

	void setColor(const Color &color);

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);
};
