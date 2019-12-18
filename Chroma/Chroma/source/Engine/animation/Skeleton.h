#ifndef _CHROMA_SKELETON_
#define _CHROMA_SKELETON_



// stl
#include <vector>
#include <map>

// thirdparty
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// chroma
#include <animation/Joint.h>
#include <buffer/DebugBuffer.h>
#include <component/ChromaMeshComponent.h>


class Skeleton
{
	ChromaMeshComponent* m_ParentComponent;

	glm::mat4 m_IdentityMatrix{ 1.0 };
	glm::mat4 m_WorldTransform{ 1.0 };
	glm::mat4 m_WorldTransformInverse{ 1.0 };
	int m_RootJointID{ 0 };
	std::map<std::pair<int, std::string>, Joint> m_Joints;

	float m_Scale{ 1.0f };
	glm::vec3 m_Translation{ 1.0f };
	glm::quat m_Rotation{ glm::quat() };

	// Functions
	void ProcessJointLocalBindOffsetTransforms(int const& jointID, glm::mat4 const& parentTransform);
	void DebugWalkChildJoints(Joint const& currentJoint, DebugBuffer* const& debugBuffer);
	void TransformJointAndChildren(int const& jointID, glm::mat4 const& transform);
	void UpdateSkeletonRootTransform();

public:
	// Accessors
	void AddJoint(Joint& newJoint);
	void SetGlobalTransform(glm::mat4 const& newGlobalTransform) ;
	void SetRootJoint(Joint& newRootJoint) { m_RootJointID = newRootJoint.GetID(); };
	void SetRootJointID(int const& newRootJointID) { m_RootJointID = newRootJointID; };

	void SetSkeletonScale(float const& newScale) { m_Scale = newScale; UpdateSkeletonRootTransform(); };
	void SetSkeletonTranslation(glm::vec3 const& newTranslation) { m_Translation = newTranslation; UpdateSkeletonRootTransform(); };
	void SetSkeletonRotation(glm::quat const& newRotation) { m_Rotation = newRotation; UpdateSkeletonRootTransform(); };

	int GetNumJoints() const { return m_Joints.size(); };
	std::map<std::pair<int, std::string>, Joint> GetIndexedNamedJoints() const { return m_Joints; };
	std::map<std::string, Joint*> GetNamedJoints();
	std::map<int, Joint*> GetIndexedJoints();
	std::vector<Joint*> GetJoints();

	int GetJointID(std::string const& jointName) const;
	std::string GetJointName(int const& jointID) const;

	Joint GetJoint(int const& index);
	Joint* GetJointPtr(int const& index);
	Joint GetJoint(std::string const& jointName);
	Joint* GetJointPtr(std::string const& jointName);
	Joint GetRootJoint() { return GetJoint(m_RootJointID); };

	bool GetJointExists(int const& index) const;
	bool GetJointExists(std::string const& jointName) const;

	// Functions
	void BindParentComponent(ChromaMeshComponent* const& newMeshComponent) { m_ParentComponent = newMeshComponent; };
	void SetJointUniforms(Shader& skinnedShader);
	void CalculateJointLocalBindOffsetTransforms();
	void DebugDraw(DebugBuffer* debugBuffer);
	void InitializeSkeleton();

	Skeleton();
	~Skeleton();
};

#endif