#pragma once

#include "MapTools.h"

#include <BWAPI.h>
//#include "bwem.h"

//namespace { auto& theMap = BWEM::Map::Instance(); }
class StarterBot
{
	MapTools m_mapTools;
	int m_drones_wanted = 4;
	int m_rushZergs = 6;
	int m_defenseZergs = 0;
	int m_pitsWanted = 1;
	int m_overlordWanted = 1;
	int m_hatWanted = 2;
	int m_exWanted = 1;
	int m_lairWanted = 1;
	int m_hydraDenWanted = 1;
	int m_hydralisk = 0;
	bool m_rushed = false;
	BWAPI::Unit m_scout = nullptr;
	bool m_enemy_found = false;
	bool m_pitBuilt = false;
	bool m_hatcheryBuilt = false;
	bool m_extractorBuilt = false;
	bool m_overlordBuilt = false;
	bool m_lairBuilt = false;
	bool m_hydraDenBuilt = false;
	bool ovie_making = false;
	bool ovie_wanted = 2;
	int m_rushCount = 3;
	int m_defenseCount = 3;
	bool m_toExtracor = false;
	bool m_building = false;
	BWAPI::Unit m_target;
	BWAPI::Position m_target_position;
public:

	StarterBot();

	// helper functions to get you started with bot programming and learn the API
	void sendIdleWorkersToMinerals();
	void sendWorkersToExtractor();
	void trainAdditionalWorkers();
	void buildAdditionalSupply();
	void drawDebugInformation();
	bool trainAdditionalUnit(BWAPI::UnitType type);
	void trainZergling();
	void trainHydralisk();

	void buildHatchery();
	void buildExtractor();
	void buildLair();
	void buildSpawnPit();
	void buildOverlord();
	void buildHydraliskDen();
	void researchLurker();

	void scout();
	void rush();
	void checkBuildOrder();
	bool enemyFound();
	void enemyInRange(BWAPI::Unit unit);
	void checkUnits();
	void stopWorker();
	void movetoNexus();
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