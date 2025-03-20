#include "DungeonManager.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <algorithm>

using namespace std;

mutex partyMutex;
mutex statusMutex;
vector<tuple<string, int, int>> instanceStatus; // Status, Time, Party ID


DungeonManager::DungeonManager() : numInstances(-1), numTanks(-1), numHealers(-1), numDPS(-1), minDungeonTime(-1), maxDungeonTime(-1), partiesInQueue(-1) {}

int getInput(const string& prompt, const string& errorMessage, int condition) {
	int option;
	while (true) {
		try {
			cout << prompt;
			cin >> option;
			if (cin.fail()) {
				throw invalid_argument("Invalid input!");
			}
			if (option < condition) {
				throw invalid_argument("Invalid input! Please enter a positive number.");
			}
			return option;
		}
		catch (const exception& e) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discards invalid input
			cerr << errorMessage << endl;
		}
	}
}

void DungeonManager::setDungeonParameters() {
	cout << "=== Welcome to the Dungeon Queue System ===" << endl << endl;

	numInstances = getInput("How many dungeon instances can run at the same time?\n > ",
		"Invalid choice. Please enter a valid number.", 1);

	numTanks = getInput("How many Tanks are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number.", 0);

	numHealers = getInput("How many Healers are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number.", 0);

	numDPS = getInput("How many DPS are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number.", 0);

	minDungeonTime = getInput("What is the fastest possible dungeon clear time (in seconds)?\n > ",
		"Invalid input. Enter a valid time.", 1);

	maxDungeonTime = getInput("What is the longest a dungeon can take to complete (in seconds)?\n > ",
		"Invalid input. The maximum time must be at least the minimum time.",
		minDungeonTime);

	cout << endl << "The dungeon gates are now open. Parties will be formed and deployed shortly..." << endl;
}

void DungeonManager::printDungeonParameters() {
	cout << "====== Dungeon Configuration ======" << endl;
	cout << "Dungeons Online: " << numInstances << endl;
	cout << "Tanks in Queue: " << numTanks << endl;
	cout << "Healers in Queue: " << numHealers << endl;
	cout << "DPS in Queue: " << numDPS << endl;
	cout << "Clear Time: " << minDungeonTime << " seconds - " << maxDungeonTime << " seconds" << endl;
	cout << "===================================" << endl << endl;
}

int DungeonManager::createPartyQueue() {
	partiesInQueue = min({ numTanks, numHealers, numDPS / 3 });

	if (partiesInQueue > 0) {
		cout << partiesInQueue << " adventuring parties have been assembled and stand ready to enter the dungeons." << endl << endl;
		cout << "===================================" << endl << endl;
		return 0;
	}
	else {
		cout << "The heroes wait... but there are not enough adventurers to form a full party." << endl << endl;
		cout << "===================================" << endl << endl;
		return -1;
	}
}

void DungeonManager::startDungeonInstances() {
	vector<thread> partyInstances;
	instanceStatus.resize(numInstances, { "EMPTY", 0, -1 });

	cout << "\n===================================" << endl;
	cout << "    *** DUNGEON RAID STATUS ***" << endl;
	cout << "===================================\n" << endl;
	cout << "[Deploying adventurers into the dungeons...]\n" << endl;

	int partyCounter = 1; 

	while (partiesInQueue > 0) {
		lock_guard<mutex> lock(partyMutex);

		for (int i = 0; i < numInstances && partiesInQueue > 0; i++) {
			if (get<0>(instanceStatus[i]) == "EMPTY") { 
				int instanceID = i + 1;
				partiesInQueue--;

				// Assign random dungeon completion time
				int dungeonTime = minDungeonTime + (rand() % (maxDungeonTime - minDungeonTime + 1));

				//  Update instanceStatus
				{
					lock_guard<mutex> lock(statusMutex);
					instanceStatus[i] = { "ACTIVE", dungeonTime, partyCounter };
					//cout << "> Party " << partyCounter << " enters... (ETA: " << dungeonTime << " sec)" << endl;
				}

				// Start the dungeon in a new thread
				partyInstances.emplace_back(&DungeonManager::runDungeon, this, instanceID, dungeonTime);
				partyCounter++;  
			}
		}

		//printInstanceStatus();
		this_thread::sleep_for(chrono::seconds(1));
	}

	//  Join finished threads before removing them
	for (auto& t : partyInstances) {
		if (t.joinable()) {
			t.join();
		}
	}

	cout << "\n-------------------------------------------" << endl;
	cout << "   The adventurers return victorious!" << endl;
	cout << "===========================================\n" << endl;
}


void DungeonManager::runDungeon(int instanceID, int dungeonTime) {
	this_thread::sleep_for(chrono::seconds(dungeonTime)); // Simulate dungeon duration
	int partyID; 

	printInstanceStatus();

	{
		lock_guard<mutex> lock(statusMutex);
		partyID = get<2>(instanceStatus[instanceID - 1]);  
		instanceStatus[instanceID - 1] = { "EMPTY", 0, -1 }; 
		cout << "> Party " << partyID << " has conquered the dungeon in " << dungeonTime << " seconds!" << endl;
	}
}

void DungeonManager::printInstanceStatus() {
	lock_guard<mutex> lock(statusMutex);

	cout << "\n-------------------------------------------" << endl;
	cout << "       ONLINE DUNGEON INSTANCES" << endl;  
	cout << "-------------------------------------------" << endl;

	for (size_t i = 0; i < instanceStatus.size(); i++) {
		if (get<0>(instanceStatus[i]) == "ACTIVE") { 
			cout << "[Dungeon " << (i + 1) << "] ACTIVE - Party " << get<2>(instanceStatus[i]) << endl;
		}
		else {
			cout << "[Dungeon " << (i + 1) << "] EMPTY" << endl;
		}
	}

	cout << "-------------------------------------------\n" << endl;
}
