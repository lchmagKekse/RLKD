#include "pch.h"
#include "RLKD.h"
#include <direct.h>
#include <iostream>
#include <fstream>
#include <sstream>

BAKKESMOD_PLUGIN(RLKD, "K/D Ratio for Rocket League", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::shared_ptr<GameWrapper> gw;

void RLKD::onLoad() {

	_globalCvarManager = cvarManager;

    gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {

        if (KD_Display_in_Menu && plugin_Enabled) {
            if (show_Total_KD || show_Match_KD)
                Render(canvas);
        }
        else if (gw->IsInOnlineGame() && plugin_Enabled) {
            if (show_Total_KD || show_Match_KD)
                Render(canvas);
        }    
    });

    cvarManager->registerCvar("plugin_Enabled", "1", "Enabled RLKD", false, true, 0, true, 1, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        plugin_Enabled = cvar.getBoolValue();
    });

    cvarManager->registerCvar("show_Match_KD", "1", "Show Match K/D", false, true, 0, true, 1, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        show_Match_KD = cvar.getBoolValue();
    });

    cvarManager->registerCvar("show_Total_KD", "1", "Show Total K/D", false, true, 0, true, 1, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        show_Total_KD = cvar.getBoolValue();
    });

    cvarManager->registerCvar("KD_Display_in_Menu", "1", "Show K/D in Menu", false, true, 0, true, 1, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_Display_in_Menu = cvar.getBoolValue();
    });

    cvarManager->registerCvar("Total_KD_Text", "Total K/D: ", "Total K/D Display Text", false, false, 420, false, 420, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        Total_KD_Text = cvar.getStringValue();
    });

    cvarManager->registerCvar("Match_KD_Text", "Match K/D: ", "Match K/D Display Text", false, false, 420, false, 420, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        Match_KD_Text = cvar.getStringValue();
    });

    cvarManager->registerCvar("KD_decimals", "2", "K/D decimal points", false, true, 0, true, 10, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_decimals = cvar.getIntValue();
    });

    cvarManager->registerCvar("KD_TextSize", "30", "K/D Text Size", false, true, 10, true, 100, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_TextSize = cvar.getIntValue();
    });

    cvarManager->registerCvar("KD_X", "0", "K/D X Location", false, true, 0, false, 420, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_X = cvar.getIntValue();
    });

    cvarManager->registerCvar("KD_Y", "50", "K/D Y Location", false, true, 0, false, 420, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_Y = cvar.getIntValue();
    });

    cvarManager->registerCvar("KD_color", "#FFFFFF", "Color of K/D Text", false, false, 420, false, 420, true);

    cvarManager->registerCvar("KD_dropShadow", "1", "Drop shadow on K/D Text", false, true, 0, true, 1, true)
        .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
        KD_dropShadow = cvar.getBoolValue();
    });

    gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
        [this](ServerWrapper caller, void* params, std::string eventname) {
        if(gw->IsInOnlineGame() && plugin_Enabled)
            onStatTickerMessage(params);
    });

    gameWrapper->HookEventPost("Function TAGame.GFxData_Playlist_TA.HandleSkillUpdateOnGameJoin",
        [this](std::string eventName) {
        matchKills = 0;
        matchDeaths = 0;
        calculateKD();
    });

    gameWrapper->SetTimeout([this](GameWrapper* gw) { init(); }, 2.0f);
}

void RLKD::onUnload(){ }

void RLKD::init() {

    if (_mkdir(getSaveDir()) == 0)
        cvarManager->log("SavePath created");

    if(std::filesystem::exists(getSaveFile()))
        while (!loadData());

    calculateKD();

    Total_KD_Text = cvarManager->getCvar("Total_KD_Text").getStringValue();
    Match_KD_Text = cvarManager->getCvar("Match_KD_Text").getStringValue();

    strcpy(tempTotalText, Total_KD_Text.c_str());
    strcpy(tempMatchText, Match_KD_Text.c_str());

}

void RLKD::calculateKD() {

    std::stringstream MKD;
    if(matchDeaths == 0.0f)
        MKD << std::fixed << std::setprecision(KD_decimals) << matchKills;
    else
        MKD << std::fixed << std::setprecision(KD_decimals) << (matchKills / matchDeaths);
    matchKD = MKD.str();

    std::stringstream TKD;
    if(totalDeaths == 0.0f)
        TKD << std::fixed << std::setprecision(KD_decimals) << totalKills;
    else
        TKD << std::fixed << std::setprecision(KD_decimals) << (totalKills / totalDeaths);
    totalKD = TKD.str();

}

void RLKD::onStatTickerMessage(void* params) {

    struct StatTickerParams {

        uintptr_t Receiver;
        uintptr_t Victim;
        uintptr_t StatEvent;
    };

    StatTickerParams* pStruct = (StatTickerParams*)params;
    PriWrapper receiver = PriWrapper(pStruct->Receiver);
    PriWrapper victim = PriWrapper(pStruct->Victim);
    StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

    if (statEvent.GetEventName() == "Demolish") {

        if (!receiver) return;
        if (!victim) return;

        PlayerControllerWrapper playerController = gameWrapper->GetPlayerController();
        if (!playerController) return;
        PriWrapper playerPRI = playerController.GetPRI();
        if (!playerPRI) return;

        if (playerPRI.memory_address == victim.memory_address) {

            matchDeaths++;
            totalDeaths++;
            saveData();
            calculateKD();
            return;
        }

        if (playerPRI.memory_address == receiver.memory_address) {

            matchKills++;
            totalKills++;
            saveData();
            calculateKD();
            return;
        }
    }
}

void RLKD::Render(CanvasWrapper canvas) {

    CVarWrapper textColorVar = cvarManager->getCvar("KD_color");
    if (!textColorVar) return;

    LinearColor textColor = textColorVar.getColorValue();
    canvas.SetColor(textColor);

    if (show_Total_KD) {

        canvas.SetPosition(Vector2F{ (float)KD_X, (float)KD_Y });
        canvas.DrawString(Total_KD_Text + totalKD, KD_TextSize / 10.0f, KD_TextSize / 10.0f, KD_dropShadow);
    }
    
    if (show_Match_KD) {

        canvas.SetPosition(Vector2F{ (float)KD_X, ((float)KD_Y + (float) KD_TextSize*1.5f) });
        canvas.DrawString(Match_KD_Text + matchKD, KD_TextSize / 10.0f, KD_TextSize / 10.0f, KD_dropShadow);
    }
}

void RLKD::saveData() {

    std::fstream file;
    file.open(getSaveFile(), std::ios::out);

    if (file.is_open()) {

        file << totalKills;
        file << "\n";
        file << totalDeaths;
    }

    file.close();
}

bool RLKD::loadData() {

    std::fstream file;
    file.open(getSaveFile(), std::ios::in);

    if (!file.is_open()) return false;

    std::string line;

    std::getline(file, line);
    totalKills = stoi(line);

    std::getline(file, line);
    totalDeaths = stoi(line);

    file.close();
    return true;
}

const char* RLKD::getSaveDir() {

    auto BMpath = gameWrapper->GetDataFolder() / "RLKD";
    std::string BMpath_str = BMpath.string();
    return BMpath_str.c_str();
}

const char* RLKD::getSaveFile() {

    auto BMpath = gameWrapper->GetDataFolder() / "RLKD" / "RLKD.txt";
    std::string BMpath_str = BMpath.string();
    return BMpath_str.c_str();
}