#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

void add_to_matrix(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& num, int& count, int& y, int& x, const int& w, const int& h);
void itterative_discover_and_alocate(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& xOg, const int& yOg, const int& w, const int& h, const int& count);

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


std::vector<std::vector<int>> read_file_and_save_to_matrix(const std::string& filename, int& h, int& w) {
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

std::vector<std::vector<int>> create_new_matrix(std::vector<std::vector<int>>& oldMatrix, const int& h, const int& w) {
    std::vector<std::vector<int>> newMatrix(h, std::vector<int>(w, 0));
    int count = 1;
    print_matrix(oldMatrix, w, h);
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            if (oldMatrix[y][x] == 1 && newMatrix[y][x] == 0) {
                print_matrix(newMatrix, w, h);
                itterative_discover_and_alocate(oldMatrix, newMatrix, x, y, w, h, count);
                count++;
            }
            //std::cout << newMatrix[y][x] << '\t';
        }
        std::cout << '\n';
    }

    return newMatrix;
}

//void recursive_discover(std::vector<std::vector<int>>& oldMatrix, std::vector<std::vector<int>>& newMatrix)
void itterative_discover_and_alocate(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& xOg, const int& yOg, const int& w, const int& h, const int& count) {
    bool flag = true;
    int xCurr = xOg, yCurr = yOg;
    while (flag) {
        newMatrix[yCurr][xCurr] = count;

        if (yCurr != h - 1 && yCurr != 0 && xCurr != w - 1 && xCurr != 0) { // sredina
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr != h - 1 && yCurr != 0 && xCurr == w - 1) { // desni rob
            if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == 0 && xCurr != 0 && xCurr != w - 1) { // spodnji rob
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr != h - 1 && yCurr != 0 && xCurr == 0) { // levi rob
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == h - 1 && xCurr != 0 && xCurr != w - 1) { // zgornji rob
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == h - 1 && xCurr == 0) { // kot, levo zgoraj
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == h - 1 && xCurr == w - 1) { // kot, desno zgoraj
            if (originalMatrix[yCurr - 1][xCurr] == 1 && newMatrix[yCurr - 1][xCurr] == 0) { // spodaj
                yCurr--;
            }
            else if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == 0 && xCurr == w - 1) { // kot, desno spodaj
            if (originalMatrix[yCurr][xCurr - 1] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // levo
                xCurr--;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else if (yCurr == 0 && xCurr == 0) { // kot, levo spodaj
            if (originalMatrix[yCurr][xCurr + 1] == 1 && newMatrix[yCurr][xCurr + 1] == 0) { // desno
                xCurr++;
            }
            else if (originalMatrix[yCurr + 1][xCurr] == 1 && newMatrix[yCurr][xCurr - 1] == 0) { // zgoraj
                yCurr++;
            }
            else {
                flag = false;
            }
        }
        else throw std::runtime_error("Error: No valid neighbor found");
    }

    return;
}

void add_to_matrix(std::vector<std::vector<int>>& originalMatrix, std::vector<std::vector<int>>& newMatrix, const int& num, int& count, int& y, int& x, const int& w, const int& h) {
    if (num == 1) {
        if (y != h - 1 && x != 0 && y != 0 && x != w - 1) {
            if (originalMatrix[y + 1][x] == 0 && originalMatrix[y][x - 1] == 0 && originalMatrix[y - 1][x] == 0 && originalMatrix[y][x + 1] == 0) {
                newMatrix[y][x] = count;
                count++;
                return;
            }
            else {
                if (originalMatrix[y + 1][x] != 0) {
                    newMatrix[y][x] = originalMatrix[y + 1][x];
                }
                else if (originalMatrix[y][x - 1] != 0) {
                    newMatrix[y][x] = originalMatrix[y][x - 1];
                }
                else if (originalMatrix[y - 1][x] != 0) {
                    newMatrix[y][x] = originalMatrix[y - 1][x];
                }
                else if (originalMatrix[y][x + 1] != 0) {
                    newMatrix[y][x] = originalMatrix[y][x + 1];
                }
                else {
                    newMatrix[y][x] = count;
                    count++;
                }
            }
        }
        else {
            if (y != h - 1 && x == 0) {
                newMatrix[y][x] = count;
                count++;
            }
            else if (y == h - 1 && x == w - 1) {
                if (originalMatrix[y][x - 1] != 0) {
                    newMatrix[y][x] = originalMatrix[y][x - 1];
                }
                else if (originalMatrix[y - 1][x] != 0) {
                    newMatrix[y][x] = originalMatrix[y - 1][x];
                }
                else {
                    newMatrix[y][x] = count;
                    count++;
                }
            }
            else if (y == 0 && x == w - 1) {
                if (originalMatrix[y - 1][x] != 0) {
                }
            }
        }
        /*
        if (y != h - 1) {
            if (originalMatrix[y + 1][x] != 0) {
                newMatrix[y][x] = originalMatrix[y + 1][x];
            }
            else {
                newMatrix[y][x] = count;
                count++;
            }
            return;
        }
        if (x != 0) {
            if (originalMatrix[y][x - 1] != 0) {
                newMatrix[y][x] = originalMatrix[y][x - 1];
            }
            else {
                newMatrix[y][x] = count;
                count++;
            }
            return;
        }
        */

        throw std::runtime_error("Error: No valid neighbor found");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    int w, h;
    std::vector <std::vector<int>> oldMatrix = read_file_and_save_to_matrix(argv[1], h, w);
    std::vector <std::vector<int>> newMatrix = create_new_matrix(oldMatrix, h, w);


    print_matrix_no_tabs(oldMatrix, w, h);

    std::cout << '\n';
    std::cout << '\n';

    print_matrix(newMatrix, w, h);

    return 0;
}
