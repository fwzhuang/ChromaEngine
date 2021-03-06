#ifndef _CHROMA_THIRDPERSON_CAMERA_CONTROLLER_H_
#define _CHROMA_THIRDPERSON_CAMERA_CONTROLLER_H_


// chroma
#include <camera/ICameraController.h>

namespace Chroma
{
	class ThirdPersonCameraController : public ICameraController
	{
	public:
		void OnEvent(Event& e, glm::vec3& camPos, glm::vec3& camDir, glm::vec3& camUp) override;


		ThirdPersonCameraController() {};
		~ThirdPersonCameraController() {};

	};
}

#endif
