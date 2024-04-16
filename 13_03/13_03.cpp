#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <cctype>

std::vector<std::string> split(const std::string& str) {
    std::stringstream ss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}


std::vector<std::string> read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::string line;
    std::vector<std::string> words;
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = split(line);
        words.insert(words.end(), tokens.begin(), tokens.end());
    }
    return words;
}

std::unordered_map<std::string, int> count_words(const std::vector<std::string>& words) {
    std::unordered_map<std::string, int> counts;
#pragma omp parallel for shared(counts)
    for (size_t i = 0; i < words.size(); i++) {
        std::string word = words[i];
#pragma omp critical
        {
            counts[word]++;
        }
    }
    return counts;
}

int main() {
    setlocale(LC_ALL, "rus");
    std::string filename = "input.txt";
    std::vector<std::string> words = read_file(filename);
    std::unordered_map<std::string, int> counts = count_words(words);


    std::vector<std::pair<std::string, int>> sorted_counts(counts.begin(), counts.end());
    std::sort(sorted_counts.begin(), sorted_counts.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });


    for (size_t i = 0; i < 10 && i < sorted_counts.size(); i++) {
        std::cout << sorted_counts[i].first << ": " << sorted_counts[i].second << std::endl;
    }

    return 0;
}
