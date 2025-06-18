#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>  


using Map = std::vector<std::vector<int>>;

void printMap(const Map& map) {
    std::cout << "--- Current Map ---" << std::endl;
    for (const auto& row : map) {
        for (int cell : row) {
            std::cout << (cell ? '#' : ' '); // '#' para celdas ocupadas, ' ' para vacías
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) {
    Map newMap = currentMap;

    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int count = 0;
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < H && ny >= 0 && ny < W) {
                        count += currentMap[nx][ny];
                    } else {
                        count += 1; // Considera bordes como ocupados
                    }
                }
            }

            int total = (2 * R + 1) * (2 * R + 1);
            if (count >= total * U)
                newMap[x][y] = 1;
            else
                newMap[x][y] = 0;
        }
    }

    return newMap;
}

Map drunkAgent(const Map& currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> rand01(0.0, 1.0);
    std::uniform_int_distribution<int> dirDist(0, 3);

    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int dirIndex = dirDist(rng);
    double currentRoomProb = probGenerateRoom;
    double currentDirProb = probChangeDirection;

    for (int j = 0; j < J; ++j) {
        for (int i = 0; i < I; ++i) {
            // Traza el camino
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W)
                newMap[agentX][agentY] = 1;

            // Probabilidad de generar habitación
            if (rand01(rng) < currentRoomProb) {
                for (int dx = -roomSizeX / 2; dx <= roomSizeX / 2; ++dx) {
                    for (int dy = -roomSizeY / 2; dy <= roomSizeY / 2; ++dy) {
                        int nx = agentX + dx;
                        int ny = agentY + dy;
                        if (nx >= 0 && nx < H && ny >= 0 && ny < W) {
                            newMap[nx][ny] = 1;
                        }
                    }
                }
                currentRoomProb = probGenerateRoom;
            } else {
                currentRoomProb += probIncreaseRoom;
            }

            // Probabilidad de cambiar dirección
            if (rand01(rng) < currentDirProb) {
                dirIndex = dirDist(rng);
                currentDirProb = probChangeDirection;
            } else {
                currentDirProb += probIncreaseChange;
            }

            // Movimiento
            int nx = agentX + directions[dirIndex].first;
            int ny = agentY + directions[dirIndex].second;

            if (nx >= 0 && nx < H && ny >= 0 && ny < W) {
                agentX = nx;
                agentY = ny;
            } else {
                dirIndex = dirDist(rng); // rebote
            }
        }
    }

    return newMap;
}

int main() {
    std::cout << "--- CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ---" << std::endl;

    int mapRows = 10;
    int mapCols = 20;
    Map myMap(mapRows, std::vector<int>(mapCols, 0));

    // Inicializa el mapa con ruido aleatorio para cellular automata
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> noise(0, 1);
    for (int i = 0; i < mapRows; ++i) {
        for (int j = 0; j < mapCols; ++j) {
            myMap[i][j] = noise(rng);
        }
    }

    int drunkAgentX = mapRows / 2;
    int drunkAgentY = mapCols / 2;

    std::cout << "\nInitial map state:" << std::endl;
    printMap(myMap);

    //std::cout << "\n--- Simulation Start ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    int numIterations = 5;

    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;
    double ca_U = 0.5;

    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = 5;
    int da_I = 10;
    int da_roomSizeX = 5;
    int da_roomSizeY = 3;
    double da_probGenerateRoom = 0.1;
    double da_probIncreaseRoom = 0.05;
    double da_probChangeDirection = 0.2;
    double da_probIncreaseChange = 0.03;

    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Iteration " << iteration + 1 << " ---" << std::endl;

        myMap = cellularAutomata(myMap, ca_W, ca_H, ca_R, ca_U);
        myMap = drunkAgent(myMap, da_W, da_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                           da_probGenerateRoom, da_probIncreaseRoom,
                           da_probChangeDirection, da_probIncreaseChange,
                           drunkAgentX, drunkAgentY);

        printMap(myMap);
    }

    std::cout << "\n--- Simulation Finished ---" << std::endl;
    return 0;
}
