// Include STD
#include <iostream>

#include "header.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_other.h"
#include "home.h"
#include "env_page.h"

/*
 *	Truc a finir asap
 *	
 *	1 - Finir mon model manager. Avec le convetissement de .obj et l'ajout de leur texture dans le texture manager. Ma commencer avec mon cube blender
 *  2 - Faire le scene manager. Avec la sauvegarde de l'environnement et comment je vais representer mes donners.
 *
 */

using namespace std;

static void glfw_error_callback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << " : " << description << std::endl;
}

enum YASECreator_state
{
	HOME_PAGE,
	CREATOR
};

class YASECreator
{
	GLFWwindow*	window;

	EnvManager environment_loader;

	HomeWindow home_window;
	EnvironmentWindow env_window;

	YASECreator_state state;
public:
	YASECreator() : home_window(&environment_loader),
		env_window(&environment_loader),
		state(HOME_PAGE),
		window(0) {}

	void init()
	{
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
		{
			std::cerr << "Erreur initialisation glfw" << std::endl;
			return;
		}

		GLFWwindow*	window = glfwCreateWindow(1600, 900, "TP Ville Procedurale", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			std::cerr << "Erreur creation de la fenetre glfw" << std::endl;
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // vsync

		int v = glewInit();

		float z = 1.0f;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 430");

		// Setup Style
		ImGui::StyleColorsDark();


		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		this->window = window;
	}

	void mainLoop()
	{
		static bool home_page_over = false;
		static bool env_page_over = false;

		while (!glfwWindowShouldClose(window))
		{
			// Regenere mes nouvelles frame de ImGui
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Ma section de rendering


			switch (state)
			{
			case HOME_PAGE:
				home_window.Draw(&home_page_over,&home_page_over);
				if(home_page_over)
				{
					state = CREATOR;
				}

				break;
			case CREATOR:
				env_window.Draw(&env_page_over);
				if (env_page_over) {

				}
				break;
			}


			// Rendering de ImGui par dessus mon stock
			ImGui::Render();
			int display_w, display_h;
			glfwMakeContextCurrent(window);
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			glfwMakeContextCurrent(window);

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

	}




	void shutdown()
	{
		glfwTerminate();
	}



	
};


int main()
{
	YASECreator c;
	c.init();
	c.mainLoop();
	c.shutdown();
	return 0;
}
