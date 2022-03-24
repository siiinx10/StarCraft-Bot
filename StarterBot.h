#pragma once

#include "MapTools.h"

#include <BWAPI.h>

class StarterBot
{
    MapTools m_mapTools;

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();
	void buildAdditionalSupply();
	void buildGateway();
	void buildExpansionBuildings();
	void buildCannon();
	void buildCyberCore();
	void buildAssimilator();
	void getExpansionLoc();
	void scoutEnemy();
	void sendWorkersToGas();
	void getExpansionLoc();
	void scoutEnemy();
	bool foundEnemyBase();
	bool baseUnderattack();
	bool expansionUnderattack();
	bool readyForAttack();
	void attack();
	bool atEnemyBase(BWAPI::Unit unit);
	void positionIdleZealots();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
};
