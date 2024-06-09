add_library(
        tlib
        SearchEngine.hpp
        Indexer.hpp
        Searcher.hpp
        QueryParser.hpp
        Trie.hpp
        ConsolePrinter.hpp
)

target_link_libraries(
        tlib
        PRIVATE ftxui::dom ftxui::screen ftxui::component
)