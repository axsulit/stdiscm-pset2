#pragma once

using namespace std;

class DungeonManager
{
public:
    /**
     * @brief Constructor to initialize the DungeonManager with default values.
     */
    DungeonManager();

    /**
     * @brief Sets up the parameters for the dungeon manager, including number of instances, tanks, healers, and DPS.
     * Prompts the user for input to define dungeon configurations.
     */
    void setDungeonParameters();

    /**
     * @brief Prints the current dungeon configuration parameters.
     * Displays the number of dungeons, tanks, healers, DPS, and dungeon clear time range.
     */
    void printDungeonParameters();

    /**
     * @brief Creates a party queue by assembling parties with available adventurers.
     * The party is composed of tanks, healers, and DPS.
     *
     * @return int - Returns 0 if parties were assembled successfully, or -1 if not enough adventurers are available.
     */
    int createPartyQueue();

    /**
     * @brief Starts the dungeon instances and assigns parties to available dungeons.
     * Manages the simulation of dungeon raids in separate threads, and ensures the progress of the dungeons.
     */
    void startDungeonInstances();

    /**
     * @brief Prints the final summary of the dungeon raids.
     * Displays the total number of parties served, total time served per dungeon, and overall statistics.
     */
    void printFinalSummary();

private:
    int numInstances;         ///< The number of dungeon instances that can run simultaneously.
    int numTanks;             ///< The number of tanks available in the queue.
    int numHealers;           ///< The number of healers available in the queue.
    int numDPS;               ///< The number of DPS available in the queue.
    int minDungeonTime;       ///< The minimum dungeon completion time in seconds.
    int maxDungeonTime;       ///< The maximum dungeon completion time in seconds.
    int partiesInQueue;       ///< The number of parties waiting in the queue.
    int totalCompletedParties;///< The total number of completed parties.
    int totalTimeServed;      ///< The total time served by all dungeons.

    /**
     * @brief Simulates the completion of a dungeon for a party in a specific dungeon instance.
     *
     * @param instanceID The ID of the dungeon instance.
     * @param dungeonTime The time taken for the dungeon completion in seconds.
     */
    void runDungeon(int instanceID, int dungeonTime);

    /**
     * @brief Prints the current status of each dungeon instance, showing whether the dungeon is active or empty.
     */
    void printInstanceStatus();

    /**
     * @brief Returns the number of active dungeons.
     *
     * @return int - The number of active dungeons.
     */
    int activeDungeons();
};
