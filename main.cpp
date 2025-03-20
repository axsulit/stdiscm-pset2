#include <iostream>
using namespace std;

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

	return 0;
}

