#ifndef ENV_PAGE_H
#define ENV_PAGE_H

#include "imgui_other.h"
#include "logger.h"

#include "env_manager.h"

struct CameraSettings
{
	FPSCamera*	camera = nullptr;

	void Draw(bool* p_open = nullptr)
	{
		if (*p_open == false)
			return;
		static float fov = 70.0f;
		if(ImGui::Begin("Camera",p_open))
		{
				ImGui::TextColored({ 255,255,0,1 }, "Nom : %s", camera->getName().c_str());
				ImGui::Separator();
				ImGui::Text("YAW %.2f PITCH %.2f", camera->getYaw(), camera->getPitch());
				const auto& v = camera->getCameraPosition();
				const auto& f = camera->getCameraFront();
				const auto& u = camera->getCameraUp();
				ImGui::Text("Position X : %.2f Y : %.2f Z : %.2f", v.x, v.y, v.z);


				if (ImGui::DragFloat("FOV", &fov, 1, 5, 180))
				{
					camera->setFOV(fov);
				}
				ImGuiIO& io = ImGui::GetIO();
				ImGui::Button("Move Front");
				if (ImGui::IsItemActive())
				{
					// Draw a line between the button and the mouse cursor
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					draw_list->PushClipRectFullScreen();
					draw_list->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
					draw_list->PopClipRect();

					// Drag operations gets "unlocked" when the mouse has moved past a certain threshold (the default threshold is stored in io.MouseDragThreshold)
					// You can request a lower or higher threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta()
					ImVec2 value_raw = ImGui::GetMouseDragDelta(0);
					ImGui::SameLine(); ImGui::Text("WithLockThresold (%.1f, %.1f)", value_raw.x, value_raw.y);
					camera->move(value_raw.x*1.0f, value_raw.y*1.0f);

					if (io.KeysDown[GLFW_KEY_A]) {
						camera->left();
					}
					if (io.KeysDown[GLFW_KEY_W]) {
						camera->right();
					}
					if (io.KeysDown[GLFW_KEY_S]) {
						camera->backward();
					}
					if (io.KeysDown[GLFW_KEY_D]) {
						camera->right();
					}
					if (io.KeysDown[GLFW_KEY_Q])
					{
						camera->down();
					}
					if (io.KeysDown[GLFW_KEY_E])
					{
						camera->up();
					}
				} else
				{
					camera->ResetMouse();
				}
			}
			ImGui::End();
	}
};

struct TexturePreview
{

	void Draw(uint text_id, bool* p_open = nullptr)
	{
		static int img_size[2]{ 200, 200};
		if(text_id != ERROR_TEXTURE && *p_open)
		{
			if(ImGui::Begin("Apercu texture",p_open))
			{
				ImGui::DragInt2("Grandeur", img_size, 1, 10, 800);
				ImGui::Separator();
				if (text_id != ERROR_TEXTURE)
				{
					ImGui::Image((void*)text_id, ImVec2(img_size[0], img_size[1]));
				}

				ImGui::End();
			}

		}
		
	}
	
};


class EnvironmentWindow {

	EnvManager*			loader;
	TextureManager*		tex_man;
	SkyBoxManager*		sb_man;

	bool				shader_panel = false;
	bool				logger_panel = false;
	bool				textures_panel = true;
	bool				assets_panel = false;
	bool				skybox_panel = true;

	bool				camera_panel = false;
	bool				textures_preview = false;

	bool				run_main_loop = true;
	
	AppLog				log_window;
	TexturePreview		texture_preview_window;
	CameraSettings		camera_settings_window;

public:
	EnvironmentWindow(EnvManager* l) {
		this->loader = l;
		this->tex_man = l->getTextureManager();
		this->sb_man = l->getSkyBoxManager();

	}

	void ShowLogger() {
		if (logger_panel) {
			for (const auto& v : logger.Consume()) {
				log_window.AddLog(v.c_str());
			}
			log_window.Draw("YASE Logger", &logger_panel);
		}
	}

	void ShowOverlay() {
		const float DISTANCE = 10.0f;
		static int corner = 3;
		ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		if (corner != -1)
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
		if (ImGui::Begin("Information Application", &run_main_loop, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("Nom environment : %s", loader->getName().c_str());
			ImGui::Text("Version : %d:%d:%d \n" "Framerate %.2f DeltaTime %.2f", 0, 0, 1, io.Framerate, io.DeltaTime);
		}
		ImGui::End();
	}

	void ShowMenuBar() {

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Fichier"))
			{
				if(ImGui::MenuItem("Sauvegarder"))
				{
					if(!loader->save())
					{
						YASE_LOG_ERROR("Erreur de la sauvegarde de l'environment\n");
					}
				}

				if(ImGui::MenuItem("LOL"))
				{
					loader->getActiveScene().LoadModel();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quitter", "Alt+F4"))
				{
					run_main_loop = false;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Fenetre"))
			{
				ImGui::Checkbox("Textures", &textures_panel);
				ImGui::Checkbox("Shaders", &textures_panel);
				if(ImGui::Checkbox("Camera", &camera_panel))
				{
					camera_settings_window.camera = loader->getActiveScene().getWorkingCamera();
				}
				ImGui::Separator();
				ImGui::Checkbox("Logger", &logger_panel);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void ShowTexturePanel() {
		if (textures_panel) {
			static bool add_new_texture = false;
			static FileExplorer fx;

			static int selected_texture = -1;
			static bool show_preview = false;

			if (ImGui::Begin("Textures environment",&textures_panel)) {
				ImGui::TextColored({ 255,255,0,1 }, "Ajouter et configurer les textures de votre environment");
				ImGui::Separator();
				ImGui::Text("Textures total : %d Textures charger : %d", tex_man->getTexturesCount(), tex_man->getTexturesLoadedCount());
				ImGui::Separator();
				if (ImGui::Button("Ajouter nouvelle texture")) {
					fx.Start();
				}

				if (fx.waiting_data) {
					fx.Draw();
					if (fx.is_close) {
						// Texture selectionner
						fx.waiting_data = false;
						fs::path p = fx.selected_current;
						if(!p.empty())
						{
							if(!tex_man->AddNewTexture(p))
							{
								// snif snif
							}
						}
					}
				}
				static int selection_mask = (1 << 2);
				static uint text_id = ERROR_TEXTURE;
				// Section liste de mes textures loader
				int i = 0;
				if (ImGui::TreeNode("Textures")) {
					for(const auto& t : tex_man->getTextures())
					{
						ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
						// Node
						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%d %s",t.id(), t.name().c_str());
						if (ImGui::IsItemClicked())
							selected_texture = t.id();
						if (node_open)
						{
							ImGui::Text("Category %s",t.category().c_str());
							ImGui::Text("Dimension %d x %d", t.width(), t.height());
							ImGui::Text("Channel %d", t.channels());
							
							ImGui::TreePop();
						}
						i++;
					}
					if(selected_texture != -1)
					{
						selection_mask = (1 << selected_texture);
					}

					ImGui::TreePop();
				}
				if(ImGui::TreeNode("Texture charger"))
				{
					for(const auto& t : tex_man->getLoadedTextures())
					{
						ImGui::Text("ID : %d GLID : %d", t.first, t.second);
					}

					ImGui::TreePop();
				}
				if(ImGui::Button("Afficher"))
				{
					if(selected_texture != -1)
					{
						text_id = tex_man->loadTexture(selected_texture);
						show_preview = true;
					}
					
				}
				if(text_id != ERROR_TEXTURE)
				{
					texture_preview_window.Draw(text_id, &show_preview);
				}

				ImGui::Separator();

				ImGui::End();
			}
		}
	}

	void ShowSkyBoxPanel()
	{
		if (skybox_panel)
		{
			static string selected_skybox = "";
			static int	  selected_index = -1;
			static FileExplorer fx;
			static int selection_mask = (1 << 2);
			static char bufName[100];

			if (ImGui::Begin("Skybox environment", &skybox_panel)) {
				ImGui::TextColored({ 255,255,0,1 }, "Ajouter et configurer les skybox de votre environment");
				ImGui::Separator();
				ImGui::Text("Skybox total : %d Skybox charger : %d", sb_man->getNumberSkyBox(), sb_man->getNumberLoadedSkyBox());
				ImGui::Separator();

				ImGui::InputText("Nom", bufName, 100);
				ImGui::SameLine();
				if (ImGui::Button("Ajouter")) {
					if (bufName == NULL)
						return;
					fx.get_folder = true;
					fx.Start();
				}
				if (fx.waiting_data) {
					fx.Draw();
					if (fx.is_close) {
						// Texture selectionner
						fx.waiting_data = false;
						fs::path p = fx.selected_current;
						if (!p.empty())
						{
							if (!sb_man->AddSkyBox(bufName,p))
							{
								YASE_LOG_ERROR("Impossible d'ajouter la skybox\n");
							}
						}
					}
				}
				if (ImGui::TreeNode("Skybox")) {
					int i = 0;
					for (const auto& b : sb_man->getMapSkyBox())
					{
						ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
						// Node
						bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", b.first.c_str());
						if (ImGui::IsItemClicked()) {
							selected_skybox = b.first;
							selected_index = i;
						}
						if (node_open)
						{
							ImGui::Text("Extensions %s", get<0>(b.second).ext().c_str());

							ImGui::TreePop();
						}
						i++;
					}
					if (selected_index != -1)
					{
						selection_mask = (1 << selected_index);
					}

					ImGui::TreePop();
				}
				if(ImGui::Button("Afficher"))
				{
					if (!selected_skybox.empty())
					{
						loader->getActiveScene().setActiveSkybox(selected_skybox);
					}

				}
				ImGui::End();
			}
		}
	}

	void Draw(bool* over) {
		if (run_main_loop = false) {
			*over = true;
		}


		loader->getActiveScene().Draw();

		ShowMenuBar();
		ShowLogger();
		ShowOverlay();
		ShowTexturePanel();
		ShowSkyBoxPanel();
		camera_settings_window.Draw(&camera_panel);

		ImGui::ShowDemoWindow();
	}

};



#endif