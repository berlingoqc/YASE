// Include STD
#include <iostream>

#include "header.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_other.h"

#include "home.h"
#include "env_page.h"

#include <atomic>
#include <future>
#include <queue>
#include <sstream>
#include "picojson.h"
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
// for string delimiter
vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}
struct membuf : std::streambuf {
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

struct my_istream {
	my_istream(const char *ptr, const std::size_t size)
		:cur(ptr)
		, end(ptr + size)
	{}

	std::size_t read(void *ptr, const std::size_t size) {
		if (cur + size > end)
			return 0;

		std::memcpy(ptr, cur, size);
		cur += size;
		return size;
	}

	char peekch() const { return *cur; }
	char getch() { return *cur++; }
	void ungetch(char) { --cur; }

	const char *cur;
	const char *end;
};

#ifdef EMSCRIPTEN


enum YASEEm_state {
	FETCHING_CONFIG,
	PUSHING_CONFIG,
	PULL_LIST_TEXTURE,
	PULL_LIST_SKYBOX,
	PULL_LIST_LOADED_TEX,
	PULL_LIST_MODEL,
	OVER_FAIL,
	DOWLOAD_TEXTURE,
	DOWLOAD_SKYBOX,
	DOWNLOAD_MODEL
};

static string	em_state[6]{
	"Telechargement de la configuration",
	"Creation nouvelle configuration",
	"Reception des textures",
	"Reception des skybox",
	"Reception des models",
	"Erreur"
};


static std::atomic<bool> download_over = false;
static emscripten_fetch_t* get_fetch;

void downloadSucceeded(emscripten_fetch_t *fetch) {
	printf("Finished downloading %llu bytes from URL %s %d.\n", fetch->numBytes, fetch->url,fetch->status);
	get_fetch = fetch;
	download_over = true;
}

void downloadFailed(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
}
#endif
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
			
#ifdef EMSCRIPTEN
			static int current_step = FETCHING_CONFIG;
			static int is_start = -1;
			static string next_download = "/helios/api/yase/config";
			static queue<string> files_download;
			static string		last_file;
			static int			face_index;

			ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Chargement de l'environnement");

			ImGui::Text("Etape Actuelle :  %s",em_state[current_step].c_str());

			if (is_start == -1) {
				emscripten_fetch_attr_t attr;
				emscripten_fetch_attr_init(&attr);
				strcpy(attr.requestMethod, "GET");
				attr.onsuccess = downloadSucceeded;
				attr.onerror = downloadFailed;
				attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
				get_fetch = emscripten_fetch(&attr, next_download.c_str());
				is_start = 0;
			}
			if(download_over) {
					if (get_fetch->status != 200) {
						current_step = OVER_FAIL;
					}
					else {
						if (current_step == FETCHING_CONFIG) {
							printf("Reception de la config\n");
							// si la size faite 0 byte on n'est cree nouvelle
							if (get_fetch->numBytes == 0) {
								if (!environment_loader.createNew("/home/web_user", "default")) {
									printf("Echec de la creation de l'environment\n");
									current_step = OVER_FAIL;
								}
								else {
									printf("Chargement de la configuration effectuer, changement vers liste texture\n");
									current_step = PULL_LIST_TEXTURE;
									is_start = -1;
									download_over = false;
									next_download = "/helios/api/yase/texture?list";
								}
							}
						}
						else if(current_step == PULL_LIST_TEXTURE) {
							printf("Reception de la liste texture\n");
							picojson::value v;
							std::string err = picojson::parse(v, get_fetch->data);
							if (!err.empty()) {
								printf("ERR : %s", err.c_str());
							}

							// check if the type of the value is "object"
							if (!v.is<picojson::array>()) {
								std::cerr << "JSON is not an object" << std::endl;
								exit(2);
							}
							printf("Contains array\n");
							// obtain a const reference to the map, and print the contents
							const picojson::value::array& obj = v.get<picojson::array>();
							for (picojson::value::array::const_iterator i = obj.begin();
								i != obj.end();
								++i) {
								if (i->is<string>()) {
									files_download.push(i->get<string>());
								}
							}
							if (files_download.empty()) {
								printf("Pas de texture passe au next\n");
								current_step = PULL_LIST_SKYBOX;
							}
							else {
								current_step = DOWLOAD_TEXTURE;
								is_start = -1;
								download_over = false;
								last_file = files_download.front();
								next_download = ("/helios/api/yase/texture?name=" + last_file).c_str();
								files_download.pop();
							}
						}
						else if (current_step == DOWLOAD_TEXTURE) {
							// un fichier de finit de download
							char* data = new char[get_fetch->numBytes];
							memcpy(data, get_fetch->data, get_fetch->numBytes);
							printf("Memecpy donne\n");
							// Ajoute la texture a l'environnement
							if (!environment_loader.getTextureManager()->AddNewTexture(last_file, data, get_fetch->numBytes)) {
								printf("Echec chargement de %s\n", last_file.c_str());
							}

							if (files_download.size() > 0) {
								printf("Stil %d texture file to download\n", files_download.size());
								is_start = -1;
								download_over = false;
								last_file = files_download.front();
								next_download = ("/helios/api/yase/texture?name=" + last_file).c_str();
								files_download.pop();
							}
							else {
								printf("Chargement de la configuration effectuer, changement vers liste texture\n");
								current_step = PULL_LIST_SKYBOX;
								is_start = -1;
								download_over = false;
								next_download = "/helios/api/yase/skybox";
							}
						}
						else if (current_step == PULL_LIST_SKYBOX) {
							printf("Reception de la liste skybox\n");
							picojson::value v;
							std::string err = picojson::parse(v, get_fetch->data);
							if (!err.empty()) {
								printf("ERR : %s", err.c_str());
							}

							// check if the type of the value is "object"
							if (!v.is<picojson::array>()) {
								std::cerr << "JSON is not an object" << std::endl;
								exit(2);
							}
							printf("Contains array\n");
							// obtain a const reference to the map, and print the contents
							const picojson::value::array& obj = v.get<picojson::array>();
							for (picojson::value::array::const_iterator i = obj.begin();
								i != obj.end();
								++i) {
								if (i->is<string>()) {
									files_download.push(i->get<string>());
								}
							}
							if (files_download.empty()) {
								printf("Aucune skybox\n");
								current_step = PULL_LIST_LOADED_TEX;
								is_start = -1;
								download_over = false;
								next_download = ("/helios/api/yase/model?texture");
							}
							else {
								current_step = DOWLOAD_SKYBOX;
								is_start = -1;
								download_over = false;
								last_file = files_download.front();
								next_download = ("/helios/api/yase/skybox?name=" + last_file + "&face=" + std::to_string(face_index)).c_str();
								files_download.pop();
							}
						}
						else if (current_step == DOWLOAD_SKYBOX) {

							// On temporise la faces

						}
						else if (current_step == PULL_LIST_LOADED_TEX) {
						printf("Reception de la liste texture model\n");
						picojson::value v;
						std::string err = picojson::parse(v, get_fetch->data);
						if (!err.empty()) {
							printf("ERR : %s", err.c_str());
						}

						// check if the type of the value is "object"
						if (!v.is<picojson::array>()) {
							std::cerr << "JSON is not an object" << std::endl;
							exit(2);
						}
						printf("Contains array\n");
						// obtain a const reference to the map, and print the contents
						const picojson::value::array& obj = v.get<picojson::array>();
						for (picojson::value::array::const_iterator i = obj.begin();
							i != obj.end();
							++i) {
							if (i->is<string>()) {
								string sss = i->get<string>();
								vector<string> results = split(sss, ";");
								printf("Value 1 : %s Value 2 : %s\n", results[0].c_str(), results[1].c_str());
								environment_loader.getTextureManager()->AddNewLoadedTexture(results[0]+results[1], results[1], YASE::DEF::REPEATED, YASE::DEF::LINEAR);
							}
						}
						current_step = PULL_LIST_MODEL;
						is_start = -1;
						download_over = false;
						next_download = ("/helios/api/yase/model");
						}
						else if (current_step == PULL_LIST_MODEL) {
						printf("Reception de la liste model\n");
						picojson::value v;
						std::string err = picojson::parse(v, get_fetch->data);
						if (!err.empty()) {
							printf("ERR : %s", err.c_str());
						}

						// check if the type of the value is "object"
						if (!v.is<picojson::array>()) {
							std::cerr << "JSON is not an object" << std::endl;
							exit(2);
						}
						printf("Contains array\n");
						// obtain a const reference to the map, and print the contents
						const picojson::value::array& obj = v.get<picojson::array>();
						for (picojson::value::array::const_iterator i = obj.begin();
							i != obj.end();
							++i) {
							if (i->is<string>()) {
								files_download.push(i->get<string>());
							}
						}
						if(files_download.empty()) {
							printf("Pas de model\n");
							home_page_over = true;
						}
						else {
							current_step = DOWNLOAD_MODEL;
							is_start = -1;
							download_over = false;
							last_file = files_download.front();
							next_download = ("/helios/api/yase/model?name=" + last_file).c_str();
							files_download.pop();
						}
						}
						else if (current_step == DOWNLOAD_MODEL) {
						// un fichier de finit de download
						char* data = new char[get_fetch->numBytes];
						memcpy(data, get_fetch->data, get_fetch->numBytes);
						printf("Memecpy donne\n");

						// Ajoute le model a l,environement
						yas::mem_istream is((void*)data, get_fetch->numBytes);
						YaseModel m;
						yas::load < yas::mem | yas::binary>(is.get_shared_buffer(),m);
						printf("Model is convert %s\n", m.name.c_str());
						environment_loader.getModelManager()->AddModel(last_file, m);
						// Le convertit de memory vers obj pour l'ajouter ensuite

						if (files_download.size() > 0) {
							printf("Stil %d model file to download\n", files_download.size());
							is_start = -1;
							download_over = false;
							last_file = files_download.front();
							next_download = ("/helios/api/yase/model?name=" + last_file).c_str();
							files_download.pop();
						}
						else {
							home_page_over = true;
						}
						}
					}
					emscripten_fetch_close(get_fetch); // Free data associated with the fetch.
			}
			ImGui::End();
#else
			home_window.Draw(&home_page_over,&home_page_over);

#endif
			if (home_page_over)
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
	c.init();
	

#ifdef EMSCRIPTEN
	emscripten_set_main_loop(run_main, 0,1);
#else
	run_main_loop();
	c.shutdown();
#endif
	return 0;
}
