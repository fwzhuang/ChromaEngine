#include "ChromaScreenManager.h"

int ChromaScreenManager::initialize()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CHROMA", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// glfw attach callbacks
	// --------------------
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_aim_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);

	// capture mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// configure Renderer
	if (configureRenderer() == -1)
	{
		std::cout << "Failed to Initialize Chroma Renderer: " << std::endl;
		glfwTerminate();
		return -1;
	}

	// attach gui
	if (configureGui() == -1)
	{
		std::cout << "Failed to Initialize Chroma GUI: " << std::endl;
		glfwTerminate();
		return -1;
	}
}

int ChromaScreenManager::configureGui()
{
	gui.attachWindow(*window);
	return 1;
}

int ChromaScreenManager::configureRenderer()
{
	// OpenGL 3
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Enabling Features
	// ---------------------------------------
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	// Blending Func
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Face Culling
	glEnable(GL_CULL_FACE);
}

// CHROMA SCREEN MANAGER LOOP
// --------------------

void ChromaScreenManager::Start()
{
	processTime();
	processInput();
}

void ChromaScreenManager::End()
{
	// draw GUI
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
void ChromaScreenManager::updateRendererViewportDimensions(int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
}

void ChromaScreenManager::updateCamera()
{
	camera.processKeyboardInput(*window, deltaTime);
	camera.processMouseInput(MOUSE_XOFFSET, MOUSE_YOFFSET);
}

void ChromaScreenManager::processTime()
{
	float GameTime = glfwGetTime();
	deltaTime = GameTime - lastFrame;
	lastFrame = GameTime;
}

void ChromaScreenManager::drawScene()
{
	std::cout << "Chroma Screen Manager Drawing" << std::endl;
}

void ChromaScreenManager::drawGUI()
{
	gui.draw();
}


// glfw callbacks
// --------------------
void ChromaScreenManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	updateRendererViewportDimensions(width, height);
}

void ChromaScreenManager::mouse_aim_callback(GLFWwindow* window, double xpos, double ypos)
{
	GLint cursorMode = glfwGetInputMode(window, GLFW_CURSOR);
	if (cursorMode == GLFW_CURSOR_DISABLED)
	{
		MOUSE_XOFFSET = xpos;
		MOUSE_YOFFSET = ypos;
	}
}

void ChromaScreenManager::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	return;
}


// processInput
// --------------------
void ChromaScreenManager::processInput()
{
	// check if should close on this frame
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	// window capture release mouse
	GLint cursorMode = glfwGetInputMode(window, GLFW_CURSOR);
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (cursorMode == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camera.firstMouse = true;
		}
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	// if mouse captured update camera
	if (cursorMode == GLFW_CURSOR_DISABLED)
		updateCamera();
}


// structors
// --------------------
ChromaScreenManager::ChromaScreenManager()
{
	initialize();
}

ChromaScreenManager::~ChromaScreenManager()
{
}