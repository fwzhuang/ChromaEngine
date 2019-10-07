#include "ChromaScreenManager.h"

int ChromaScreenManager::initialize()
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

	// Enabling Render Features
	// ---------------------------------------
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	// Blending Func
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Face Culling
	glEnable(GL_CULL_FACE);
	// Enabling MSAA
	glEnable(GL_MULTISAMPLE);
	// Enabling Gamme Correction
	glEnable(GL_FRAMEBUFFER_SRGB);
}


// CHROMA SCREEN MANAGER LOOP
// --------------------

void ChromaScreenManager::Start()
{
	// process
	processTime();
	processInput();
	// start gui
	gui.Start();
	// render start
	Render();
	// Post FX
	if (usePostFX)
		framebuffer->bind();
}

void ChromaScreenManager::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ChromaScreenManager::End()
{
	// SkyBox
	if (useSkybox)
		skybox->Draw();

	//PostFX
	if (usePostFX)
		framebuffer->Draw();
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

void ChromaScreenManager::TogglePostFX()
{
	if (usePostFX)
	{
		usePostFX = false;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}
	else
		usePostFX = true;
}

void ChromaScreenManager::ToggleSkybox()
{
	if (useSkybox)
		useSkybox = false;
	else
		useSkybox = true;
}


// Convenience Methods
// --------------------
void ChromaScreenManager::updateRendererViewportDimensions(int width, int height)
{
	// make sure the viewport matches the new window dimensions;  
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


void ChromaScreenManager::drawGUI()
{
	gui.End();
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
	// opengl is now loaded we can instantiate framebuffer object
	framebuffer = new Framebuffer();
	skybox = new SkyBox("resources/textures/skybox/blueskywater", camera);
}

ChromaScreenManager::~ChromaScreenManager()
{
	delete framebuffer;
}