#pragma once

#include <BWAPI.h>
#include <windows.h>
#include <bwem.h>
//namespace { auto& theMap = BWEM::Map::Instance(); }
namespace Tools
{
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units,bool complete = false);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type, BWAPI::Unit skip = nullptr);
    BWAPI::Unit GetDepot();

    bool BuildBuilding(BWAPI::UnitType type, BWAPI::Unit skip = nullptr);
    bool BuildByChokePoint(BWAPI::UnitType type, BWAPI::Unit skip = nullptr);
    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetTotalSupply(bool inProgress = false);
    bool CanAfford(BWAPI::UnitType unit);

    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
}