#ifndef HOME_H
#define HOME_H

#include "imgui.h"
#include "imgui_other.h"
#include "env_manager.h"

#include "logger.h"

class HomeWindow
{
	EnvManager* loader = nullptr;

public:
	HomeWindow(EnvManager* el) : loader(el)
	{
		
	}
	
	void Draw(bool* over, bool* p_open = nullptr)
	{
		static bool wait_file = false;
		static bool wait_file_new = false;
		static char	buffer_name[100];
		static char buffer_path[100];
		static FileExplorer fx;

		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPosCenter();
		if(ImGui::Begin("Acceuil", p_open))
		{

			ImGui::TextColored({ 255,255,0,1 }, "YASE Yet another Simple Engine");
			ImGui::Separator();

			ImGui::InputText("Nom", buffer_name, 100);
			ImGui::InputText("Dossier", buffer_path, 100);
			ImGui::SameLine();
			if(ImGui::Button("Parcourir"))
			{
				fx.get_folder = true;
				fx.Start();
				wait_file_new = true;
			}
			if(ImGui::Button("Nouvelle environnment"))
			{
				if(!loader->createNew(buffer_path,buffer_name))
				{
				}
				else {
					*over = true;
				}
			}
			ImGui::SameLine();
			if(ImGui::Button("Charger environnment"))
			{
				fx.get_folder = false;
				fx.Start();
				wait_file = true;
			}

			if(wait_file_new)
			{
				fx.Draw();
				if(fx.is_close)
				{
					wait_file_new = false;
					if(!fx.selected_current.empty())
						strcpy(buffer_path, fx.selected_current.string().c_str());
				}
			}

			if(wait_file)
			{
				fx.Draw();
				if(fx.is_close)
				{
					wait_file = false;
					if(!fx.selected_current.empty())
						if(!loader->load(fx.selected_current))
						{
							// Message d'erreur
						}
						else {
							ImVec2 v = ImGui::GetIO().DisplaySize;
							//loader->getActiveScene().setSceneSize(v.x, v.y);
							*over = true;
						}
				}
			}


			ImGui::End();
		}
	}
};

#endif