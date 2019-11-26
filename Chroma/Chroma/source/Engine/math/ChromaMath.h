#ifndef _CHROMA_MATH_
#define _CHROMA_MATH_

#include <bullet/btBulletCollisionCommon.h>
#include <glm/glm.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

static glm::mat4 BulletToGLM(const btTransform& transform)
{
	glm::mat4 glmTransform;
	transform.getOpenGLMatrix(glm::value_ptr(glmTransform));
	return glmTransform;
}


static glm::mat4 BulletToGLM(btTransform& transform)
{
	glm::mat4 glmTransform;
	transform.getOpenGLMatrix(glm::value_ptr(glmTransform));
	return glmTransform;
}

static glm::vec3 BulletToGLM(const btVector3& vector3)
{
	return glm::vec3(vector3[0], vector3[1], vector3[2]);
}

static btTransform GLMToBullet(const glm::mat4& mat4)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat4, scale, rotation, translation, skew, perspective);

	btQuaternion orient(rotation.x, rotation.y, rotation.z, rotation.w);
	btVector3 translate(translation.x, translation.y, translation.z);
	return btTransform(orient, translate);
}

static btVector3 GLMToBullet(const glm::vec3& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}


#endif