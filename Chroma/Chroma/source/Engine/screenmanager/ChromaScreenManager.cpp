#include "ChromaScreenManager.h"

bool ChromaScreenManager::Initialize()
{
	// Configure Window
	if (!configureWindow())
	{
		std::cout << "Failed to Initialize Chroma Window: " << std::endl;
		return false;
	}

	// Configure Renderer
	if (!configureRenderer())
	{
		std::cout << "Failed to Initialize Chroma Renderer: " << std::endl;
		glfwTerminate();
		return false;
	}

	// Attach GUI
	if (!configureGui())
	{
		std::cout << "Failed to Initialize Chroma GUI: " << std::endl;
		glfwTerminate();
		return false;
	}
}

bool ChromaScreenManager::configureWindow()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CHROMA", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	return true;
}

bool ChromaScreenManager::configureGui()
{
	gui.attachWindow(*window);
	return true;
}

bool ChromaScreenManager::configureRenderer()
{
	// OpenGL 3
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Enabling Render Features
	// ---------------------------------------
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// Enable Face Culling
	glEnable(GL_CULL_FACE);
	// Enabling MSAA
	glEnable(GL_MULTISAMPLE);
	// Setting Clear Color
	glClearColor(SCREEN_DEFAULT_COLOR.x, SCREEN_DEFAULT_COLOR.y, SCREEN_DEFAULT_COLOR.z, SCREEN_DEFAULT_COLOR.w);
	return true;
}


// CHROMA SCREEN MANAGER LOOP
// --------------------

void ChromaScreenManager::StartLoop()
{
	// process
	processTime();
	// start gui
	gui.StartLoop();
}

void ChromaScreenManager::EndLoop()
{
	
	//draw GUI
	drawGUI();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void ChromaScreenManager::Close()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}


// Convenience Methods
// --------------------

void ChromaScreenManager::ToggleSkybox()
{
	useSkybox = useSkybox ? false : true;
}

void ChromaScreenManager::ToggleBloom()
{
	useBloom = useBloom ? false : true;
}

void ChromaScreenManager::ToggleDebug()
{
	drawDebug = drawDebug ? false : true;
}

void ChromaScreenManager::TogglePhysicsDebug()
{
	drawPhysicsDebug = drawPhysicsDebug ? false : true;
}

void ChromaScreenManager::ToggleSkeletonsDebug()
{
	drawSkeletonsDebug = drawSkeletonsDebug ? false : true;
}

void ChromaScreenManager::ToggleGraphicsDebug()
{
	drawGraphicsDebug = drawGraphicsDebug ? false : true;
}


void ChromaScreenManager::updateRendererViewportDimensions(int width, int height)
{
	// make sure the viewport matches the new window dimensions;  
	glViewport(0, 0, width, height);
}


void ChromaScreenManager::processTime()
{
	float GameTime = glfwGetTime();
	delta = GameTime - lastFrame;
	lastFrame = GameTime;
}


void ChromaScreenManager::drawGUI()
{
	// Time
	ImGui::Text("Deltatime %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	//// exposure
	//ImGui::SliderFloat("Exposure", &exposure, 0.0f, 2.0f);
	//ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);

	// bloom
	if (ImGui::Button("Toggle Bloom"))
		ToggleBloom();

	// debug draw
	//if (ImGui::Button("Toggle Debug"))
	//	ToggleDebug();

	//// debug draw
	if (ImGui::Button("Toggle Physics Debug"))
		TogglePhysicsDebug();

	if (ImGui::Button("Toggle Skeletons Debug"))
		ToggleSkeletonsDebug();

	if (ImGui::Button("Toggle Graphics Debug"))
		ToggleGraphicsDebug();

	// Graphics Debug Dropdowns 
	ImGui::Combo("Graphics Debug", &graphicsDebugSelected, GraphicsDebugs, IM_ARRAYSIZE(GraphicsDebugs));

	// camera dropdowns
	ImGui::Combo("Cameras", &cameraSelected, cameras, IM_ARRAYSIZE(cameras));

	ImGui::Text("Selected Entity : %s", SelectedEntity.c_str());
	gui.End();
}

unsigned int ChromaScreenManager::getScreenWidth()
{
	glfwGetWindowSize(window, &width, &height);
	return width;
}

unsigned int ChromaScreenManager::getScreenHeight()
{
	glfwGetWindowSize(window, &width, &height);
	return height;
}

std::pair<int, int> ChromaScreenManager::getWidthHeight()
{
	return std::make_pair(width, height);
}



// glfw callbacks
// --------------------
void ChromaScreenManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	updateRendererViewportDimensions(width, height);
}




// structors
// --------------------
ChromaScreenManager::ChromaScreenManager()
{
	Initialize();
}

ChromaScreenManager::~ChromaScreenManager()
{
}
