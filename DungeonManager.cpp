#include "DungeonManager.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <limits>
#include <stdexcept>

using namespace std;

mutex partyMutex;
mutex statusMutex;
vector<pair<string, int>> instanceStatus;

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
	cout << "Dungeon Instances Online: " << numInstances << endl;
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
	instanceStatus.resize(numInstances, { "EMPTY", 0 });

	cout << "\n===========================================" << endl;
	cout << "      *** DUNGEON RAID STATUS ***" << endl;
	cout << "===========================================\n" << endl;
	cout << "[Deploying adventurers into the dungeons...]\n" << endl;

	while (partiesInQueue > 0 && partyInstances.size() < numInstances) {
		lock_guard<mutex> lock(partyMutex);

		int availableSpots = numInstances - partyInstances.size();
		int partiesToStart = min(partiesInQueue, availableSpots);

		for (int i = 0; i < partiesToStart; i++) {
			int instanceID = partyInstances.size() + 1;
			partiesInQueue--;

			// Assign a random dungeon completion time
			int dungeonTime = minDungeonTime + (rand() % (maxDungeonTime - minDungeonTime + 1));

			// Update instance status before launching thread
			{
				lock_guard<mutex> lock(statusMutex);
				instanceStatus[instanceID - 1] = { "ACTIVE", dungeonTime };
			}

			// Flavorful dungeon entry log
			cout << "> Party " << instanceID << " enters \"The Taft Abyss\" (ETA: "
				<< dungeonTime << " sec)" << endl;

			// Start the dungeon in a new thread
			partyInstances.emplace_back(&DungeonManager::runDungeon, this, instanceID, dungeonTime);
		}

		// Print instance status only once
		printInstanceStatus();
		this_thread::sleep_for(chrono::seconds(1));
	}

	// Ensure all dungeon threads complete
	for (auto& t : partyInstances) {
		if (t.joinable()) {
			t.join();
		}
	}

	cout << "\n-------------------------------------------" << endl;
	cout << "     The adventurers return victorious! " << endl;
	cout << "===========================================\n" << endl;
}



void DungeonManager::runDungeon(int instanceID, int dungeonTime) {
	this_thread::sleep_for(chrono::seconds(dungeonTime)); // Simulate dungeon duration

	{
		lock_guard<mutex> lock(statusMutex);
		instanceStatus[instanceID - 1] = { "EMPTY", 0 };
	}

	// More immersive completion log
	cout << "\nParty " << instanceID << " has conquered the dungeon! " << endl;
}

void DungeonManager::printInstanceStatus() {
	lock_guard<mutex> lock(statusMutex);

	cout << "\n-------------------------------------------" << endl;
	cout << "       ACTIVE DUNGEON INSTANCES" << endl;
	cout << "-------------------------------------------" << endl;

	for (size_t i = 0; i < instanceStatus.size(); i++) {
		if (instanceStatus[i].first == "ACTIVE") {
			cout << "[Instance " << (i + 1) << "] Party " << (i + 1)
				<< " - Estimated completion: " << instanceStatus[i].second << " sec" << endl;
		}
		else {
			cout << "[Instance " << (i + 1) << "] [EMPTY]" << endl;
		}
	}

	cout << "\n-------------------------------------------" << endl;
}