#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <cmath>
#include "Trie.hpp"

namespace fs = std::filesystem;

const std::unordered_set<std::string> validExtensions = {".cpp", ".h", ".hpp", ".txt"};

class Indexer {
private:    
    std::string directoryPath;
    Trie dataBase;

    struct FileData {
        size_t fileId;         // ID of file
        std::string fileName;  // Name of file
        size_t fileLength;     // Length in words
    };
    
    std::vector<FileData> file_data;
    double avgFileLength;
    size_t totalFileNum;
    
public:
    Indexer() = default;

    Indexer(const std::string& directoryPath_)
        : directoryPath(directoryPath_)
        , avgFileLength(0)
        , totalFileNum(0) {}

    void startIndexer() {
        size_t docId = 0;
        size_t totalFileLength = 0;
        std::vector<size_t> docLengths;
        
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                std::string extension = fs::path(filePath).extension().string();
                if (validExtensions.count(extension) > 0) {
                    processFile(filePath, docId, docLengths, totalFileLength);
                    ++docId;
                }
            }
        }
        setBM25Parameters(docLengths, totalFileLength);
        createStorage();
    }

    std::pair<size_t, double> getBM25Parameters() {
        return {totalFileNum, avgFileLength};
    }

private:
    void processFile(const std::string& filePath, size_t docId, std::vector<size_t>& docLengths, size_t& totalFileLength) {
        std::ifstream file(filePath);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << "\n";
            exit(1);
        }

        size_t fileSize = 0;
        std::string line;
        size_t lineCounter = 0;
        while (std::getline(file, line)) {
            ++lineCounter;
            std::string word;
            for (char c : line) {
                if (std::isalpha(c) || c == '_') {
                    word += std::tolower(c);
                } else if (!word.empty()) {
                    dataBase.insert(word, docId, lineCounter);
                    word.clear();
                    ++fileSize;
                }
            }
            if (!word.empty()) {
                dataBase.insert(word, docId, lineCounter);
                ++fileSize;
            }
        }

        docLengths.push_back(fileSize);
        totalFileLength += fileSize;
        file_data.push_back({docId, filePath, fileSize});
        file.close();
    }

    void setBM25Parameters(const std::vector<size_t>& docLengths, size_t& totalFileLength) {
        totalFileNum = docLengths.size();
        avgFileLength = static_cast<double>(totalFileLength) / totalFileNum;
    }

    void createStorage() const {
        // Create a directory for Indexer data
        fs::path path(directoryPath);
        std::string storage_name = path.filename().string();

        std::string storage_path = "SearchEngineData\\" + storage_name;
        fs::create_directory(storage_path);

        std::string file_path = storage_path + "\\file_data.ind";
        std::ofstream file_storage(file_path, std::ios::binary);

        if (!file_storage.is_open()) {
            std::cerr << "Error creating indexer files.\n";
            exit(1);
        }

        // Write total file number + average file length
        file_storage.write(reinterpret_cast<const char*>(&totalFileNum), sizeof(size_t));
        file_storage.write(reinterpret_cast<const char*>(&avgFileLength), sizeof(double));

        // Write file data
        for (const auto& file : file_data) {
            size_t nameSize = file.fileName.size();
            file_storage.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
            file_storage.write(file.fileName.c_str(), nameSize);
            file_storage.write(reinterpret_cast<const char*>(&file.fileLength), sizeof(size_t));
            file_storage.write(reinterpret_cast<const char*>(&file.fileId), sizeof(size_t));
        }

        // Write Trie data
        std::string trie_path = storage_path + "\\trie_data.ind";
        dataBase.serialize(trie_path);
        file_storage.close();
    }
};