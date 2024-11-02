#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "../../math/vector/Vector3.cpp"
#include "../../objects/mesh/Mesh.cpp"

class MeshRenderer {
public:
	static void render(Mesh mesh, Vector3 camPos, bool isSelected, bool isEditMode);

private:
	static bool isSilhouetteEdge(const Mesh& mesh, const std::vector<int>& faces, Vector3 camPos);
};

#endif // MESH_RENDERER_H
