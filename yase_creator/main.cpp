// Include STD
#include <iostream>

#include "header.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_other.h"

#include "home.h"
#include "env_page.h"



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
	bool			 over = false;
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
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		GLFWwindow*	window = glfwCreateWindow(1600, 900, "TP Ville Procedurale", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			std::cerr << "Erreur creation de la fenetre glfw" << std::endl;
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // vsync

#ifndef EMSCRIPTEN
		int v = glewInit();
#endif

		float z = 1.0f;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(SHADER_VERSION);

		// Setup Style
		ImGui::StyleColorsDark();


		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef EMSCRIPTEN
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
		glClearColor(0.6f, 0.5f, 0.05f, 1.0f);

		this->window = window;
	}

	void mainLoop()
	{
		static bool home_page_over = false;
		static bool env_page_over = false;

		if(glfwWindowShouldClose(window))
		{
			over = true;
			printf("Window shot close bye bye\n");
			END_PROGRAM(EXIT_SUCCESS);
		}
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


	void shutdown()
	{
		glfwTerminate();
	}
	
};
YASECreator c;

void run_main()
{
	try {
		c.mainLoop();
	}
	catch (std::exception& e) {
		printf("EX: %s\n", e.what());
	}
}

void run_main_loop()
{
	while(!c.over)
	{
		c.mainLoop();
	}
	printf("End of main loop\n");
}


int main()
{
/*
	constexpr size_t flags = yas::file | yas::binary;
	//ofstream of("env.yase", ios::in | ios::binary);
	yas::file_ostream os("env.yase", ios::in | ios::binary);
	YASE::DEF::Texture t{ "lol.png", "default", 1080, 720, 3 };
	yas::save<flags>(os,t);
	os.flush();

	if (!fs::exists("env.yase")) {
		printf("Faillll\n");
	}

	yas::file_istream ifs("env.yase");
	YASE::DEF::Texture t2;
	yas::load<flags>(ifs, t2);
*/	





	c.init();
#ifdef EMSCRIPTEN
	emscripten_set_main_loop(run_main, 0,1);
#else
	run_main_loop();
	c.shutdown();
#endif
	return 0;
}
