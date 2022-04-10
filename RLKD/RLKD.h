#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH); // "." stringify(VERSION_BUILD);


class RLKD: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{

	float matchKills = 0.0f, matchDeaths = 0.0f, totalKills = 0.0f, totalDeaths = 0.0f;
	std::string matchKD, totalKD;

	bool plugin_Enabled = true;
	float KD_TextSize = 30.0f;
	bool KD_Display_in_Menu = true;
	bool show_Match_KD = true;
	bool show_Total_KD = true;
	bool KD_dropShadow = true;
	int KD_decimals = 2;
	int KD_X = 0, KD_Y = 50;

	std::string Total_KD_Text = "Total K/D: ";
	std::string Match_KD_Text = "Match K/D: ";
	char tempMatchText[64];
	char tempTotalText[64];


	virtual void onLoad();
	virtual void onUnload();
	void init();

	void calculateKD();
	void onStatTickerMessage(void* params);

	void Render(CanvasWrapper canvas);

	void saveData();
	bool loadData();
	std::string getSaveDir();
	std::string getSaveFile();

	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

};

