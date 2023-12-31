#include "QLearningAgent.h"
#include <utility> // For std::pair
#include <cstdlib> // For std::rand, std::srand
#include <ctime> // For std::time
#include <algorithm> // For std::max_element
#include "Agent.h"

// Assuming these are constant for the maze
const int GOAL_ROW = 12; // 21st row in 0-based indexing
const int GOAL_COL = 6; // 21st column in 0-based indexing


QLearningAgent::QLearningAgent(int row, int col) : Agent(row, col) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for randomness
    // Initialize Q-table to 0
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 4; k++) {
                Q[i][j][k] = 0;
            }
        }
    }
    stepsTaken = 0;
    startingPosition = std::make_pair(row, col);
}

int QLearningAgent::chooseAction(const Maze &maze) {
    if (!validMoves.empty()) {
        int chosenIndex = std::rand() % validMoves.size();
        int chosenAction = validMoves[chosenIndex].second;  // Extract the action
        return chosenAction;
    } else {
        // Move backwards or choose a default action
        return (lastAction + 2) % 4;  // Example of moving backward
    }
}








void QLearningAgent::updateQValues(int action, int reward, int newRow, int newCol) {
    // Find the maximum Q-value for the new state
    double maxQNew = *max_element(Q[newRow][newCol], Q[newRow][newCol] + 4);

    // Update Q-value using the Q-learning formula
    Q[position.first][position.second][action] += ALPHA * (reward + GAMMA * maxQNew - Q[position.first][position.second][action]);
}

int QLearningAgent::calculateReward(const Maze &maze, int row, int col) {
    int cellValue = maze.at(row, col);

    switch (cellValue) {
        case 0: // Empty space
            return STEP_REWARD;
        case 1: // Wall
            return WALL_PENALTY;
        case 2: // Goggles
            return GOGGLES_REWARD;
        case 3: // Speed potion
            return SPEED_POTION_REWARD;
        case 4: // Fog
            return FOG_PENALTY;
        case 5: // Slowpoke potion
            return SLOWPOKE_POTION_PENALTY;
        case 6: // Goal
            return GOAL_REWARD;
        default:
            return 0; // Default case, if needed
    }
}

void QLearningAgent::move(const Maze &maze) {
    int action = chooseAction(maze); // Choose action based on Q-values and epsilon-greedy strategy
    std::pair<int, int> newPosition = getNextPosition(maze, position, action, lastAction);  // Pass maze and lastAction
    // ... rest of the method ...
    // Calculate new position based on action
    
    int calcRow = position.first;
    int calcCol = position.second;
    // std::cout << "Attempting calc to: (" << calcRow << ", " << calcCol << ")" << std::endl;
    // Check for boundaries and walls
    if (isValidMove(maze, newPosition)) {
        // Update position if valid move
        position = newPosition;
        setPosition(newPosition.first, newPosition.second);
    }

    int reward = calculateReward(maze, position.first, position.second);
    updateQValues(action, reward, position.first, position.second);

    // Check for max steps
    if (++stepsTaken >= MAX_STEPS) {
        reset(); // Reset agent position and step count
    }
}

std::vector<std::pair<std::pair<int, int>, int>> QLearningAgent::getNextPosition(const Maze &maze, std::pair<int, int> currentPosition) {
    std::vector<int> possibleActions = {0, 1, 2, 3}; // All possible actions: Up, Right, Down, Left
    validMoves.clear(); // Clear previous valid moves

    std::cout << "Evaluating possible moves from (" << currentPosition.first << ", " << currentPosition.second << "):" << std::endl;
    for (int action : possibleActions) {
        std::pair<int, int> newPosition = calculateNewPosition(currentPosition, action);
        if (isValidMove(maze, newPosition)) {
            validMoves.push_back(std::make_pair(newPosition, action));
            std::cout << "Valid move: (" << newPosition.first << ", " << newPosition.second << ") with action " << action << std::endl;
        }
    }

    return validMoves; // Correctly placed return statement
}




// Helper function to calculate new position based on an action
std::pair<int, int> QLearningAgent::calculateNewPosition(std::pair<int, int> currentPosition, int action) {
    int newRow = currentPosition.first;
    int newCol = currentPosition.second;

    switch (action) {
        case 1: newRow--; break; // Up
        case 0: newCol++; break; // Right
        case 3: newRow++; break; // Down
        case 2: newCol--; break; // Left
    }
    return std::make_pair(newRow, newCol);
}
int QLearningAgent::mapPositionToAction(std::pair<int, int> currentPosition, std::pair<int, int> newPosition) {
    // Example implementation, adjust according to your logic
    if(newPosition.first < currentPosition.first) return 0; // Up
    if(newPosition.second > currentPosition.second) return 1; // Right
    if(newPosition.first > currentPosition.first) return 2; // Down
    if(newPosition.second < currentPosition.second) return 3; // Left

    return -1; // No movement or invalid
}



bool QLearningAgent::isValidMove(const Maze &maze, std::pair<int, int> newPosition) {
    // Get the size of the maze and check if the new position is within bounds and not a wall
    std::pair<int, int> mazeSize = maze.getSize();
    int mazeRows = 21;
    int mazeCols = 21;
    int newRow = newPosition.first;
    int newCol = newPosition.second;

    // Debugging: Print current and new position
    std::cout << "Current Position: (" << position.first << ", " << position.second << ")" << std::endl;
    std::cout << "Attempting Move to: (" << newRow << ", " << newCol << ")" << std::endl;

    // Check if the new position is within the maze boundaries
    if (newRow < 0 || newRow >= mazeRows || newCol < 0 || newCol >= mazeCols) {
        // Debugging: Print out of bounds message
        std::cout << "Move Invalid: New position out of bounds.("<< newRow<< ","<< mazeRows << "," << newCol<< "," << mazeCols<< ")" << std::endl;
        return false;
    }

    // Check if the new position is not a wall
    int cellValue = maze.at(newRow, newCol);
    if (cellValue == 1) {  // Assuming 1 represents wall cells
        // Debugging: Print wall collision message along with the cell value
        // std::cout << "Move Invalid: New position (" << newRow << ", " << newCol 
        //         << ") is a wall. Cell Value: " << cellValue << std::endl;
        return false;
}


    // Debugging: Print valid move message
    std::cout << "Move Valid: New position is free." << std::endl;
    return true;
}

std::pair<int, int> findStartingPosition(const std::vector<std::vector<int>>& mazeGrid) {
    for (int row = 0; row < mazeGrid.size(); ++row) {
        for (int col = 0; col < mazeGrid[row].size(); ++col) {
            if (mazeGrid[row][col] == 2) {  // 2 represents the starting position
                return std::make_pair(row, col);
            }
        }
    }
    return std::make_pair(0, 0); // Default starting position if '2' is not found
}



// Implementation of hasReachedGoal
bool QLearningAgent::hasReachedGoal(const Maze &maze) {
    return position.first == GOAL_ROW && position.second == GOAL_COL;
}

// Implementation of reset
void QLearningAgent::reset() {
    position = startingPosition;
    stepsTaken = 0;
}