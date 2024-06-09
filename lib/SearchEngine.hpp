#pragma once

#include "Indexer.hpp"
#include "Searcher.hpp"
#include "QueryParser.hpp"
#include "ConsolePrinter.hpp"

class SearchEngine
{
private:
    Indexer indexer;
    Searcher searcher;
    QueryParser parser;
    ConsolePrinter printer;

public:
    void Start() {
        printer.clearScreen();
        // Create general data storage
        std::string storage = "SearchEngineData";
        fs::create_directory(storage);

        Index();
        Search();
    }

    void Index() {
        std::string directoryPath = printer.StartPrinter();
        printer.printProgressBar(0.0);

        indexer = Indexer(directoryPath);
        printer.printProgressBar(0.25);
        indexer.startIndexer();
        
        printer.printProgressBar(0.50);
        // Locate and load the indexed folder's data
        fs::path temp_path(directoryPath);
        std::string storage_name = temp_path.filename().string();

        std::string file_path = "SearchEngineData\\" + storage_name + "\\file_data.ind";
        std::string trie_path = "SearchEngineData\\" + storage_name + "\\trie_data.ind";
        
        printer.printProgressBar(0.75);
        searcher.loadStorage(file_path, trie_path);

        printer.printProgressBar(1.0);
        printer.clearScreen();
    }

    void Search() {
        std::string user_query = printer.SearchQuery();
        printer.clearScreen();
        // Convert user query to Reverse Polish Notation
        std::vector<std::string> results = parser.tokenizeQuery(user_query);
        
        try {
            std::vector<std::string> rpn = parser.convertToRPN(results);
            std::unordered_map<size_t, double> documentScores = parser.evaluateRPN(searcher, rpn);
        } catch(const std::exception& e) {
            printer.setErrorType(3);
            Search();
            return;
        }
        printer.setErrorType(0);

        std::vector<std::string> rpn = parser.convertToRPN(results);
        std::unordered_map<size_t, double> documentScores = parser.evaluateRPN(searcher, rpn);
        // Rank documents based on BM25 scores
        std::vector<size_t> rankedDocuments = parser.rankDocuments(documentScores);
        printer.clearScreen();

        // Print top-ranked document IDs
        bool more = false;
        size_t row = 1;
        if (rankedDocuments.empty()) {
            std::vector<std::vector<std::string>> table_data(1);
            table_data[0].push_back("No files were found containing search query.");
            more = printer.printSearchResults(table_data);

        } else {
            std::vector<std::vector<std::string>> table_data(rankedDocuments.size() + 1);
            std::vector<std::string> row_data(rankedDocuments.size() + 1);
            table_data[0].push_back("File ID:");
            table_data[0].push_back("File Name:");

            for (size_t docId : rankedDocuments) {
                table_data[row].push_back(std::to_string(docId));
                table_data[row].push_back(searcher.getFileName(docId));
                ++row;
            }
            more = printer.printSearchResults(table_data);
        }
        if (more)
            Search();
    }
};