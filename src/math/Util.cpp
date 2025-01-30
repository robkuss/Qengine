#include "Util.h"

#include <graphics/ui/UIOption.h>

#include "math/matrix/Matrix4.h"

Vector2 project(const Vector3& worldPoint, const array<int, 4>* viewport, const array<float, 16>& viewMatrix, const array<float, 16>& projMatrix) {
	// TODO Make this better
	auto clip = Vector4(
       viewMatrix[0] * worldPoint.x + viewMatrix[4] * worldPoint.y + viewMatrix[8] * worldPoint.z + viewMatrix[12],
       viewMatrix[1] * worldPoint.x + viewMatrix[5] * worldPoint.y + viewMatrix[9] * worldPoint.z + viewMatrix[13],
       -(viewMatrix[2] * worldPoint.x + viewMatrix[6] * worldPoint.y + viewMatrix[10] * worldPoint.z + viewMatrix[14]),
       viewMatrix[3] * worldPoint.x + viewMatrix[7] * worldPoint.y + viewMatrix[11] * worldPoint.z + viewMatrix[15]
	);

	// Step 2: Apply the projection matrix (this transforms from camera space to clip space)
	clip.x = projMatrix[0] * clip.x + projMatrix[4] * clip.y + projMatrix[8]  * clip.z + projMatrix[12] * clip.w;
	clip.y = projMatrix[1] * clip.x + projMatrix[5] * clip.y + projMatrix[9]  * clip.z + projMatrix[13] * clip.w;
	clip.z = projMatrix[2] * clip.x + projMatrix[6] * clip.y + projMatrix[10] * clip.z + projMatrix[14] * clip.w;
	clip.w = projMatrix[3] * clip.x + projMatrix[7] * clip.y + projMatrix[11] * clip.z + projMatrix[15] * clip.w;

	if (abs(clip.w) < EPSILON) throw runtime_error("Cannot project point: w component is zero.");
	const float ndcX = clip.x / clip.w;
	const float ndcY = clip.y / clip.w;

	float screenX = (ndcX * 0.5f + 0.5f) * static_cast<float>((*viewport)[2]);
	float screenY = (1.0f - (ndcY * 0.5f + 0.5f)) * static_cast<float>((*viewport)[3]);

	return {screenX, screenY};
}

Vector3 unproject(const Vector2& screenPoint, const array<int, 4>* viewport, const array<float, 16>& viewMatrix, const array<float, 16>& projMatrix) {
	// Convert mouse coordinates to normalized device coordinates (NDC)
	const auto x = static_cast<float>(2.0 * screenPoint.x / (*viewport)[2] - 1.0);
	const auto y = static_cast<float>(1.0 - 2.0 * screenPoint.y / (*viewport)[3]);

	const auto viewSpace = Vector4(x, y, 1.0f, 1.0f);									// Create a vector in clip space
	const auto clipSpace = Matrix4(projMatrix).invert() * viewSpace;					// Transform from clip space to view space by applying the inverse of the projection matrix
	const auto unprojectedClipSpace = Vector4(clipSpace.x, clipSpace.y, -1.0f, 0.0f);	// Set the Z to -1 for proper unprojection and W to 0 for direction vector in the case of a ray
	const auto worldSpace = Matrix4(viewMatrix).invert() * unprojectedClipSpace;		// Transform from clip space to world space by applying the inverse of the view matrix

	return {worldSpace.x, worldSpace.y, worldSpace.z};
}
