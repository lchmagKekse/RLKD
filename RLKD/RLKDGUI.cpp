#include "pch.h"
#include "RLKD.h"

std::string RLKD::GetPluginName() {
	return "K/D Ratio";
}

void RLKD::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void RLKD::RenderSettings() {

	CVarWrapper plugin_Enabled = cvarManager->getCvar("plugin_Enabled");
	if (!plugin_Enabled) return;
	bool plugin_Enabled_var = plugin_Enabled.getBoolValue();

	if (ImGui::Checkbox("Enable Plugin", &plugin_Enabled_var))
		plugin_Enabled.setValue(plugin_Enabled_var);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Kills and Deaths are only counted when enabled");
	}

	ImGui::NewLine();

	ImGui::Text("Total Kills: ");
	ImGui::SameLine(0,0);
	ImGui::Text(std::to_string((int)totalKills).c_str());

	ImGui::Text("Total Deaths: ");
	ImGui::SameLine(0,0);
	ImGui::Text(std::to_string((int)totalDeaths).c_str());

	ImGui::Text("Total K/D: ");
	ImGui::SameLine(0,0);
	ImGui::Text(totalKD.c_str());

	ImGui::Text("Match Kills: ");
	ImGui::SameLine(0,0);
	ImGui::Text(std::to_string((int)matchKills).c_str());

	ImGui::Text("Match Deaths: ");
	ImGui::SameLine(0,0);
	ImGui::Text(std::to_string((int)matchDeaths).c_str());

	ImGui::Text("Match K/D: ");
	ImGui::SameLine(0,0);
	ImGui::Text(matchKD.c_str());

	ImGui::Separator();
	ImGui::NewLine();
	
	
	CVarWrapper KD_Display_in_Menu = cvarManager->getCvar("KD_Display_in_Menu");
	if (!KD_Display_in_Menu) return;
	bool KD_Display_in_Menu_var = KD_Display_in_Menu.getBoolValue();
	
	if (ImGui::Checkbox("Show K/D in Menu", &KD_Display_in_Menu_var))
		KD_Display_in_Menu.setValue(KD_Display_in_Menu_var);


	CVarWrapper show_ALL_KD = cvarManager->getCvar("show_Total_KD");
	if (!show_ALL_KD) return;
	bool show_ALL_KD_var = show_ALL_KD.getBoolValue();

	if (ImGui::Checkbox("Show Total K/D", &show_ALL_KD_var))
		show_ALL_KD.setValue(show_ALL_KD_var);


	CVarWrapper show_KD = cvarManager->getCvar("show_Match_KD");
	if (!show_KD) return;
	bool show_KD_var = show_KD.getBoolValue();

	if (ImGui::Checkbox("Show Match K/D", &show_KD_var))
		show_KD.setValue(show_KD_var);


	strcpy(tempTotalText, Total_KD_Text.c_str());
	if (ImGui::InputText("Total K/D Text", tempTotalText, IM_ARRAYSIZE(tempTotalText))) {
		Total_KD_Text = tempTotalText;
		cvarManager->getCvar("Total_KD_Text").setValue(Total_KD_Text); }

	strcpy(tempMatchText, Match_KD_Text.c_str());
	if (ImGui::InputText("Match K/D Text", tempMatchText, IM_ARRAYSIZE(tempMatchText))) {
		Match_KD_Text = tempMatchText;
		cvarManager->getCvar("Match_KD_Text").setValue(Match_KD_Text); }

	if (ImGui::InputInt("Decimal Places", &KD_decimals, 1)) {
		cvarManager->getCvar("KD_decimals").setValue(KD_decimals);
		calculateKD(); }
	if (KD_decimals >= 10)
		cvarManager->getCvar("KD_decimals").setValue(10);
	if (KD_decimals <= 0)
		cvarManager->getCvar("KD_decimals").setValue(0);

	if (ImGui::InputFloat("Text Size", &KD_TextSize, 1.0f, 1.0f, "%.0f"))
		cvarManager->getCvar("KD_TextSize").setValue(KD_TextSize);
	if (KD_TextSize >= 100)
		cvarManager->getCvar("KD_TextSize").setValue(100);
	if (KD_TextSize <= 10)
		cvarManager->getCvar("KD_TextSize").setValue(10);

	if (ImGui::InputInt("Text Location X", &KD_X, 1, 10))
		cvarManager->getCvar("KD_X").setValue(KD_X);
	if (KD_X <= 0)
		cvarManager->getCvar("KD_X").setValue(0);

	if (ImGui::InputInt("Text Location Y", &KD_Y, 1, 10))
		cvarManager->getCvar("KD_Y").setValue(KD_Y);
	if (KD_Y <= 0)
		cvarManager->getCvar("KD_Y").setValue(0);
		

	CVarWrapper textColorVar = cvarManager->getCvar("KD_color");
	if (!textColorVar) { return; }
	LinearColor textColor = textColorVar.getColorValue() / 255;

	if (ImGui::ColorEdit4("Text Color", &textColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar))
		textColorVar.setValue(textColor * 255);


	CVarWrapper KD_dropShadow = cvarManager->getCvar("KD_dropShadow");
	if (!KD_dropShadow) return;
	bool KD_dropShadow_var = KD_dropShadow.getBoolValue();

	if (ImGui::Checkbox("Text Shadow", &KD_dropShadow_var))
		KD_dropShadow.setValue(KD_dropShadow_var);

}
