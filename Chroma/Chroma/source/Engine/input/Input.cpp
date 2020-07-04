#include "Input.h"
#include <scene/Scene.h>
#include <physics/PhysicsEngine.h>
#include <UI/core/UI.h>
#ifdef EDITOR
#include <editor/ui/EditorUI.h>
#endif

#include <core/Application.h>


namespace Chroma
{
	// Controller
	bool Input::m_ControllerEnabled;

	int Input::m_ControllerAxesCount;
	int Input::m_ControllerButtonsCount;
	const unsigned char* Input::m_ControllerButtonsMapping;

	float Input::m_ControllerRightVertical;
	float Input::m_ControllerRightHorizontal;

	float Input::m_ControllerLeftVertical;
	float Input::m_ControllerLeftHorizontal;

	float Input::m_ControllerLeftBumper;
	float Input::m_ControllerRightBumper;


	float Input::m_LastMouseX, Input::m_LastMouseY;
	float Input::m_CurrentMouseX, Input::m_CurrentMouseY;

	// Mouse
	glm::vec3 Input::m_LastMouseRay;


	void Input::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(CHROMA_BIND_EVENT_STATIC_FN(Input::OnMousePressed));
		dispatcher.Dispatch<MouseMovedEvent>(CHROMA_BIND_EVENT_STATIC_FN(Input::OnMouseMoved));
	}

	glm::vec2 Input::GetMouseCoordinates()
	{
		return Application::Get().GetWindow().GetCursorCoordinates();
	}

	void Input::ToggleCursorEnabled()
	{
		Application::Get().GetWindow().ToggleCursorEnabled();
	}

	void Input::SetCursorEnabled(const bool& enabledState)
	{
		Application::Get().GetWindow().SetCursorEnabled(enabledState);
	}

	bool Input::OnMousePressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == MouseCode::ButtonLeft)
		{
			m_LastMouseRay = Math::ScreenToWorldRay(Application::Get().GetWindow().GetCursorCoordinates());
			m_MousePickerCallback();
		}
		return true;

	}

	bool Input::OnMouseMoved(MouseMovedEvent& e)
	{
		m_LastMouseX = m_CurrentMouseX;
		m_LastMouseY = m_CurrentMouseY;

		m_CurrentMouseX = e.GetX();
		m_CurrentMouseY = e.GetY();

		return true;
	}

	void Input::m_MousePickerCallback()
	{
		// Ray Interest Test
		glm::vec3 start = Chroma::Scene::GetRenderCamera()->GetPosition();
		glm::vec3 end = start + (Chroma::Input::GetLastRay() * glm::vec3(1000.0));
		IEntity* clickedEntity = Chroma::Physics::GetEntityRayTest(start, end);
		if (clickedEntity)
		{
			Chroma::UI::SetSelectedEntityName(clickedEntity->GetName());
		}
#ifdef EDITOR
		if (Chroma::EditorUI::GetIsMouseOverViewport() && clickedEntity)
		{
			Chroma::EditorUI::SetSelectedObjectUID(clickedEntity->GetUID());
		}
#endif
	}


	void Input::UpdateController()
	{
		m_ControllerEnabled = (glfwJoystickPresent(GLFW_JOYSTICK_1) > 0) ? true : false;

		if (m_ControllerEnabled)
		{
			// Axes
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &m_ControllerAxesCount);
			for (int i = 0; i < m_ControllerAxesCount; i++)
			{
				// Sticks
				m_ControllerRightVertical = axes[RV_AXIS];
				m_ControllerRightHorizontal = axes[RH_AXIS];

				m_ControllerLeftVertical = axes[LV_AXIS];
				m_ControllerLeftHorizontal = axes[LH_AXIS];

				// Bumpers
				m_ControllerLeftBumper = axes[L2_AXIS];
				m_ControllerRightBumper = axes[R2_AXIS];
			}
			// Buttons
			m_ControllerButtonsMapping = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &m_ControllerButtonsCount);

			// DEBUGGING
			//// BUTTONS
			//for (int i = 0; i < m_ControllerButtonsCount; i++)
			//{
			//	if (m_ControllerButtonsMapping[i] == GLFW_PRESS)
			//	{
			//		CHROMA_INFO("Index : {0}", i);
			//	}
			//}
			//AXIS
			//for (int i = 0; i < m_ControllerAxesCount; i++)
			//{
			//	CHROMA_INFO("Axis Index : {0} , Weight : {1}", i, axes[i]);
			//}
		}
		else
		{
			//CHROMA_WARN("INPUT :: No Controller Connected");
		}
	}


	float Input::GetAxis(const char* axis)
	{
		if (axis == "Horizontal")
		{
			if (m_ControllerEnabled)
			{
				return m_ControllerLeftHorizontal;
			}
		}
		else if (axis == "Vertical")
		{
			if (m_ControllerEnabled)
			{
				return m_ControllerLeftVertical;
			}
		}
		else
		{
			CHROMA_ERROR("CHROMA INPUT :: Invalid Axis {}", axis);
			throw(std::invalid_argument("Invalid Axis"));
		}
	
	}

}
