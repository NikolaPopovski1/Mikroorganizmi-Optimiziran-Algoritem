#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <tuple>
#include <unordered_map>

std::tuple<int, std::pair<int, int>, std::pair<int, int>> itterative_discover_and_alocate(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& xOg, const int& yOg, const int& w, const int& h, const int& count);

void print_matrix(std::vector<std::vector<int>>& newMatrix, const int& w, const int& h) {
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Save the current console attributes
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;

    std::cout << '\t';
    for (int x = 0; x < w; x++) {
        std::cout << x << '\t';
    }
    std::cout << std::endl;
    for (int y = h - 1; y >= 0; y--) {
        std::cout << y << '\t';
        for (int x = 0; x < w; x++) {
            if (newMatrix[y][x] != 0) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
                std::cout << newMatrix[y][x];
                SetConsoleTextAttribute(hConsole, saved_attributes); // Reset to default
            }
            else {
                std::cout << newMatrix[y][x];
            }
            std::cout << '\t';
        }
        std::cout << '\n';
    }
}
void print_matrix_no_tabs(std::vector<std::vector<int>>& newMatrix, const int& w, const int& h) {
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Save the current console attributes
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;

    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            if (newMatrix[y][x] != 0) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
                std::cout << newMatrix[y][x];
                SetConsoleTextAttribute(hConsole, saved_attributes); // Reset to default
            }
            else {
                std::cout << newMatrix[y][x];
            }
        }
        std::cout << '\n';
    }
}



std::vector<std::vector<int>> read_file_and_save_to_matrix(const std::string& filename, int& w, int& h) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open file!\n";
        return std::vector<std::vector<int>>{};
    }

    file >> h >> w;
    file.ignore();

    std::vector<std::vector<int>> oldMatrix(h, std::vector<int>(w, 0));
    char byte;
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            if (!file.get(byte)) throw std::runtime_error("Error reading byte");
            if (byte == '1') oldMatrix[y][x] = byte - '0';
        }
        file.ignore();
        std::cout << '\n';
    }

    return oldMatrix;
}

std::vector<std::vector<int>> create_new_matrix(std::vector<std::vector<int>>& oldMatrix, const int& w, const int& h) {
    std::vector<std::vector<int>> newMatrix(h, std::vector<int>(w, 0));
    std::vector<std::pair<int, std::vector<int>>> microorganismHolder;

    std::vector<
        std::pair<
            /*points in microorganism, O(1) access->*/
            int,
            /*list of pairs and their corresponding matrices*/
            std::unordered_multimap<
                /*area ->*/
                int, 
                /*list of matrices ->*/
                std::vector<
                    /*matrix->*/
                    std::vector<std::vector<int>>
                >
            >
        >
    > saveMicroorganisms;

    std::vector<std::vector<int>> subregion;
    std::tuple<int, std::pair<int, int>, std::pair<int, int>> tmp;
    int xMin, xMax, yMin, yMax;
    int count = 1;
    //print_matrix(oldMatrix, w, h);
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            if (oldMatrix[y][x] == 1 && newMatrix[y][x] == 0) {
                tmp = itterative_discover_and_alocate(oldMatrix, newMatrix, x, y, w, h, count);
                xMin = std::get<1>(tmp).first; xMax = std::get<1>(tmp).second; yMin = std::get<2>(tmp).first; yMax = std::get<2>(tmp).second;

                // ||||||||||||||||||||||||||||||||||||||||||||||||||||||!!!!!!!!!!!!!!!!!!!!!!!!||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // ||||||||||||||||||||||||||||||||||||||||||||||||||||||!OPTIMIZIRAT IF NEED BE!||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
                for (int y = yMin; y <= yMax; y++) {
                    std::vector<int> row;
                    for (int x = xMin; x <= xMax; ++x) {
                        int val = newMatrix[y][x];
                        if (val != 0 && val != count)
                            row.push_back(0);
                        else
                            row.push_back(val);
                    }
                    subregion.push_back(std::move(row));
                }

                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat
                saveMicroorganisms.push_back(std::make_pair(std::get<0>(tmp), std::make_pair((xMax - xMin) * (yMax - yMin), subregion)));
                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat
                //z onw notes, updejtat

                print_matrix(subregion, xMax - xMin + 1, yMax - yMin + 1);

                print_matrix(newMatrix, w, h);
                std::cout << "numOfOnes: " << std::get<0>(tmp) << ", xMin: " << xMin << ", xMax : " << xMax << ", yMin : " << yMin << ", yMax : " << yMax << '\n';
                count++;
                subregion = {};
            }
        }
        std::cout << '\n';
    }

    return newMatrix;
}

//void recursive_discover(std::vector<std::vector<int>>& oldMatrix, std::vector<std::vector<int>>& newMatrix)
std::tuple<int, std::pair<int, int>, std::pair<int, int>> itterative_discover_and_alocate(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& xOg, const int& yOg, const int& w, const int& h, const int& count) {
    if (newMatrix[yOg][xOg] != 0) {
        return std::make_tuple(0, std::pair<int, int>(-1, -1), std::pair<int, int>(-1, -1));
    }

    std::vector<std::pair<int, int>> rememberCoords = {};
    int xCurr = xOg, yCurr = yOg, ones = 0, xMin = w, xMax = -1, yMin = h, yMax = -1, xMinTmp, xMaxTmp, yMinTmp, yMaxTmp, numOfOnes = 0;
    bool middle = false, rightEdge = false, lowerEdge = false, leftEdge = false, upperEdge = false,
        leftUpperCorner = false, rightUpperCorner = false, rightLowerCorner = false, leftLowerCorner = false,
        right = false, down = false, left = false, up = false,
        runCheckCurr = true, error = false;

    if (w == 1 && h == 1) {
        newMatrix[yCurr][xCurr] = count;
        return std::make_tuple(1, std::pair<int, int>(0, 0), std::pair<int, int>(0, 0));
    }

    while (runCheckCurr) {
        newMatrix[yCurr][xCurr] = count;
        numOfOnes++;

        if (xCurr < xMin) xMin = xCurr;
        if (xCurr > xMax) xMax = xCurr;
        if (yCurr < yMin) yMin = yCurr;
        if (yCurr > yMax) yMax = yCurr;

        // find area
        if (yCurr != h - 1 && yCurr != 0 && xCurr != w - 1 && xCurr != 0) { // sredina
            middle = true;
        }
        else if (yCurr != h - 1 && yCurr != 0 && xCurr == w - 1) { // desni rob
            rightEdge = true;
        }
        else if (yCurr == 0 && xCurr != 0 && xCurr != w - 1) { // spodnji rob
            lowerEdge = true;
        }
        else if (yCurr != h - 1 && yCurr != 0 && xCurr == 0) { // levi rob
            leftEdge = true;
        }
        else if (yCurr == h - 1 && xCurr != 0 && xCurr != w - 1) { // zgornji rob
            upperEdge = true;
        }
        else if (yCurr == h - 1 && xCurr == 0) { // kot, levo zgoraj
            leftUpperCorner = true;
        }
        else if (yCurr == h - 1 && xCurr == w - 1) { // kot, desno zgoraj
            rightUpperCorner = true;
        }
        else if (yCurr == 0 && xCurr == w - 1) { // kot, desno spodaj
            rightLowerCorner = true;
        }
        else if (yCurr == 0 && xCurr == 0) { // kot, levo spodaj
            leftLowerCorner = true;
        }
        else throw std::runtime_error("Error: No valid neighbor found");


        if (middle) { // sredina
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else if (rightEdge) { // desni rob
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else if (lowerEdge) { // spodnji rob
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else if (leftEdge) { // levi rob
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else if (upperEdge) { // zgornji rob
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
        }
        else if (leftUpperCorner) { // kot, levo zgoraj
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
        }
        else if (rightUpperCorner) { // kot, desno zgoraj
            down = originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0;
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
        }
        else if (rightLowerCorner) { // kot, desno spodaj
            left = originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else if (leftLowerCorner) { // kot, levo spodaj
            right = originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0;
            up = originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr + 1][xCurr] == 0;
        }
        else throw std::runtime_error("Error: No valid neighbor found");

        if (right) ones++;
        if (down) ones++;
        if (left) ones++;
        if (up) ones++;

        if (ones == 0) runCheckCurr = false;
        else if (ones > 1) {
            while (ones > 1) {
                if (right) {
                    rememberCoords.push_back(std::make_pair(xCurr + 1, yCurr));
                    right = false;
                    ones--;
                }
                else if (down) {
                    rememberCoords.push_back(std::make_pair(xCurr, yCurr - 1));
                    down = false;
                    ones--;
                }
                else if (left) {
                    rememberCoords.push_back(std::make_pair(xCurr - 1, yCurr));
                    left = false;
                    ones--;
                }
                else if (up) {
                    rememberCoords.push_back(std::make_pair(xCurr, yCurr + 1));
                    up = false;
                    ones--;
                }
            }
        }
        if (ones == 1) {
            if (right) xCurr++;
            else if (down) yCurr--;
            else if (left) xCurr--;
            else if (up) yCurr++;
            else throw std::runtime_error("Error: No valid neighbor found");
        }

        // reset
        ones = 0;
        middle = false, rightEdge = false, lowerEdge = false, leftEdge = false, upperEdge = false,
            leftUpperCorner = false, rightUpperCorner = false, rightLowerCorner = false, leftLowerCorner = false,
            right = false, down = false, left = false, up = false,
            error = false;
    }

    std::tuple<int, std::pair<int, int>, std::pair<int, int>> tmp;
    for (const auto& coord : rememberCoords) {
        tmp = itterative_discover_and_alocate(originalMatrix, newMatrix, coord.first, coord.second, w, h, count);
        xMinTmp = std::get<1>(tmp).first;
        xMaxTmp = std::get<1>(tmp).second;
        yMinTmp = std::get<2>(tmp).first;
        yMaxTmp = std::get<2>(tmp).second;
        if (xMinTmp == -1 || xMaxTmp == -1 || yMinTmp == -1 || yMaxTmp == -1) continue;
        if (xMinTmp < xMin) xMin = xMinTmp;
        if (xMaxTmp > xMax) xMax = xMaxTmp;
        if (yMinTmp < yMin) yMin = yMinTmp;
        if (yMaxTmp > yMax) yMax = yMaxTmp;
        numOfOnes += std::get<0>(tmp);
    }

    return std::make_tuple(numOfOnes, std::make_pair(xMin, xMax), std::make_pair(yMin, yMax));
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    int w, h;

    std::vector <std::vector<int>> oldMatrix = read_file_and_save_to_matrix(argv[1], w, h);
    //print_matrix_no_tabs(oldMatrix, w, h);
    std::vector <std::vector<int>> newMatrix = create_new_matrix(oldMatrix, w, h);

    ///*
    print_matrix_no_tabs(oldMatrix, w, h);

    std::cout << '\n';
    std::cout << '\n';

    print_matrix(newMatrix, w, h);
    //*/

    // End timer
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    std::cout << "Elapsed time: " << duration_ms << " ms (" << duration_s << " s)" << std::endl;

    return 0;
}