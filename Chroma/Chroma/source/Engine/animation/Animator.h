#ifndef _CHROMA_ANIMATOR_H_
#define _CHROMA_ANIMATOR_H_

// stl
#include <vector>
// thirdparty
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// chroma
#include <animation/AnimationLoader.h>
#include <animation/Skeleton.h>
#include <time/Time.h>
#include <worker/IWorker.h>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>


typedef std::map<std::string, KeyFrame> KeyFrames;

class Animator
{
	// skeleton
	Skeleton* m_Skeleton{ nullptr };

	// takes
	std::map<std::string, Take> m_Takes;
	std::string m_CurrentTake;
	void AddTake(Take const& newTake);

	// animation
	void PlayTake(std::string const& takeName, float const& normalizedTime);

	// time
	float CalculateFrameNumber(std::string const& takeName, float const& normalizedTime);

	// joint transforms
	void ApplyAnimJointHierarchy(int const& jointID, KeyFrames& keyFrames, glm::mat4& parentTransform , float const& timeStamp);

	glm::mat4 GetJointMat4AtKeyFrameTime(std::string const& jointName, KeyFrames& keyFrames, float timeStamp);
	JointTransform GetJointTransformAtKeyFrameTime(KeyFrame& keyFrame, float const& timeStamp);
	JointTransform InterpolateJointTransforms(JointTransform const& from, JointTransform const& to, float const& lerp);
	glm::mat4 JointTransformToMat4(JointTransform const& jointTransform);

	void testOrientation(int const& jointID);

public:
	void LoadAnimations(std::string const& sourcePath);

	void BindSkeleton(Skeleton* newSkeleton) { m_Skeleton = newSkeleton; };

	void DoAnimation();

	void DebugAnimationTake(std::string const& takeName, float const& debugTime);

	Animator();
	~Animator();
};

#endif 