#include <iostream>
#include <thread>
#include <chrono>

#include "DungeonManager.h"

using namespace std;

int main()
{
	DungeonManager dm;

	// Get user input
	dm.setDungeonParameters();
	this_thread::sleep_for(chrono::seconds(1));
	system("cls");

	dm.printDungeonParameters();
	this_thread::sleep_for(chrono::seconds(1));

	// Exit if no parties are formed
	if (dm.createPartyQueue() == -1) {
		return -1;
	}

	dm.startDungeonInstances();

	return 0;
}