#include <iostream>
#include <string>

#include "MazeSolver.h"

int main() {
    string input, line;
    while (getline(cin, line)) {
        input += line;
    }
    if (input.empty()) {
        return 0;
    }
    MazeSolver solver;
    solver.loadConfig(input);
    solver.solve();
    string json = solver.resultToJson();
    cout << json << endl;
    return 0;
}
