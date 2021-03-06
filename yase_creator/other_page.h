#ifndef OTHER_PAGE_H
#define OTHER_PAGE_H

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
		static float speed[3];
		ImGui::Begin("Camera", p_open);
		
		if (camera != nullptr) {
				ImGui::TextColored({ 255,255,0,1 }, "Nom : %s", camera->getName().c_str());
				ImGui::Separator();
				ImGui::Text("YAW %.2f PITCH %.2f", camera->getYaw(), camera->getPitch());
				const auto& v = camera->getCameraPosition();
				const auto& f = camera->getCameraFront();
				const auto& u = camera->getCameraUp();
				ImGui::Text("Position X : %.2f Y : %.2f Z : %.2f", v.x, v.y, v.z);

				ImGui::Text("Vitesse");
				const glm::vec3& speed_v = camera->getCameraSpeed();
				speed[0] = speed_v.x; speed[1] = speed_v.y; speed[2] = speed_v.z;
				ImGui::Separator();
				if (ImGui::DragFloat3("## 1", speed, 0.1, 0.0, 50.0)) {
					camera->setCameraSpeed({ speed[0],speed[1],speed[2] });
				}

				if (ImGui::DragFloat("FOV", &camera->fov, 1, 5, 180))
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
						camera->forward();
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
				}
				else
				{
					camera->ResetMouse();
				}
			} else
			{
				ImGui::Text("Vous devez avoir une scene active pour utiliser la camera !");
			}
		ImGui::End();
	}
};

struct TexturePreview
{

	void Draw(uint text_id, bool* p_open = nullptr)
	{
		static int img_size[2]{ 200, 200 };
		if (text_id != ERROR_TEXTURE && *p_open)
		{
			if (ImGui::Begin("Apercu texture", p_open))
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

struct ModelManager_Panel {
	ModelManager*	model_manager = nullptr;
	FileExplorer	fx;
	bool			show = true;


	void Draw() {
		if (show)
		{
			static char			bufNewModelName[50];
			
			ImGui::Begin("Model Manager", &show);
			ImGui::TextColored({ 255,255,0,1 }, "Ajouter et configurer les models de votre environment");
			ImGui::Separator();
			ImGui::Text("Model total : %d Model charger : %d", model_manager->getNumberModel(), model_manager->getNumberLoadedModel());
			ImGui::Separator();
			ImGui::PushItemWidth(150);
			ImGui::InputText("## 1", bufNewModelName, 50);
			ImGui::PopItemWidth();
			if (ImGui::Button("Importer nouveau model")) {
				// Importer un model existant dans les exenstions suivantes obj et yase model
				fx.Start();
			}
			ImGui::SameLine();
			if (fx.waiting_data) { // Si on attend un fichier pour l'importer
				fx.Draw();
				if (fx.is_close) {
					fx.waiting_data = false;
					fs::path p = fx.selected_current;
					if (!p.empty()) {
						model_manager->AddModelAssimp(bufNewModelName, p);
					}
				}
			}

			ImGui::Separator();
			static int selection_mesh_mask = (1 << 2);
			static int selected_mesh_index = -1;
			static int selection_mask = (1 << 2);
			static int selected_index = -1;
			static string selected_model = "";


			if (ImGui::TreeNode("Model")) { // Liste simple des models existants
				int i = 0;
				for (const auto& t : model_manager->getModelMap()) {
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
					bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", t.first.c_str());
					if (ImGui::IsItemClicked()) {
						selected_index = i;
						selected_model = t.first;
					}
					if (node_open) {
						if (t.second == nullptr) {
							ImGui::Text("Model non charger");
							ImGui::SameLine();
							if(ImGui::Button("Charger ?")) {
								// Load le model pour que c'est info soit afficher
							}
						} else
						{
							if(ImGui::TreeNode("Mesh"))
							{
								ImGui::TextColored({255,255,0,1},"%d meshes", t.second->getMeshes().size());
								int ii = 0;
								for(auto& m : t.second->meshes)
								{
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mesh_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
									bool node_mesh_open = ImGui::TreeNodeEx((void*)(intptr_t)ii, node_flags, "%d",ii);
									if (ImGui::IsItemClicked()) {
										selected_mesh_index = ii;
									}
									if(node_mesh_open) {
										// Affiche les detailles de cette meshes
										ImGui::Text("Nombre de vertices : %d",t.second->getMeshes()[ii].vertices.size());
										ImGui::Text("Nombre d'indice : %d",t.second->getMeshes()[ii].indices.size());
										ImGui::TextColored({255,255,0,1},"Texture requise");
										ImGui::Separator();
										for(const auto& tex : t.second->getMeshes()[ii].getNeededTexture()) {
											ImGui::Text("%s",tex.c_str());
										}
										ImGui::TreePop();
									}

									ii++;
								}
								
								if (selected_mesh_index != -1)
								{
									selection_mesh_mask = (1 << selected_mesh_index);
								}
								ImGui::TreePop();
							}
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
			if (ImGui::Button("Charger")) {

			}
			ImGui::SameLine();
			if (ImGui::Button("Effacer")) {

			}

			ImGui::End();
		}
	}



};

#endif