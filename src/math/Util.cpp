#include "Util.h"

#include "math/matrix/Matrix4.h"

Vector2 project(const Vector3& worldPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix) {
	// Convert world coordinates to view space
	const Vector4 viewSpace = Matrix4(viewMatrix) * Vector4(worldPoint.x, worldPoint.y, worldPoint.z, 1.0f);
	const Vector4 clipSpace = Matrix4(projMatrix) * viewSpace;

	// Convert to normalized device coordinates (NDC)
	const Vector4 ndcSpace = clipSpace / clipSpace.w;

	// Convert from NDC to screen coordinates (mapping NDC [-1, 1] to [0, 1])
	const float x = (ndcSpace.x + 1.0f) * 0.5f * static_cast<float>(viewport[2]) + static_cast<float>(viewport[0]);
	const float y = (1.0f - ndcSpace.y) * 0.5f * static_cast<float>(viewport[3]) + static_cast<float>(viewport[1]);

	return {x, y};
}

Vector3 unproject(const Vector2& screenPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix) {
	// Convert mouse coordinates to normalized device coordinates (NDC)
	const auto x = static_cast<float>(2.0 * screenPoint.x / viewport[2] - 1.0);
	const auto y = static_cast<float>(1.0 - 2.0 * screenPoint.y / viewport[3]);

	const auto viewSpace = Vector4(x, y, 1.0f, 1.0f);									// Create a vector in clip space
	const auto clipSpace = Matrix4(projMatrix).invert() * viewSpace;					// Transform from clip space to view space by applying the inverse of the projection matrix
	const auto unprojectedClipSpace = Vector4(clipSpace.x, clipSpace.y, -1.0f, 0.0f);	// Set the Z to -1 for proper unprojection and W to 0 for direction vector in the case of a ray
	const auto worldSpace = Matrix4(viewMatrix).invert() * unprojectedClipSpace;		// Transform from clip space to world space by applying the inverse of the view matrix

	return {worldSpace.x, worldSpace.y, worldSpace.z};
}
