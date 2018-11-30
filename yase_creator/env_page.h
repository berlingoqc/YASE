#ifndef ENV_PAGE_H
#define ENV_PAGE_H

#include "imgui_other.h"
#include "logger.h"

#include "env_manager.h"


class EnvironmentWindow {

	EnvManager*			loader;
	TextureManager*		tex_man;
	SkyBoxManager*		sb_man;

	bool				shader_panel = false;
	bool				logger_panel = false;
	bool				textures_panel = true;
	bool				assets_panel = false;
	bool				skybox_panel = true;

	bool				textures_preview = false;

	bool				run_main_loop = true;
	
	AppLog		log_window;

public:
	EnvironmentWindow(EnvManager* l) {
		this->loader = l;
		this->tex_man = l->getTextureManager();
		this->sb_man = l->getSkyBoxManager();
	}

	void ShowLogger() {
		if (logger_panel) {
			YASE_LOG_INFO("Erreur creation nouvelle %s", "popo");
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
					}
					
				}

				ImGui::Separator();
				if(text_id != ERROR_TEXTURE)
				{
					ImGui::Image((void*)text_id,ImVec2(200,200));
				}

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
				ImGui::End();
			}
		}
	}

	void Draw(bool* over) {
		if (run_main_loop = false) {
			*over = true;
		}
		ShowMenuBar();
		ShowLogger();
		ShowOverlay();
		ShowTexturePanel();
		ShowSkyBoxPanel();

		ImGui::ShowDemoWindow();
	}

};



#endif