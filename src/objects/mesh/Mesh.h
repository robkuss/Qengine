#pragma once

#include <string>
#include <map>
#include <vector>

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
	std::vector<int> faceIndices = {};
	Color color = Colors::MESH_FACE_COLOR;

	// Constructor & Destructor
	explicit Mesh(const std::string& name, const Color& color) : Object{name}, color(color) {}
	~Mesh() override = default;

	void buildEdgeToFaceMap();
	void buildVertexToEdgeMap();
	void initializeNormals() const;

	[[nodiscard]] std::vector<Triangle> getTriangles() const;

	void applyTransformation(const Mode &mode, const Matrix4 &transformation) override;

private:
	friend class SceneManager;
	friend class MeshRenderer;

	// adjacency information
	std::map<Edge, std::vector<Triangle>> edgeToFaceMap;
	std::map<std::shared_ptr<Vertex>, std::vector<Edge>> vertexToEdgeMap;

	ShadingMode shadingMode = ShadingMode::FLAT;

	virtual void initializeVertices()    = 0;
	virtual void initializeFaceIndices() = 0;

	[[nodiscard]] Triangle getTriangle(int index) const;

	void addEdgeToMap(const Edge &e, const Triangle &t);

	void setColor(const Color &color);

	void setPosition(const Vector3& translation);
	void setScale(const Vector3& scale);
	void setRotation(const Vector3& rotation);

	void updateVertexNormals() const;

	void setShadingMode(ShadingMode shadingMode);
};
