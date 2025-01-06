#pragma once

#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <ranges>

unsigned firstPass(const std::vector<std::string>& file, std::map<std::string, unsigned short>& labels, unsigned short& globalStart);
std::vector<unsigned char> secondPass(const std::vector<std::string>& file, const std::map<std::string, unsigned short>& labels, unsigned short globalStart, unsigned maxSize);

template<unsigned romsize>
std::array<unsigned char, romsize> assemble(std::string filename) {
    std::array<unsigned char, romsize> rom;
    unsigned short globalStart = 0;

    std::map<std::string, unsigned short> labels;

    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("file not found: " + filename);
    }
    std::vector<std::string> file;
    std::string line;
    while (std::getline(in, line)) {
        file.push_back(line);
    }

    if (firstPass(file, labels, globalStart) > romsize + globalStart) {
        throw std::runtime_error("not enough rom");
    }
    int counter = 0;
    for (auto c : secondPass(file, labels, globalStart, romsize)) {
        rom[counter] = c;
        ++counter;
    }
    return rom;
}
