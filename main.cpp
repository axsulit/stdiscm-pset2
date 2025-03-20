#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

mutex partyMutex;
mutex statusMutex;
vector<pair<string, int>> instanceStatus;

// Helper function to handle input
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

void printInstanceStatus() {
    lock_guard<mutex> lock(statusMutex);
    cout << "\nInstance Status:\n";
    for (size_t i = 0; i < instanceStatus.size(); i++) {
        if (instanceStatus[i].first == "active") {
            cout << "Instance " << (i + 1) << ": active (estimated completion time: "
                << instanceStatus[i].second << " seconds)" << endl;
        }
        else {
            cout << "Instance " << (i + 1) << ": empty" << endl;
        }
    }
}

void runDungeon(int instanceID, int minTime, int maxTime) {
    int dungeonTime = minTime + (rand() % (maxTime - minTime + 1)); // Random dungeon completion time

    {
        lock_guard<mutex> lock(statusMutex);
        instanceStatus[instanceID - 1] = { "active", dungeonTime }; // Store estimated time
    }

    this_thread::sleep_for(chrono::seconds(dungeonTime)); // Simulate dungeon duration

    {
        lock_guard<mutex> lock(statusMutex);
        instanceStatus[instanceID - 1] = { "empty", 0 }; // Mark as empty
    }

    printInstanceStatus();
}

int main()
{
    int numInstances = getInput("Number of party instances: ", "Invalid input! Please enter a valid number.", 1);

    int numTanks = getInput("Number of tanks: ", "Invalid input! Please enter a valid number.", 0);
    int numHealers = getInput("Number of healers: ", "Invalid input! Please enter a valid number.", 0);
    int numDPS = getInput("Number of DPS: ", "Invalid input! Please enter a valid number.", 0);

	if (numTanks < 1 || numHealers < 1 || numDPS < 3) {
        cout << "There are not enough players to start a party.";
		return -1;
	}

    int minTime = getInput("Minimum dungeon completion time: ", "Invalid input! Please enter a valid number.", 0);
    int maxTime = getInput("Maximum dungeon completion time: ", "Invalid input! Please enter a valid number.", 0);

	vector<thread> partyInstances;

	instanceStatus.resize(numInstances, { "empty", 0 }); // Initialize instance status

    while (numTanks >= 1 && numHealers >= 1 && numDPS >= 3) {
        int instanceID;
        int dungeonTime = minTime + (rand() % (maxTime - minTime + 1)); // Generate estimated time

        {
            lock_guard<mutex> lock(partyMutex);

            if (partyInstances.size() >= numInstances) {
                continue; // If all instances are full, keep waiting
            }

            numTanks--; numHealers--; numDPS -= 3;
            instanceID = partyInstances.size() + 1;

            {
                lock_guard<mutex> lock(statusMutex);
                instanceStatus[instanceID - 1] = { "active", dungeonTime }; // Store estimated time
            }

            cout << endl << "A new party has entered Instance " << instanceID << "." << endl;
        }

        partyInstances.emplace_back(runDungeon, instanceID, minTime, maxTime);

        printInstanceStatus(); // Print the updated status
    }

    for (auto& t : partyInstances) if (t.joinable()) t.join();

	return 0;
}

