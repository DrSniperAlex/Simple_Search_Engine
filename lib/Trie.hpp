#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

const int ALPHABET_SIZE = 27; // Including "_" as the 27th character
const uint8_t nullFlag = 0;
const uint8_t nonNullFlag = 1;

class Trie {
private:

    class TrieNode {
    public:
        std::vector<std::unique_ptr<TrieNode>> children;
        std::vector<std::vector<size_t>> positions;
        bool isEndOfWord;

        TrieNode() 
            : children(ALPHABET_SIZE)
            , positions()
            , isEndOfWord(false)
        {}

        ~TrieNode() = default;
    };

    std::unique_ptr<TrieNode> root;

    void insertHelper(TrieNode* node, const std::string& word, size_t index, size_t documentId, size_t position) {
        if (index == word.size()) {
            node->isEndOfWord = true;
            if (node->positions.size() <= documentId) {
                node->positions.resize(documentId + 1);
            }
            node->positions[documentId].push_back(position);
            return;
        }

        char currentChar = word[index];
        int childIndex = (currentChar == '_') ? ALPHABET_SIZE - 1 : currentChar - 'a';
        if (!node->children[childIndex]) {
            node->children[childIndex] = std::make_unique<TrieNode>();
        }
        insertHelper(node->children[childIndex].get(), word, index + 1, documentId, position);
    }

    void serializeNode(std::ofstream& outFile, const TrieNode* node) const {
        if (!node) {
            outFile.write(reinterpret_cast<const char*>(&nullFlag), sizeof(nullFlag));
            return;
        }

        outFile.write(reinterpret_cast<const char*>(&nonNullFlag), sizeof(nonNullFlag));
        outFile.write(reinterpret_cast<const char*>(&node->isEndOfWord), sizeof(node->isEndOfWord));
        size_t positionsSize = node->positions.size();
        outFile.write(reinterpret_cast<const char*>(&positionsSize), sizeof(positionsSize));
        for (const auto& positions : node->positions) {
            size_t positionsCount = positions.size();
            outFile.write(reinterpret_cast<const char*>(&positionsCount), sizeof(positionsCount));
            outFile.write(reinterpret_cast<const char*>(positions.data()), positions.size() * sizeof(size_t));
        }

        for (const auto& child : node->children) {
            serializeNode(outFile, child.get());
        }
    }

    static std::unique_ptr<TrieNode> deserializeNode(std::ifstream& inFile) {
        uint8_t flag;
        inFile.read(reinterpret_cast<char*>(&flag), sizeof(flag));

        if (flag == nullFlag) {
            return nullptr;
        }

        auto node = std::make_unique<TrieNode>();
        inFile.read(reinterpret_cast<char*>(&node->isEndOfWord), sizeof(node->isEndOfWord));

        size_t positionsSize;
        inFile.read(reinterpret_cast<char*>(&positionsSize), sizeof(positionsSize));
        node->positions.resize(positionsSize);
        for (size_t i = 0; i < positionsSize; ++i) {
            size_t positionsCount;
            inFile.read(reinterpret_cast<char*>(&positionsCount), sizeof(positionsCount));
            node->positions[i].resize(positionsCount);
            inFile.read(reinterpret_cast<char*>(node->positions[i].data()), positionsCount * sizeof(size_t));
        }

        for (auto& child : node->children) {
            child = deserializeNode(inFile);
        }

        return node;
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}

    Trie(const Trie& other) = delete;
    Trie(Trie&& other) noexcept = default;
    
    Trie& operator=(const Trie& other) {
        if (this != &other) {
            // Clear current Trie data
            root.reset();

            // Perform deep copy
            if (other.root) {
                root = cloneNode(other.root.get());
            }
        }
        return *this;
    }

    std::unique_ptr<TrieNode> cloneNode(const TrieNode* node) {
        if (!node) {
            return nullptr;
        }

        auto clonedNode = std::make_unique<TrieNode>();
        clonedNode->isEndOfWord = node->isEndOfWord;
        clonedNode->positions = node->positions;

        for (const auto& child : node->children) {
            clonedNode->children.push_back(cloneNode(child.get()));
        }

        return clonedNode;
    }

    Trie& operator=(Trie&& other) noexcept = default;

    ~Trie() = default;

    void insert(const std::string& word, size_t documentId, size_t position) {
        insertHelper(root.get(), word, 0, documentId, position);
    }

    void serialize(const std::string& filename) const {
        std::ofstream outFile(filename, std::ios::binary | std::ios::ate);
        if (!outFile) {
            std::cerr << "Error: Unable to create file for writing trie data\n";
            return;
        }

        serializeNode(outFile, root.get());
    }

    static Trie deserialize(const std::string& filename) {
        Trie trie;

        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open file for reading\n";
            return trie;
        }

        trie.root = deserializeNode(inFile);
        return trie;
    }

    size_t getTermFrequency(const std::string& word, size_t documentId) const {
        TrieNode* current = root.get();
        for (char c : word) {
            int index = (c == '_') ? ALPHABET_SIZE - 1 : c - 'a';
            if (!current->children[index]) {
                return 0;
            }
            current = current->children[index].get();
        }
        if (current->positions.empty()) {
            return 0;
        }
        return current->positions[documentId].size();
    }

    size_t getDocFrequency(const std::string& word) const {
        TrieNode* current = root.get();
        for (char c : word) {
            int index = (c == '_') ? ALPHABET_SIZE - 1 : c - 'a';
            if (!current->children[index]) {
                return 0;
            }
            current = current->children[index].get();
        }
        size_t doc_num = 0;
        for (auto temp : current->positions) {
            if (!temp.empty()) {
                ++doc_num;
            }
        }
        return doc_num;
    }

    friend std::ostream& operator<<(std::ostream& os, const Trie& trie) {
        os << "Saved words in Trie:\n";
        trie.preorderTraversal(os, trie.root.get(), "");
        return os;
    }

private:
    void preorderTraversal(std::ostream& os, TrieNode* node, std::string prefix) const {
        if (!node) return;
        
        if (node->isEndOfWord) {
            os << prefix << " [";
            for (size_t docId = 0; docId < node->positions.size(); ++docId) {
                if (!node->positions[docId].empty()) {
                    os << "File " << docId << ": {";
                    for (size_t pos : node->positions[docId]) {
                        os << " " << pos;
                    }
                    os << " } ";
                }
            }
            os << "]" << std::endl;
        }
        
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            char ch = (i == ALPHABET_SIZE - 1) ? '_' : 'a' + i;
            preorderTraversal(os, node->children[i].get(), prefix + ch);
        }
    }
};