#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

std::map<std::string, std::string> config;

int main() {
    // parse the config file
    std::ifstream configFile("config.txt");
    
    if (!configFile.is_open()){
        std::cerr << "Could not open config file" << std::endl;
        return 1;
    }
    
    std::string line;
    while (getline(configFile, line)){
        std::size_t pos = line.find('=');
        config.insert({line.substr(0, pos), line.substr(pos + 1)});
    }

}