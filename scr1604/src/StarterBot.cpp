#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <assert.h>
#include <cassert>
bool analyzed;
bool analysis_just_finished;

StarterBot::StarterBot()
{

}

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
/*
    try
    {
        theMap.Initialize(BWAPI::BroodwarPtr);
        //theMap.EnableAutomaticPathAnalysis();
        //bool startingLocationsOK = theMap.FindBasesForStartingLocations();
        //assert(startingLocationsOK);
    }
    catch (const std::exception& e)
    {
        BWAPI::Broodwar << "EXCEPTION: " << e.what() << std::endl;
    }

    // Call MapTools OnStart
    m_mapTools.onStart();
    */
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();
    //Write group member names on screen
    BWAPI::Broodwar->drawTextScreen(10, 25, "Rohan Rumjansing  Jabulani Mabena");


    checkBuildOrder();

    if (!m_enemy_found) {
        scout();
        m_enemy_found = enemyFound();
        std::cout << m_enemy_found;
    }

    checkUnits();
    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    trainZergling();

    trainHydralisk();
    researchLurker();
    

    if (m_enemy_found && !m_rushed)
    {
        BWAPI::UnitType zergling = BWAPI::UnitTypes::Zerg_Zergling;
        if (Tools::CountUnitsOfType(zergling, BWAPI::Broodwar->self()->getUnits()) >= 6)
        {
            //auto chokepoint = theMap.GetArea((BWAPI::Position)BWAPI::Broodwar->self()->getStartLocation());
            rush();
        }
    }
    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
    //We first get the number of workers and check if it is greater than 8
    //Then we loop through all units and filter all the workers using an if statement
    //Then we stop gathering minerals 
    /*BWAPI::Position startPos(BWAPI::Broodwar->self()->getStartLocation());
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();*/
    /*if (workersOwned == 8)
    {
        int count = 0;
        int id = 0;
        BWAPI::Unit un;
        BWAPI::Position p;
        for (auto& unit : myUnits)
        {
            if (unit->getType().isWorker())
            {
                unit->stop();
                int x = startPos.x + (24 * count);
                int y = startPos.y - 24;
                unit->move(BWAPI::Position(x,y));
                count++;

            }
        }
    }*/



}


void StarterBot::checkBuildOrder()
{
    const int totalSupply = Tools::GetTotalSupply(true);
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    if (totalSupply == 18 || !m_rushed)
    {
        if (!m_pitBuilt)
        {
            buildSpawnPit();

        }
        else if (unusedSupply >= 10)
        {
            //buildOverlord();
            trainZergling();

        }
        else if (unusedSupply >= 4)
        {
            m_drones_wanted = 5;
        }
        else if (unusedSupply == 0 && !m_overlordBuilt || m_rushed)
        {
            m_overlordWanted = 2;
            buildOverlord();
        }
    }
    else if (!m_hydraDenBuilt && m_rushed && totalSupply > 18)
    {
        if (!m_hatcheryBuilt)
        {
            //std::cout << "Test";
            buildHatchery();
            m_defenseCount = 6;
        }
        else if (!m_extractorBuilt && m_hatcheryBuilt)
        {
            buildExtractor();
            sendWorkersToExtractor();
        }
        else if (!m_hydraDenBuilt && m_extractorBuilt)
        {

            buildHydraliskDen();
            //buildLair();
            buildOverlord();
            m_drones_wanted = 9;
            m_overlordWanted = 3;
            m_hydralisk = 5;
            m_defenseCount = 10;
        }
        else if (m_lairBuilt)
        {
            m_defenseCount += unusedSupply / 2;
            trainZergling();
        }
    }
    else if (m_hydraDenBuilt)
    {
        if (!m_lairBuilt)
        {
            buildLair();

        }

    }
    /*std::cout << totalSupply;
    switch (totalSupply)
    {
        case 18:
        {
            switch (unusedSupply)
            {
                case 10:
                {
                    buildSpawnPit();
                    trainZergling(m_rushCount);
                    break;
                }
            }
            break;
        }
        case 34:
        {
            m_drones_wanted = 6;
            switch (unusedSupply)
            {
                case 16:
                {
                    buildHatchery();
                    break;
                }
                case 18:
                {
                    trainZergling(m_defenseCount);
                    break;
                }
                case 10:
                {
                    buildExtractor();
                    m_drones_wanted += 2;
                    break;
                }


            }
        }
    }*/
}


void StarterBot::rush()
{
    const BWAPI::UnitType zergling = BWAPI::UnitTypes::Zerg_Zergling;
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->getType() == zergling && unit->isIdle())
        {
            //std::cout << m_target;
            //Tools::SmartRightClick(unit, m_target);
            //unit->rightClick(m_target);
            unit->attack(m_target_position);
        }
    }
    m_rushed = true;
}

void StarterBot::scout()
{
    if (!m_scout)
    {
        m_scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
    }
    auto& startLocations = BWAPI::Broodwar->getStartLocations();
    for (BWAPI::TilePosition tp : startLocations)
    {
        if (BWAPI::Broodwar->isExplored(tp)) { continue; }
        BWAPI::Position pos(tp);
        BWAPI::Broodwar->drawCircleMap(pos, 32, BWAPI::Colors::Red, true);
        m_scout->move(pos);
        break;
    }

}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own

    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();



    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            if (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Build || unit == m_scout) { continue; }
            if (m_toExtracor)
            {
                //BWAPI::Unit closestGeyser = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getGeysers());

                // If a valid mineral was found, right click it with the unit in order to start harvesting
                BWAPI::Unit extractor = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Extractor);
                if (extractor) { Tools::SmartRightClick(unit, extractor); }
                m_toExtracor = false;
            }
            else
            {
                // Get the closest mineral to this worker unit
                BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

                // If a valid mineral was found, right click it with the unit in order to start harvesting
                if (closestMineral) { Tools::SmartRightClick(unit, closestMineral); }
                if (m_extractorBuilt) { m_toExtracor = true; }
            }

        }
    }

}

void StarterBot::sendWorkersToExtractor()
{
    if (!m_extractorBuilt) { return; }
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();


    int count = 0;
    for (auto& unit : myUnits)
    {
        if (unit == m_scout) { continue; }
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (count == 2) break;
        if (unit->getType().isWorker())
        {
            count++;

            BWAPI::Unit extractor = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Extractor);
            if (extractor) { Tools::SmartRightClick(unit, extractor); }
        }
    }
}

bool StarterBot::enemyFound()
{
    const BWAPI::Unitset& enemyUnits = BWAPI::Broodwar->enemy()->getUnits();



    for (auto& unit : enemyUnits)
    {
        if (unit->getType().isResourceDepot())
        {
            m_target = unit;
            m_target_position = unit->getPosition();
            m_enemy_found = true;
            m_scout->stop();
            m_scout = nullptr;
            break;
        }
    }
    return m_enemy_found;
}

void StarterBot::enemyInRange(BWAPI::Unit unit)
{
    //const BWAPI::Unitset& enemies = BWAPI::Broodwar->enemy()->getUnits();
    const BWAPI::Unitset& units = BWAPI::Broodwar->getUnitsInRadius(unit->getPosition(), 20);
    for (auto& u : units)
    {
        if (unit->getPlayer()->isEnemy(u->getPlayer()) && !unit->isAttacking())
        {
            unit->attack(u);
            break;
        }
    }
}




// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    if (!Tools::CanAfford(workerType)) { return; }
    const int workersWanted = m_drones_wanted;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }

    }
}

void StarterBot::trainZergling()
{
    if (m_rushed && !m_overlordBuilt) { return; }
    const BWAPI::UnitType zergling = BWAPI::UnitTypes::Zerg_Zergling;
    const int zerglingsOwned = Tools::CountUnitsOfType(zergling, BWAPI::Broodwar->self()->getUnits());
    int zergsWanted = 0;

    if (!m_rushed)
    {
        zergsWanted = m_rushZergs;
    }
    else
    {
        zergsWanted = m_rushZergs + m_defenseZergs;
    }
    if (zerglingsOwned < zergsWanted)
    {
        bool training = trainAdditionalUnit(zergling);
    }
    /*for (auto i = 0; i < count; i++)
    {
        bool training = trainAdditionalUnit(zergling);
    }*/
}

void StarterBot::trainHydralisk()
{
    const BWAPI::UnitType hydralisk = BWAPI::UnitTypes::Zerg_Hydralisk;
    const int hydrasOwned = Tools::CountUnitsOfType(hydralisk, BWAPI::Broodwar->self()->getUnits());

    if (hydrasOwned < m_hydralisk)
    {
        bool training = trainAdditionalUnit(hydralisk);
    }
}

bool StarterBot::trainAdditionalUnit(BWAPI::UnitType type)
{
    BWAPI::UnitType builderType = type.whatBuilds().first;
    if (!Tools::CanAfford(type)) { return false; }
    BWAPI::Unit builder = Tools::GetUnitOfType(builderType, m_scout);
    if (!builder) { return false; }
    return builder->train(type);
}
void StarterBot::buildOverlord()
{
    ////if (Tools::GetTotalSupply(true) > 18) return;
    //const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();
    ////if (unusedSupply >= 2) { return; }
    //for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits())
    //{
    //    if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && u->canTrain(BWAPI::UnitTypes::Zerg_Overlord) && ovie_making == false)
    //    {
    //        u->train(BWAPI::UnitTypes::Zerg_Overlord);
    //        ovie_making = true;
    //    }
    //}

    const BWAPI::UnitType overlord = BWAPI::UnitTypes::Zerg_Overlord;
    const int ownedOverlords = Tools::CountUnitsOfType(overlord, BWAPI::Broodwar->self()->getUnits());
    if (ownedOverlords < m_overlordWanted && !m_overlordBuilt)
    {
        if (!Tools::CanAfford(overlord)) { return; }
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits())
        {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && u->canTrain(BWAPI::UnitTypes::Zerg_Overlord))
            {
                bool training = u->train(BWAPI::UnitTypes::Zerg_Overlord);
                if (training)
                {
                    m_overlordBuilt = true;
                    BWAPI::Broodwar->printf("Started training %s", overlord.getName().c_str());
                    break;
                }


            }
        }
        /*const bool startedBuilding = Tools::BuildBuilding(overlord, m_scout);
        if (startedBuilding)
        {
            m_pitBuilt = true;
            BWAPI::Broodwar->printf("Started training %s", overlord.getName().c_str());
        }*/
    }
}

void StarterBot::buildSpawnPit()
{
    const BWAPI::UnitType spawnpit = BWAPI::UnitTypes::Zerg_Spawning_Pool;
    const int ownedPits = Tools::CountUnitsOfType(spawnpit, BWAPI::Broodwar->self()->getUnits());
    if (ownedPits < m_pitsWanted)
    {
        if (!Tools::CanAfford(spawnpit)) { return; }
        const bool startedBuilding = Tools::BuildBuilding(spawnpit, m_scout);
        if (startedBuilding)
        {
            m_pitBuilt = true;
            BWAPI::Broodwar->printf("Started Building %s", spawnpit.getName().c_str());
        }
    }
}

void StarterBot::buildHatchery()
{
    const BWAPI::UnitType hatchery = BWAPI::UnitTypes::Zerg_Hatchery;
    const int hatOwned = Tools::CountUnitsOfType(hatchery, BWAPI::Broodwar->self()->getUnits());
    if (hatOwned < m_hatWanted)
    {
        if (!Tools::CanAfford(hatchery)) { return; }
        const bool startedBuilding = Tools::BuildBuilding(hatchery, m_scout);
        if (startedBuilding)
        {
            //m_hatcheryBuilt = true;
            BWAPI::Broodwar->printf("Started Building %s", hatchery.getName().c_str());
        }
    }
}

void StarterBot::buildExtractor()
{
    const BWAPI::UnitType extractor = BWAPI::UnitTypes::Zerg_Extractor;
    const int exOwned = Tools::CountUnitsOfType(extractor, BWAPI::Broodwar->self()->getUnits());
    if (exOwned < m_exWanted)
    {
        if (!Tools::CanAfford(extractor)) { return; }
        const bool startedBuilding = Tools::BuildBuilding(extractor);
        if (startedBuilding)
        {
            //m_extractorBuilt = true;
            BWAPI::Broodwar->printf("Started Building %s", extractor.getName().c_str());
        }
    }
}

void StarterBot::buildLair()
{
    const BWAPI::UnitType lair = BWAPI::UnitTypes::Zerg_Lair;
    const int lairOwned = Tools::CountUnitsOfType(lair, BWAPI::Broodwar->self()->getUnits());
    if (lairOwned < m_lairWanted)
    {
        if (!Tools::CanAfford(lair)) { return; }
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits())
        {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
            {
                bool training = u->build(lair);

                if (training)
                {
                    //m_lairBuilt = true; 
                    BWAPI::Broodwar->printf("Started training %s", lair.getName().c_str());
                    break;
                }


            }

            //const bool startedBuilding = Tools::BuildBuilding(lair);
            //if (startedBuilding)
            //{
            //    //m_lairBuilt = true;
            //    BWAPI::Broodwar->printf("Started Building %s", lair.getName().c_str());
        }
    }

}

void StarterBot::buildHydraliskDen()
{
    const BWAPI::UnitType den = BWAPI::UnitTypes::Zerg_Hydralisk_Den;
    const int denOwned = Tools::CountUnitsOfType(den, BWAPI::Broodwar->self()->getUnits());
    if (denOwned < m_hydraDenWanted)
    {
        if (!Tools::CanAfford(den)) { return; }
        //bool startedBuilding = Tools::BuildBuilding(den,m_scout);
        //if (startedBuilding)
        //{
        //    //m_hydraDenBuilt = true;
        //    BWAPI::Broodwar->printf("Started Building %s", den.getName().c_str());
        //}
        if (m_scout && m_building && m_scout->isConstructing()) { return; }
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits())
        {
            if (m_scout && u != m_scout) { continue; }
            if (u->getType() == BWAPI::UnitTypes::Zerg_Drone)
            {
                std::cout << "drone found \n";
                BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();
                int maxBuildRange = 64;
                bool buildingOnCreep = den.requiresCreep();
                BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(den, desiredPos, maxBuildRange, buildingOnCreep);
                bool training = u->build(den, buildPos);
                m_scout = u;
                //if (u->isMorphing()) { m_building = true; }
                if (u->isConstructing() && !m_building)
                {
                    BWAPI::Broodwar->printf("Started building %s", den.getName().c_str());
                    m_building = true;
                    break;
                }


            }
        }
    }
}

void StarterBot::researchLurker()
{
    if (!m_lairBuilt) { return; }
    
    BWAPI::UnitType denType = BWAPI::UnitTypes::Zerg_Hydralisk_Den;
    BWAPI::Unit den = Tools::GetUnitOfType(denType);
    if (den == nullptr) { return; }
    BWAPI::TechType lurkertech = BWAPI::TechTypes::Lurker_Aspect;
    if (!(den->isResearching()))
    {
        bool researching = den->research(lurkertech);
        if (researching)
        {
            BWAPI::Broodwar->printf("Started Researching %s", lurkertech.getName().c_str());
        }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }

}

void StarterBot::checkUnits()
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        onUnitComplete(unit);
        if (unit->canAttack())
        {
            enemyInRange(unit);
        }
    }

}

// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{

    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
    try
    {
        //if (unit->getType().isMineralField())    theMap.OnMineralDestroyed(unit);
        //else if (unit->getType().isSpecialBuilding()) theMap.OnStaticBuildingDestroyed(unit);
    }
    catch (const std::exception& e)
    {
        BWAPI::Broodwar << "EXCEPTION: " << e.what() << std::endl;
    }
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
    /*if (BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Zerg_Overlord) > ovie_count)
    {
        ovie_count = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Zerg_Overlord);
        ovie_making = false;
    }
    */
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text)
{
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{

}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor)
    {
        int count = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits(), true);
        if (count != m_exWanted) { return; }
        m_extractorBuilt = true;
        sendWorkersToExtractor();
    }
    else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
    {
        int count = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
        if (count != m_hatWanted) { return; }
        m_hatcheryBuilt = true;
    }
    else if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair)
    {
        int count = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
        if (count != m_lairWanted) { return; }
        m_lairBuilt = true;
    }
    else if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den)
    {
        int count = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
        if (count != m_hydraDenWanted) { return; }
        m_hydraDenBuilt = true;
    }
    else if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)
    {
        int count = Tools::CountUnitsOfType(unit->getType(), BWAPI::Broodwar->self()->getUnits());
        if (count != m_overlordWanted) { return; }
        m_overlordBuilt = false;
    }
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{

}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{

}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{

}