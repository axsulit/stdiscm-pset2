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

mutex statusMutex;
mutex printMutex;
vector<tuple<string, int, int>> instanceStatus; // Status, Time, Party ID
vector<int> partiesServedPerInstance;
vector<int> totalTimePerInstance;

DungeonManager::DungeonManager() : numInstances(-1), numTanks(-1), numHealers(-1), numDPS(-1), minDungeonTime(-1), maxDungeonTime(-1), partiesInQueue(-1), totalCompletedParties(0), totalTimeServed(0) {}

int DungeonManager::activeDungeons() {
	lock_guard<mutex> lock(statusMutex);
	return count_if(instanceStatus.begin(), instanceStatus.end(),
		[](const tuple<string, int, int>& s) { return get<0>(s) == "ACTIVE"; });
}

void DungeonManager::startDungeonInstances() {
	vector<thread> dungeonThreads;
	instanceStatus.resize(numInstances, { "EMPTY", 0, -1 });

	// Initialize per-instance tracking vectors
	partiesServedPerInstance.resize(numInstances, 0);
	totalTimePerInstance.resize(numInstances, 0);

	cout << "    *** DUNGEON RAID STATUS ***" << endl;
	cout << "===================================\n" << endl;
	cout << "[Deploying adventurers...]\n" << endl;

	int partyCounter = 1;

	while (partiesInQueue > 0 || activeDungeons() > 0) {
		for (int i = 0; i < numInstances && partiesInQueue > 0; i++) {

			lock_guard<mutex> lock(statusMutex);
			if (get<0>(instanceStatus[i]) == "EMPTY") {
				int instanceID = i + 1;
				partiesInQueue--;

				// Assign random dungeon completion time
				int dungeonTime = minDungeonTime + (rand() % (maxDungeonTime - minDungeonTime + 1));

				instanceStatus[i] = { "ACTIVE", dungeonTime, partyCounter };

				cout << "> Party " << partyCounter << " enters dungeon " << instanceID << "..." << endl;
				printInstanceStatus();

				// Start the dungeon in a new thread
				dungeonThreads.emplace_back(&DungeonManager::runDungeon, this, instanceID, dungeonTime);
				partyCounter++;
			}
		}

		this_thread::sleep_for(chrono::milliseconds(500));
	}

	for (auto& t : dungeonThreads) {
		if (t.joinable()) {
			t.join();
		}
	}

	// Check again for active dungeons
	while (activeDungeons() > 0) {
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	cout << "\n  The adventurers are victorious!" << endl << endl;
	cout << "===================================\n" << endl;

	printFinalSummary();
}

void DungeonManager::runDungeon(int instanceID, int dungeonTime) {
	this_thread::sleep_for(chrono::seconds(dungeonTime)); // Simulate dungeon duration
	int partyID;

	{
		lock_guard<mutex> lock(statusMutex);
		partyID = get<2>(instanceStatus[instanceID - 1]);
		instanceStatus[instanceID - 1] = { "EMPTY", 0, -1 };

		// Track total served per instance
		totalCompletedParties++;
		totalTimeServed += dungeonTime;

		partiesServedPerInstance[instanceID - 1]++;
		totalTimePerInstance[instanceID - 1] += dungeonTime;
	}

	{
		lock_guard<mutex> lock(printMutex);
		cout << "> Party " << partyID << " finished dungeon " << instanceID << " in " << dungeonTime << " secs!" << endl;
		printInstanceStatus();
	}
}

void DungeonManager::printFinalSummary() {
	cout << "\n===================================" << endl;
	cout << "      DUNGEON RAID SUMMARY" << endl;
	cout << "===================================" << endl;

	for (size_t i = 0; i < partiesServedPerInstance.size(); i++) {
		cout << "[Dungeon " << (i + 1) << "]" << endl;
		cout << "  - Parties Served: " << partiesServedPerInstance[i] << endl;
		cout << "  - Total Time Served: " << totalTimePerInstance[i] << " seconds" << endl;
		cout << "-----------------------------------" << endl;
	}

	// General Summary
	cout << "OVERALL TOTAL" << endl;
	cout << "Total Parties Served: " << totalCompletedParties << endl;
	cout << "Total Time Served: " << totalTimeServed << " seconds" << endl;
	cout << "===================================" << endl;
}

void DungeonManager::printInstanceStatus() {
	cout << "-----------------------------------" << endl;
	for (size_t i = 0; i < instanceStatus.size(); i++) {
		if (get<0>(instanceStatus[i]) == "ACTIVE") {
			cout << "[Dungeon " << (i + 1) << "] ACTIVE - Party " << get<2>(instanceStatus[i]) << endl;
		}
		else {
			cout << "[Dungeon " << (i + 1) << "] EMPTY" << endl;
		}
	}
	cout << "-----------------------------------" << endl;
}

#include <cctype>  // For isdigit

int getInput(const string& prompt, const string& errorMessage, int condition) {
	string input;
	while (true) {
		try {
			cout << prompt;
			cin >> input;  // Read input as string

			// Check if the input contains only digits
			if (input.empty() || !all_of(input.begin(), input.end(), ::isdigit)) {
				throw invalid_argument("Invalid input! Please enter a valid number.");
			}

			// Convert string to integer using stringstream
			stringstream ss(input);
			int option;
			ss >> option;

			// Check if the input is less than the required condition
			if (option < condition) {
				throw invalid_argument("Invalid input! Please enter a number >= " + to_string(condition));
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
		"Invalid choice. Please enter a valid number >= 1.", 1);

	numTanks = getInput("How many Tanks are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number >= 0.", 0);

	numHealers = getInput("How many Healers are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number >= 0.", 0);

	numDPS = getInput("How many DPS are waiting in the queue?\n > ",
		"Invalid choice. Please enter a valid number >=0.", 0);

	minDungeonTime = getInput("What is the fastest possible dungeon clear time (in seconds)?\n > ",
		"Invalid input. Enter a valid time >= 0.", 0);

	maxDungeonTime = getInput("What is the longest a dungeon can take to complete (in seconds)?\n > ",
		"Invalid input. The maximum time must be at least the minimum time.",
		minDungeonTime);

	cout << endl << "The dungeon gates are now open. Parties will be formed and deployed shortly..." << endl;
}

void DungeonManager::printDungeonParameters() {
	cout << "====== Dungeon Configuration ======" << endl;
	cout << "Dungeons Online (n): " << numInstances << endl;
	cout << "Tanks in Queue (t): " << numTanks << endl;
	cout << "Healers in Queue (h): " << numHealers << endl;
	cout << "DPS in Queue (d): " << numDPS << endl;
	if (minDungeonTime == maxDungeonTime) {
		cout << "Clear Time (t1,t2): " << minDungeonTime << " seconds" << endl;
	}
	else
		cout << "Clear Time (t1,t2): " << minDungeonTime << " seconds - " << maxDungeonTime << " seconds" << endl;
	cout << "===================================" << endl << endl;
}

int DungeonManager::createPartyQueue() {
	partiesInQueue = min({ numTanks, numHealers, numDPS / 3 });

	//leftover tanks, healers, dps
	numTanks -= partiesInQueue;
	numHealers -= partiesInQueue;
	numDPS -= (partiesInQueue * 3);

	if (partiesInQueue > 0) {
		cout << partiesInQueue << " parties have been assembled!" << endl << endl;

		cout << "=== Leftover Players ===" << endl;
		cout << "Unassigned Tanks: " << numTanks << endl;
		cout << "Unassigned Healers: " << numHealers << endl;
		cout << "Unassigned DPS: " << numDPS << endl << endl;

		cout << "===================================" << endl;
		return 0;
	}
	else {
		cout << "The heroes wait... but there are not enough adventurers to form a full party." << endl << endl;
		cout << "===================================" << endl << endl;
		return -1;
	}
}