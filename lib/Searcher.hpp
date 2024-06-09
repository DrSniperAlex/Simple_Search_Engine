#pragma once

#include <algorithm>
#include <fstream>
#include "Trie.hpp"

class Searcher {
private:

    Trie data_base;
    struct FileData {
        size_t fileId;         // ID of file
        std::string fileName;  // Name of file
        size_t fileLength;     // Length in words
    };
    
    std::vector<FileData> file_data;
    double avgFileLength;
    size_t totalFileNum;

    const double k1 = 1.2; // Tuning parameter
    const double b = 0.75; // Tuning parameter

public:
    Searcher() 
        : avgFileLength(0)
        , totalFileNum(0)
    {};

    void loadStorage(const std::string& file_storage, const std::string& trie_storage) {
        std::ifstream storage(file_storage, std::ios::binary);
        if (!storage.is_open()) {
            std::cerr << "Error opening indexer file data storage: " << file_storage << "\n";
            exit(1);
        }

        // Read total file number + average length
        storage.read(reinterpret_cast<char*>(&totalFileNum), sizeof(size_t));
        storage.read(reinterpret_cast<char*>(&avgFileLength), sizeof(double));

        // Read file data
        file_data.resize(totalFileNum);
        for (auto& file : file_data) {
            size_t nameSize;
            storage.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
            file.fileName.resize(nameSize);
            storage.read(reinterpret_cast<char*>(&file.fileName[0]), nameSize);
            storage.read(reinterpret_cast<char*>(&file.fileLength), sizeof(size_t));
            storage.read(reinterpret_cast<char*>(&file.fileId), sizeof(size_t));
        }

        // Read Trie data
        data_base = data_base.deserialize(trie_storage);
        storage.close();
    }

    std::unordered_map<size_t, double> getMatchingDocuments(const std::string& term) {
        std::unordered_map<size_t, double> matchingDocuments;
        for (const auto& document : file_data) {
            double bm25Score = calculateBM25(term, document.fileId);
            if (bm25Score > 0) {
                matchingDocuments[document.fileId] = bm25Score;
            }
        }
        return matchingDocuments;
    }

    std::string getFileName(size_t docId) {
        for(auto file : file_data) {
            if (file.fileId == docId) {
                return file.fileName;
            }
        }
        return "";
    }

    double calculateBM25(const std::string& term, size_t documentId) const {
        double score = 0;
        double idf = calculateIDF(term, documentId);
        double tf = calculateTF(term, documentId);

        double K = k1 * ((1 - b) + b * (static_cast<double>(file_data[documentId].fileLength) / avgFileLength));
        score = idf * ((tf * (k1 + 1)) / (tf + K));
        return score;
    }

private:
    double calculateIDF(const std::string& term, size_t documentId) const {
        double documentFrequency = data_base.getDocFrequency(term);
        if (documentFrequency == 0) {
            return 0; // word not found in any document
        }
        return (std::log((totalFileNum - documentFrequency + 0.5) / (documentFrequency + 0.5)) / std::log(10)) + 1;
    }

    double calculateTF(const std::string& term, size_t documentId) const {
        return data_base.getTermFrequency(term, documentId);
    }
};