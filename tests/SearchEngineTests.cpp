#include <iostream>
#include <cmath>
#include "../lib/Searcher.hpp"
#include "../lib/Indexer.hpp"
#include "../lib/QueryParser.hpp"
#include <gtest/gtest.h>

TEST(IndexerTests, ValidDirectoryPath) {
    std::string storage = "SearchEngineData";
    fs::create_directory(storage);
    std::filesystem::path dirPath = (std::filesystem::path(__FILE__).remove_filename()) /= "testing_folder";
    Indexer indexer = Indexer(dirPath.string());
    bool found_excp = false;
    try {
        indexer.startIndexer();
    } catch(const std::exception& e) {
        found_excp = true;
    }
    EXPECT_EQ(found_excp, false);
}

TEST(IndexerTests, WrongDirectoryPath) {
    std::string storage = "SearchEngineData";
    fs::create_directory(storage);
    std::filesystem::path dirPath = (std::filesystem::path(__FILE__).remove_filename()) /= "wrong_directory";
    Indexer indexer = Indexer(dirPath.string());
    bool found_excp = false;
    try {
        indexer.startIndexer();
    } catch(const std::exception& e) {
        found_excp = true;
    }
    EXPECT_EQ(found_excp, true);
}

TEST(IndexerTests, CheckBM25Parameters) {
    std::string storage = "SearchEngineData";
    fs::create_directory(storage);
    std::filesystem::path dirPath = (std::filesystem::path(__FILE__).remove_filename()) /= "testing_folder";
    Indexer indexer = Indexer(dirPath.string());
    indexer.startIndexer();
    std::pair<size_t, double> BM25Data = indexer.getBM25Parameters();
    EXPECT_EQ(BM25Data.first, 7);
    EXPECT_NEAR(BM25Data.second, 170.142, 0.1);
}

TEST(QueryParserTests, TokenizingValidQuery) {
    QueryParser parser;
    std::string user_query = "(vector OR map) AND int";
    std::vector<std::string> results = parser.tokenizeQuery(user_query);

    std::vector<std::string> expected = {"(", "vector", "OR", "map", ")", "AND", "int"};
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i], expected[i]);
    }
}

TEST(QueryParserTests, TokenizingInvalidQuery) {
    QueryParser parser;
    std::string user_query = "(vector or map) AND OR int";
    std::vector<std::string> results = parser.tokenizeQuery(user_query);

    std::vector<std::string> expected = {"(", "vector", "or", "map", ")", "AND", "OR", "int"};
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i], expected[i]);
    }
}

TEST(QueryParserTests, ConversionToRPNValidQuery) {
    QueryParser parser;
    std::string user_query = "(vector OR map) AND int";
    std::vector<std::string> results = parser.tokenizeQuery(user_query);
    std::vector<std::string> rpn = parser.convertToRPN(results);

    std::vector<std::string> expected = {"vector", "map", "OR", "int", "AND"};
    for (size_t i = 0; i < rpn.size(); ++i) {
        EXPECT_EQ(rpn[i], expected[i]);
    }
}

TEST(QueryParserTests, ConversionToRPNEmptyQuery) {
    QueryParser parser;
    std::string user_query = "";
    std::vector<std::string> results = parser.tokenizeQuery(user_query);
    std::vector<std::string> rpn = parser.convertToRPN(results);
    int doc_cnt = 0;
    for (size_t i = 0; i < rpn.size(); ++i) {
        ++doc_cnt;
    }
    EXPECT_EQ(doc_cnt, 0);
}

TEST(QueryParserTests, ConversionToRPNInvalidQuery) {
    QueryParser parser;
    std::string user_query = "vector AND OR map";
    std::vector<std::string> results = parser.tokenizeQuery(user_query);

    bool found_error = false;
    try {
        std::vector<std::string> rpn = parser.convertToRPN(results);
    } catch(const std::exception& e) {
        found_error = true;
    }
    EXPECT_EQ(found_error, true);
}

TEST(SearcherTests, EvaluationOfRPN) {
    std::string storage = "SearchEngineData";
    fs::create_directory(storage);
    std::filesystem::path dirPath = (std::filesystem::path(__FILE__).remove_filename()) /= "testing_folder";
    Indexer indexer = Indexer(dirPath.string());
    indexer.startIndexer();

    fs::path temp_path(dirPath);
    std::string storage_name = temp_path.filename().string();

    std::string file_path = "SearchEngineData\\" + storage_name + "\\file_data.ind";
    std::string trie_path = "SearchEngineData\\" + storage_name + "\\trie_data.ind";

    Searcher searcher;
    searcher.loadStorage(file_path, trie_path);

    QueryParser parser;
    std::string user_query = "(vector OR map) AND int";
    std::vector<std::string> query = parser.tokenizeQuery(user_query);
    std::vector<std::string> rpn = parser.convertToRPN(query);
    std::unordered_map<size_t, double> documentScores = parser.evaluateRPN(searcher, rpn);
    std::vector<std::pair<size_t, double>> result = {};
    std::vector<std::pair<size_t, double>> expected = {{5, 1.032}, {3, 1.174}, {1, 1.174}, {0, 1.237}};

    for (auto x : documentScores) {
        result.push_back({x.first, x.second});
    }
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i].first, expected[i].first);
        EXPECT_NEAR(result[i].second, expected[i].second, 0.1);
    }
}

TEST(SearcherTests, EvaluationOfEmptyRPN) {
    std::string storage = "SearchEngineData";
    fs::create_directory(storage);
    std::filesystem::path dirPath = (std::filesystem::path(__FILE__).remove_filename()) /= "testing_folder";
    Indexer indexer = Indexer(dirPath.string());
    indexer.startIndexer();

    fs::path temp_path(dirPath);
    std::string storage_name = temp_path.filename().string();

    std::string file_path = "SearchEngineData\\" + storage_name + "\\file_data.ind";
    std::string trie_path = "SearchEngineData\\" + storage_name + "\\trie_data.ind";

    Searcher searcher;
    searcher.loadStorage(file_path, trie_path);

    QueryParser parser;
    std::string user_query = "";
    std::vector<std::string> query = parser.tokenizeQuery(user_query);
    std::vector<std::string> rpn = parser.convertToRPN(query);
    std::unordered_map<size_t, double> documentScores = parser.evaluateRPN(searcher, rpn);
    int doc_cnt = 0;
    for (auto x : documentScores) {
        ++doc_cnt;
    }
    EXPECT_EQ(doc_cnt, 0);
}