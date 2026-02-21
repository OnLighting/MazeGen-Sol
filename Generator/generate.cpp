#include"MySTL.h"
#include<string>
#include"MazeGenerator.h"
int main() {
    std::string json, line;
    while (std::getline(std::cin, line)) {
        json += line;
    }

    if (json.empty()) {
        return 0;
    }
    MazeGenerator gen;
    gen.loadConfig(json);
    gen.generate();
    cout << gen.gridToJson() << endl;
    return 0;
}