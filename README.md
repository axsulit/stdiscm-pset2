# STDISCM S12 Problem Set 2: Synchronization
- SULIT, Anne Gabrielle M.
- [GitHub Repo](https://github.com/axsulit/stdiscm-pset2)

## Overview
The Dungeon Manager simulates an MMORPG dungeon queue system, managing dungeon instances and party formation. It ensures efficient use of resources, avoiding deadlock and starvation while handling party assignments to multiple dungeon instances concurrently.

## Key Features
- **Max Concurrent Instances**: Supports up to `n` dungeon instances running simultaneously.
- **Party Composition**: Each party consists of 1 Tank, 1 Healer, and 3 DPS.
- **Random Dungeon Time**: Dungeon completion times range randomly between `t1` and `t2` seconds.
- **Concurrency**: Utilizes multithreading to manage dungeon instances.

## Input
- `n`: Number of concurrent dungeon instances.
- `t`: Number of Tanks in the queue.
- `h`: Number of Healers in the queue.
- `d`: Number of DPS players in the queue.
- `t1`: Minimum dungeon completion time.
- `t2`: Maximum dungeon completion time.

## Output
- **Dungeon Status**: Shows which instances are "active" or "empty".
- **Final Summary**: Displays total parties served and the total time spent in each dungeon.


## How to Run
1. **Open the Solution**: Open the `.sln` file in Visual Studio.
2. **Run the Program**: Click **Local Windows Debugger** in Visual Studio to start the program.
3. **Configure Parameters**: Enter the number of instances, players in queue, and dungeon times.
4. **Party Formation**: The program automatically forms parties and assigns them to available instances.
5. **Execution**: The system runs dungeon instances concurrently and provides a final summary of parties served.

