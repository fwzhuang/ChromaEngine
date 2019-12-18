#ifndef _CHROMA_JOINT_
#define _CHROMA_JOINT_

// stl
#include <string>
#include <vector>
#include <iostream>

// thirdparty
#include <glm/glm.hpp>

// chroma
#include <math/ChromaMath.h>

class Joint
{
	// Joint Data
	int m_ID{0};
	std::string m_Name;
	glm::mat4 m_LocalBindTransform{ 1.0 };
	glm::mat4 m_ModelBindTransform{ 1.0 };
	glm::mat4 m_ModelInverseBindTransform{ 1.0 };

	glm::mat4 m_AnimatedTransform{ 1.0 };
	std::vector<int> m_ChildJointIDs;
	int m_ParentJointID{ 0 };

public:
	// Accessors
	void SetID(int JointID) { m_ID = JointID; };
	void SetName(std::string Name) { m_Name = Name; };
	void SetModelBindTransform(glm::mat4 newModelBindTransform) { m_ModelBindTransform = newModelBindTransform; };
	void SetLocalBindTransform(glm::mat4 newOffsetMatrix) { m_LocalBindTransform = newOffsetMatrix; };
	void SetAnimatedTransform(glm::mat4 newAnimatedTransform) { m_AnimatedTransform = newAnimatedTransform; };
	void SetModelInverseBindTransform(glm::mat4 newInverseModelBindTransform) { m_ModelInverseBindTransform = newInverseModelBindTransform; };

	void AddChildJointID(int const&  newChildJoint) { m_ChildJointIDs.push_back(newChildJoint); };
	void SetChildJointIDs(std::vector<int> const& newChildJoints) { m_ChildJointIDs = newChildJoints; };
	void SetParentJointID(int const& newParentJoint) { m_ParentJointID = newParentJoint; };

	int GetID() const { return m_ID; };
	std::string GetName() const { return m_Name; };
	glm::mat4 GetModelBindTransform() const { return m_ModelBindTransform; };
	glm::mat4 GetModelInverseBindTransform() const { return m_ModelInverseBindTransform; };
	glm::mat4 GetLocalBindTransform() const { return m_LocalBindTransform; };
	glm::mat4 GetAnimatedTransform() const { return m_AnimatedTransform; };
	std::vector<int> GetChildJointIDs() const { return m_ChildJointIDs; };
	
	
	Joint();
	~Joint();
};

#endif