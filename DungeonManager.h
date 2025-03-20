#pragma once

using namespace std;

class DungeonManager
{
public:
	DungeonManager();
	void setDungeonParameters();
	void printDungeonParameters();
	int createPartyQueue();
	void startDungeonInstances();

private:
	int numInstances;
	int numTanks;
	int numHealers;
	int numDPS;
	int minDungeonTime; // TODO: convert to time
	int maxDungeonTime; // TODO: convert to time
	int partiesInQueue;

	void runDungeon(int instanceID, int dungeonTime);
	void printInstanceStatus();
	int activeDungeons();
};

