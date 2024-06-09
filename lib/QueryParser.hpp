#pragma once

#include <iostream>
#include <stack>
#include "Searcher.hpp"

class QueryParser
{
public:
    std::vector<std::string> tokenizeQuery(const std::string& user_query) {
        std::istringstream iss(user_query);
        std::vector<std::string> tokens;
        std::string token = "";

        for (char c : user_query) {
            if (c == '(' || c == ')' || c == ' ' || c == '\0') {
                if (token != "") {
                    tokens.push_back(token); 
                    token.clear();
                }
                if (c == '(' || c == ')') {
                    tokens.push_back(std::string(1, c));
                }
            } else if (isspace(c)) {
                continue;
            } else {
                token += c; 
            }
        }
        if (token != "") {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::vector<std::string> convertToRPN(const std::vector<std::string>& tokens) {
        std::vector<std::string> rpn;
        std::stack<std::string> operators;
        bool flag = 0;

        for (const auto& token : tokens) {
            if (isOperator(token)) {
                if (!flag) {
                    throw invalidQuery;
                }
                flag = 0;
                while (!operators.empty() && precedence(operators.top()) >= precedence(token)) {
                    rpn.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            } else if (token == "(") {
                operators.push(token);
            } else if (token == ")") {
                while (!operators.empty() && operators.top() != "(") {
                    rpn.push_back(operators.top());
                    operators.pop();
                }
                operators.pop();
            } else {
                if (flag) {
                    throw invalidQuery;
                }
                flag = 1;
                rpn.push_back(token);
            }
        }

        while (!operators.empty()) {
            rpn.push_back(operators.top());
            operators.pop();
        }
        return rpn;
    }

    std::unordered_map<size_t, double> evaluateRPN(Searcher& searcher, const std::vector<std::string>& rpn) {
        std::stack<std::unordered_map<size_t, double>> operands;

        for (const auto& token : rpn) {
            if (isOperator(token)) {
                if (operands.empty()) {
                    throw invalidQuery;
                }   
                std::unordered_map<size_t, double> operand2 = operands.top();
                operands.pop();

                if (operands.empty()) {
                    throw invalidQuery;
                }   
                std::unordered_map<size_t, double> operand1 = operands.top();
                operands.pop();

                if (token == "AND") {
                    operands.push(intersect(operand1, operand2));
                } else if (token == "OR") {
                    operands.push(unite(operand1, operand2));
                }
            } else {
                std::unordered_map<size_t, double> docs = searcher.getMatchingDocuments(token);
                operands.push(docs);
            }
        }
        // Sum all bm25 scores for overall search query
        std::unordered_map<size_t, double> documentScores;
        while (!operands.empty()) {
            for (auto docId : operands.top()) {
                documentScores[docId.first] += docId.second;
            }
            operands.pop();
        }
        return documentScores;
    }

    std::vector<size_t> rankDocuments(const std::unordered_map<size_t, double>& documentScores) {
        std::vector<std::pair<size_t, double>> pairs(documentScores.begin(), documentScores.end());
        std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        std::vector<size_t> rankedDocuments;
        for (const auto& pair : pairs) {
            rankedDocuments.push_back(pair.first);
        }
        return rankedDocuments;
    }

private:
    bool isOperator(const std::string& token) {
        return token == "AND" || token == "OR";
    }

    int precedence(const std::string& op) {
        if (op == "AND") return 2;
        if (op == "OR") return 1;
        return 0; // 0 = query terms
    }

    std::unordered_map<size_t, double> intersect(const std::unordered_map<size_t, double>& map1, const std::unordered_map<size_t, double>& map2) {
        std::unordered_map<size_t, double> result;
        for (auto elem : map1) {
            if (map2.count(elem.first) > 0) {
                result.insert(elem);
            }
        }
        return result;
    }

    std::unordered_map<size_t, double> unite(const std::unordered_map<size_t, double>& map1, const std::unordered_map<size_t, double>& map2) {
        std::unordered_map<size_t, double> result = map1;
        for (auto elem : map2) {
            result.insert(elem);
        }
        return result;
    }

    std::exception invalidQuery;
};