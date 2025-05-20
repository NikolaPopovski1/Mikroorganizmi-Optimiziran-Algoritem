#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <tuple>
#include <unordered_map>
#include <psapi.h>
#include <stdexcept>
#include <set>

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
void iterate_and_print_saved_microorganisms(std::unordered_map<int, std::unordered_multimap<int, std::vector<std::vector<int>>>>& saveMicroorganisms) {
    for (auto it = saveMicroorganisms.begin(); it != saveMicroorganisms.end(); ++it) {
        int points = it->first;
        auto& multimap = it->second;

        std::cout << "Points: " << points << '\n';

        for (auto mmIt = multimap.begin(); mmIt != multimap.end(); ++mmIt) {
            int area = mmIt->first;
            const auto& matrix = mmIt->second;

            std::cout << "  Area: " << area << '\n';

            // Use const_cast to get non-const reference for dimension extraction
            auto& nonConstMatrix = const_cast<std::vector<std::vector<int>>&>(matrix);
            int height = static_cast<int>(nonConstMatrix.size());
            int width = height > 0 ? static_cast<int>(nonConstMatrix[0].size()) : 0;

            print_matrix_no_tabs(nonConstMatrix, width, height);
        }
    }
}

void flipHorizontal(std::vector<std::vector<int>>& mat) {
    for (auto& row : mat)
        std::reverse(row.begin(), row.end());
}
void flipVertical(std::vector<std::vector<int>>& mat) {
    std::reverse(mat.begin(), mat.end());
}
void rotate90Clockwise(std::vector<std::vector<int>>& mat) {
    int n = mat.size(), m = mat[0].size();
    std::vector<std::vector<int>> res(m, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            res[j][n - 1 - i] = mat[i][j];
    mat = std::move(res);
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
    }

    file >> std::ws;
    if (!file.eof()) {
        throw std::runtime_error("Input file contains extra characters after matrix data.");
    }

    return oldMatrix;
}

std::unordered_map<int, std::unordered_multimap<int, std::vector<std::vector<int>>>> get_unique_microorganisms(std::vector<std::vector<int>>& oldMatrix, std::vector<std::vector<int>>& newMatrix, const int& w, const int& h) {
    std::vector<std::pair<int, std::vector<int>>> microorganismHolder;

    std::unordered_map<
        /*points in microorganism, O(1) access->*/
        int,
        /*list of pairs and their corresponding matrices*/
        std::unordered_multimap<
            /*area + hash->*/
            int,
            /*matrix->*/
            std::vector<std::vector<int>>
        >
    > savedMicroorganisms = {};

    std::vector<std::vector<int>> subregion, tmpVector;
    std::tuple<int, std::pair<int, int>, std::pair<int, int>> tmp;
    int xMin, xMax, yMin, yMax, numOfOnes, area = 0, yForHash = 0, yMaxForHash = -1, xForHash = 1, widthOfHash = 0;
    int count = 1;

    // preveri, ce je robni primer ali ne, ce ni gre v else stavek
    if (w == 1 && h == 1) {
        std::unordered_multimap<int, std::vector<std::vector<int>>> innerMap;
        if (oldMatrix[0][0] == 1) {
            innerMap.insert({ 1, {{1}} });
            savedMicroorganisms.insert({ numOfOnes, innerMap });
        }
    }
    else if (h == 1) {
        std::unordered_multimap<int, std::vector<std::vector<int>>> innerMap;
        std::set<int> counter = {};
        int width = 0;

        for (int x = 0; x < w; x++) {
            if (oldMatrix[0][x] == 1 && newMatrix[0][x] == 0) {
                int xCurr = x;
                while (true) {
                    if (xCurr != w - 1) {
                        newMatrix[0][xCurr] = count;
                        width++;
                    }
                    else {
                        break;
                    }
                    xCurr++;
                }
                counter.insert(width);
                width = 0;
                count++;
            }
        }

        innerMap.insert({ static_cast<int>(counter.size()), {{1}}});
        savedMicroorganisms.insert({ -1, innerMap });
    }
    else {
        for (int y = h - 1; y >= 0; y--) {
            for (int x = 0; x < w; x++) {
                if (oldMatrix[y][x] == 1 && newMatrix[y][x] == 0) {
                    tmp = itterative_discover_and_alocate(oldMatrix, newMatrix, x, y, w, h, count);
                    numOfOnes = std::get<0>(tmp); xMin = std::get<1>(tmp).first; xMax = std::get<1>(tmp).second; yMin = std::get<2>(tmp).first; yMax = std::get<2>(tmp).second;
                    area = (xMax - xMin + 1) * (yMax - yMin + 1);

                    yMaxForHash = xMax - xMin + 1;
                    for (int y = yMax; y >= yMin; y--) {
                        std::vector<int> row;
                        for (int x = xMin; x <= xMax; x++) {
                            xForHash++;
                            int val = newMatrix[y][x];
                            if (val != 0 && val != count)
                                row.push_back(0);
                            else if (val == 0)
                                row.push_back(0);
                            else {
                                row.push_back(1);
                                if (x == xMin || x == xMax || y == yMax || y == yMin) area++;
                            }
                        }
                        subregion.push_back(std::move(row));
                    }

                    bool add = true;
                    area += (xMax - xMin + 1) * (yMax - yMin + 1);
                    if (savedMicroorganisms.find(numOfOnes) != savedMicroorganisms.end()) {
                        auto range = savedMicroorganisms[numOfOnes].equal_range(area);
                        for (auto it = range.first; it != range.second; ++it) {
                            tmpVector = it->second;
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            rotate90Clockwise(subregion);
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            rotate90Clockwise(subregion);
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            rotate90Clockwise(subregion);
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            rotate90Clockwise(subregion);

                            // ||||||||||||||||||||||||||||||||||||||||||||||||||||||!!!!!!!!!!!!!!!!!!!!!!!!||||||||||||||||||||||||||||||||||||||||||||||||||||||
                            // ||||||||||||||||||||||||||||||||||||||||||||||||!CHECKING MIRRORED MICROORGANISMS!||||||||||||||||||||||||||||||||||||||||||||||||||
                            // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv!!!!!!!!!!!!!!!!!!!!!!!!vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
                            /*
                            flipHorizontal(subregion);
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            flipHorizontal(subregion);
                            flipVertical(subregion);
                            if (tmpVector == subregion) {
                                add = false;
                                break;
                            }
                            flipVertical(subregion);
                            */
                        }
                        if (add) savedMicroorganisms[numOfOnes].insert({ area, subregion });
                    }
                    else {
                        std::unordered_multimap<int, std::vector<std::vector<int>>> innerMap;
                        innerMap.insert({ area, subregion });
                        savedMicroorganisms.insert({ numOfOnes, innerMap });
                    }

                    count++;
                    area = 0;
                    subregion = {};
                }
            }
        }
    }

    return savedMicroorganisms;
}

std::tuple<int, std::pair<int, int>, std::pair<int, int>> itterative_discover_and_alocate(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& xOg, const int& yOg, const int& w, const int& h, const int& count) {
    std::vector<std::pair<int, int>> rememberCoords = {};
    int xCurr = xOg, yCurr = yOg, ones = 0, xMin = w, xMax = -1, yMin = h, yMax = -1, xMinTmp, xMaxTmp, yMinTmp, yMaxTmp, numOfOnes = 0;
    bool middle = false, rightEdge = false, lowerEdge = false, leftEdge = false, upperEdge = false,
        leftUpperCorner = false, rightUpperCorner = false, rightLowerCorner = false, leftLowerCorner = false,
        right = false, down = false, left = false, up = false,
        runCheckCurr = true;

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

        if (ones == 0) break;
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
        if (right) xCurr++;
        else if (down) yCurr--;
        else if (left) xCurr--;
        else if (up) yCurr++;
        else throw std::runtime_error("Error: No valid neighbor found");

        // reset
        ones = 0;
        middle = false, rightEdge = false, lowerEdge = false, leftEdge = false, upperEdge = false,
            leftUpperCorner = false, rightUpperCorner = false, rightLowerCorner = false, leftLowerCorner = false,
            right = false, down = false, left = false, up = false;
    }

    std::tuple<int, std::pair<int, int>, std::pair<int, int>> tmp;
    for (const auto& coord : rememberCoords) {
        if (newMatrix[coord.second][coord.first] == 0) {
            tmp = itterative_discover_and_alocate(originalMatrix, newMatrix, coord.first, coord.second, w, h, count);
        }
        else continue;
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
    std::vector<std::vector<int>> newMatrix(h, std::vector<int>(w, 0));
    std::unordered_map<int, std::unordered_multimap<int, std::vector<std::vector<int>>>> savedMicroorganisms = get_unique_microorganisms(oldMatrix, newMatrix, w, h);

    if (savedMicroorganisms.find(-1) != savedMicroorganisms.end()) {
        auto& innerMap = savedMicroorganisms[-1];
        if (!innerMap.empty()) {
            auto it = innerMap.begin();
            int area = it->first;
            std::cout << area << '\n';
            return 0;
        }
        return 1;
    }

    size_t count = 0;
    for (const auto& outer_pair : savedMicroorganisms) {
        const auto& inner_multimap = outer_pair.second;
        count += inner_multimap.size(); // each element's mapped value is one std::vector<std::vector<int>>
    }
    std::cout << count << '\n';

    /*
    // End timer
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    std::cout << "Elapsed time: " << duration_ms << " ms (" << duration_s << " s)" << std::endl;
    */

    return 0;
}