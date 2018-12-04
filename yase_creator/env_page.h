#ifndef ENV_PAGE_H
#define ENV_PAGE_H

#include "other_page.h"

class EnvironmentWindow {

	EnvManager*			loader;
	TextureManager*		tex_man;
	SkyBoxManager*		sb_man;

	BaseScene*			current_scene = nullptr;

	bool				logger_panel = false;

	bool				shader_panel = false;
	bool				textures_panel = true;
	bool				model_panel = false;
	bool				skybox_panel = true;
	bool				camera_panel = false;
	bool				scene_panel = false;


	bool				textures_preview = false;

	bool				run_main_loop = true;
	
	AppLog				log_window;
	TexturePreview		texture_preview_window;
	CameraSettings		camera_settings_window;
	ModelManager_Panel  model_manager_window;

public:
	EnvironmentWindow(EnvManager* l) {
		this->loader = l;
		this->tex_man = l->getTextureManager();
		this->sb_man = l->getSkyBoxManager();
		model_manager_window.model_manager = l->getModelManager();
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
			if(current_scene != nullptr)
			{
				ImGui::Text("Scene active : %s", current_scene->name.c_str());
			}
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
					ModelManager* mm = loader->getModelManager();
					try {
						mm->AddModel("ground", getDefaultModel());
					}
					catch (const ModelException& ex) {
						YASE_LOG_ERROR(ex.what());
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quitter", "Alt+F4"))
				{
					// TODO : regarder si l'environnement a changer et demande si on veut sauvgarder avec de quitter
					run_main_loop = false;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Environment"))
			{
				ImGui::TextColored({ 255,255,0,1 }, "Fenetre");
				ImGui::Checkbox("Texture", &textures_panel);
				ImGui::Checkbox("Shader", &shader_panel);
				ImGui::Checkbox("Skybox", &skybox_panel);
				if (ImGui::Checkbox("Model", &model_panel)) {
					model_manager_window.show = model_panel;
				}
				ImGui::Separator();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene"))
			{
				ImGui::TextColored({ 255,255,0,1 }, "Fenetre");
				ImGui::Checkbox("Scene", &scene_panel);
				ImGui::Separator();
				if (current_scene != nullptr) {
					if (ImGui::Checkbox("Camera", &camera_panel))
					{
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Aide"))
			{
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
		if (true)
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

	void ShowScenePanel()
	{
		if (scene_panel) {
			if (ImGui::Begin("Scene", &scene_panel))
			{
				if (current_scene == nullptr) {
					static bool popup_open = false;
					static char bufName[100];
					static vector<const char*> scene_items = loader->getSceneManager()->getKeys();
					static int scene_item_current = -1;
					ImGui::Text("Aucune scene active. Cree une nouvelle scene ou charger une existante");

					ImGui::Combo("Scene ", &scene_item_current, scene_items.data(), scene_items.size());
					ImGui::SameLine();
					if(ImGui::Button("Charger"))
					{
						if(scene_item_current >= -1)
						{
							current_scene = loader->getSceneManager()->loadScene(scene_items[scene_item_current]);
							camera_settings_window.camera = &current_scene->work_camera;
						}
						
					}
					ImGui::SameLine();
					if (ImGui::Button("Nouvelle scene"))
					{
						ImGui::OpenPopup("Nouvelle Scene");
						popup_open = true;
					}

					if (ImGui::BeginPopupModal("Nouvelle Scene"))
					{

						ImGui::InputText("Nom", bufName, 100);

						if (ImGui::Button("Cree")) {
							ImGui::CloseCurrentPopup();
							// Valide le nom et cree notre nouvelle scene
							string scene_name = string(bufName);
							if (scene_name.empty())
								return;
							// Crée une nouvelle scene
							ImGuiIO& io = ImGui::GetIO();
							bool b = loader->getSceneManager()->addNewScene(scene_name, io.DisplaySize.x, io.DisplaySize.y);
							if(b)
							{
								current_scene = loader->getSceneManager()->getActiveScene();
								camera_settings_window.camera = &current_scene->work_camera;
							}
						}

						ImGui::EndPopup();
					}

				}
				else
				{
					static int selected_skybox = -1;
					static vector<const char*> skybox = sb_man->getKeys();

					static int selected_add_model = -1;
					static vector<const char*> add_model_list = loader->getModelManager()->getKeys();
					static char	add_mode_name[100];

					ImGui::TextColored({ 255,255,0,1 }, "Scene active %s", current_scene->name.c_str());
					ImGui::Separator();
					ImGui::Text("Environnement");
					ImGui::Separator();
					ImGui::Text("Skybox : %s",current_scene->active_skybox.c_str());
					ImGui::SameLine();
					ImGui::Combo("##1", &selected_skybox, skybox.data(), skybox.size());
					ImGui::SameLine();
					if(ImGui::Button("Update"))
					{
						if (selected_skybox >= 0) {
							current_scene->setActiveSkybox(sb_man->getKeys()[selected_skybox]);
						}
					}
					ImGui::SameLine();
					if(ImGui::Button("Clear"))
					{
						selected_skybox = -1;
						current_scene->setActiveSkybox("");
					}
					ImGui::Separator();
					ImGui::InputText("Nom", add_mode_name, 100);
					ImGui::Combo("##2", &selected_add_model, add_model_list.data(), add_model_list.size());
					if(ImGui::Button("Ajouter model"))
					{
						if(selected_add_model >= 0)
						{
							current_scene->addEnvironmentModel(add_mode_name, add_model_list[selected_add_model]);
						}
					}
					if (ImGui::TreeNode("Model statique")) {
						static int selected_index = -1;
						static string selected_model = "";
						static int selection_mask = (1 << 2);

						static float	scale_values[3];

						int i = 0;
						for (const auto& b : current_scene->env_model_map)
						{
							ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
							// Node
							bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", b.first.c_str());
							if (ImGui::IsItemClicked()) {
								selected_model = b.first;
								selected_index = i;
							}
							if (node_open)
							{
								scale_values[0] = b.second->transformation.scale.x; scale_values[1] = b.second->transformation.scale.y; scale_values[2] = b.second->transformation.scale.z;
								// Control la matrice model
								if(ImGui::DragFloat3("Scale", scale_values, 1, 0, 1000))
								{
									b.second->transformation.scale.x = scale_values[0];
									b.second->transformation.scale.y = scale_values[1];
									b.second->transformation.scale.z = scale_values[2];
									b.second->updateModel();
								}
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

				}


				ImGui::End();
			}
		}
	}

	void Draw(bool* over) {
		if (run_main_loop = false) {
			*over = true;
		}

		if(current_scene != nullptr)
		{
			current_scene->Draw();
		}

		ShowMenuBar();
		ShowLogger();
		ShowOverlay();
		ShowTexturePanel();
		ShowSkyBoxPanel();
		ShowScenePanel();
		camera_settings_window.Draw(&camera_panel);
		model_manager_window.Draw();
	}

};



#endif