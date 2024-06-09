#pragma once

#include <iostream>
#include "QueryParser.hpp"
#include "Searcher.hpp"

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/dom/table.hpp"

using namespace ftxui;

class ConsolePrinter
{
private:
    const std::string text_1 = "Overall that's a simple Search Engine that uses Indexing as it's main optimization in performance and speed.\n";
    const std::string text_2 = "It uses Okapi BM25 ranging function to provide you the most relevant of the total amount of files in the directory.\n";
    const std::string text_3 = "It saves Indexed data in the Trie (= Prefix Tree) and stores it in the \"SearchEngineData\" folder in the root directory.\n";
    const std::string text_4 = "You can exit the Search Engine by pressing the \"Exit\" button down below.\n";
    const std::string text_5 = "  HOW TO USE EARCH ENGINE:\n";
    const std::string text_6 = "1) Write the directory path in the \"Directory Input\" section, that you would like to search in and press \"Enter\".\n";
    const std::string text_7 = "2) Wait a moment, while the Search Engine indexes the directory. You can see the progress made with the progress bar.\n";
    const std::string text_8 = "3) Write the search query you would like to find (a.k.a. terms/words, you'd like to find) and press \"Enter\".";
    const std::string text_9 = "4) You'll be presented with the table of all files, that contain your query in order from MOST relevant to LEAST relevant.";
    const std::string text_10 = "*Note, that the query should contain only AND-, OR- operators and be logically-correct:" ;
    const std::string text_11 = "   For example: \"for\", \"vector OR lSist\", \"(while OR for) AND vector\" - are correct";
    const std::string text_12 = "   \"for AND\", \"for AND OR list\", \"vector Or list\" - are incorrect";
    struct ColorPalette {
        Color color1;
        Color color2;
        Color color3;
        Color error_color_1;
        Color error_color_2;
    };
    std::vector<ColorPalette> ColorPalettes = {
        {Color::Aquamarine3, Color::White, Color::Aquamarine1, Color::Aquamarine3, Color::Black},        // White & Aquamarine (default)
        {Color::Salmon1, Color::White, Color::LightSalmon1, Color::Salmon1, Color::DarkRed},             // White & Salmon
        {Color::DarkOliveGreen3, Color::White, Color::PaleGreen3, Color::DarkOliveGreen3, Color::Black}, // White & Green
        {Color::SkyBlue3, Color::White, Color::SkyBlue1, Color::SkyBlue1, Color::DarkBlue},              // White & Blue
        {Color::MediumPurple3, Color::White, Color::MediumPurple1, Color::MediumPurple3, Color::Black},  // White & Purple
    };
    std::vector<std::string> PaletteNames = {
        " Morning Breeze ", " Shining Sunset ", " Living Forest ", " Cleansing Rain ", " Night Owl "
    };
    ColorPalette Palette = ColorPalettes[0];
    int palette_id = 0;
    std::string directory_path = "";
    int has_error = 0; // 0 - no error, 1 - invalid directory path, 2 - empty directory path, 3 - invalid search query

public:
    std::string StartPrinter() {
        auto screen = ScreenInteractive::TerminalOutput();
        // Search Engine Title
        auto title = Renderer([&] {
            return text("Simple Search Engine") | color(Palette.color2) 
                   | flex | bold | center;
        });
        // Instructions
        auto instructions = Renderer([&] {
            return window(text("Instructions:"), 
                          vbox({
                            text(text_1) | color(Color::GrayLight),
                            text(text_2) | color(Color::GrayLight),
                            text(text_3) | color(Color::GrayLight),
                            text(text_4) | color(Color::GrayLight),
                            text(text_5) | color(Palette.color3),
                            text(text_6) | color(Palette.color2),
                            text(text_7) | color(Palette.color2),
                            text(text_8) | color(Palette.color2),
                            text(text_9) | color(Palette.color2),
                            text(text_10) | color(Palette.color3),
                            text(text_11) | color(Palette.color1),
                            text(text_12) | color(Palette.color1)
                          })
                          ) | color(Palette.color3)
                            | size(HEIGHT, EQUAL, 14);
        });
        // Directory Input
        std::string directoryPath = "";
        auto dir_input = Input(&directoryPath, "C:\\example_folder\\example_subfolder");
        dir_input = Wrap("Directory path:", dir_input);
        dir_input |= CatchEvent([&](Event event) {
            if (event.input() == "\n") {
                screen.ExitLoopClosure()();
                return true;
            }  
            return false;
        });
        // Search Query Input
        std::string user_query;
        auto query_input = Input(&user_query, "any terms you'd like to search (e.g. \"for AND map\")");
        query_input = Wrap("Search terms:", query_input);
        query_input |= CatchEvent([&](Event event) {
            if (event.input() == "\n") {
                screen.ExitLoopClosure()();
                return true;
            }  
            return false;
        });
        // Exit Button
        bool exit_is_active = false;
        auto exit_action = [&] { 
            exit_is_active = true;
            screen.ExitLoopClosure()();
        };
        auto exit_button = Button("Exit Search Engine", exit_action, ButtonOption::Animated(Palette.color1)) 
                        | border
                        | size(WIDTH, EQUAL, 22);
        // Color Palette Change
        int saved_palette_id = palette_id;
        auto palette_menu = Menu(&PaletteNames, &palette_id, MenuOption::HorizontalAnimated());
        auto palette_title = Renderer([&] {
            return text("Choose a palette:") | color(Palette.color3) 
                   | flex | bold | center;
        });
        // Change Color Palette Button
        bool is_palette_change = false;
        auto change_action = [&] { 
            is_palette_change = true;
            screen.ExitLoopClosure()();
        };
        auto change_button = Button("Change Palette", change_action, ButtonOption::Animated(Palette.color3)) 
                        | border
                        | center
                        | size(WIDTH, EQUAL, 20);
        auto palette_layout = Container::Vertical({
            palette_title,
            palette_menu
        }) | borderRounded | size(WIDTH, EQUAL, 80);
        // Whole lower layout
        auto lower_layout = Container::Horizontal({
            palette_layout,
            change_button,
            exit_button
        }) | xflex;
        // Error message (if it occurs)
        Element error_message;
        if (has_error == 1) {
            error_message = text("Error: Invalid directory path. Please enter valid folder path.") 
                | bgcolor(Palette.error_color_1) | color(Palette.error_color_2) | xflex;
        } else if (has_error == 2) {
            error_message = text("Error: Empty directory path. Please enter folder path first before searching.") 
                | bgcolor(Palette.error_color_1) | color(Palette.error_color_2) | xflex;
        } else {
            error_message = text("") 
                | xflex;
        }

        auto error = Renderer([&] {
                return hbox({
                    error_message
                }) | xflex;
            });
        // Layout
        auto layout = Container::Vertical({
            title,
            instructions,
            dir_input,
            query_input,
            lower_layout,
            error
        });
        // Main render component
        auto component = Renderer(layout, [&] {
            return vbox({
                    title->Render(),
                    instructions->Render(),
                    dir_input->Render(),
                    query_input->Render(),
                    lower_layout->Render(),
                    separator(),
                    error->Render()
                }) | flex | border | color(Palette.color2);
        });
        screen.Loop(component);
        if (saved_palette_id != palette_id) {
            Palette = ColorPalettes[palette_id];
        }
        // Check if the user wants to quit
        if (exit_is_active)
            exit(0);
        // Check if it's just a palette change
        if (is_palette_change) {
            is_palette_change = false;
            clearScreen();
            directory_path = StartPrinter();
            return directory_path;
        }
        // Check if the directory is viable for Indexing
        if (directoryPath.empty()) {
            has_error = 2;
            clearScreen();
            directory_path = StartPrinter(); // Inform user and try again
        } else if (!checkDirectoryPath(directoryPath)) {
            has_error = 1;
            clearScreen();
            directory_path = StartPrinter(); // Inform user and try again
        } else {
            has_error = 0;
            directory_path = directoryPath; // Accept current directory path
        }
        return directory_path;
    }

    void printProgressBar(const float& progress) {
        auto document = vbox({
            text("Indexing..."),
            border(gauge(progress) | color(Palette.color1)) | size(WIDTH, EQUAL, 100) | color(Palette.color2),
        });
        auto screen = Screen::Create(Dimension::Fit(document));
        Render(screen, document);
        screen.Print();
    }

    std::string SearchQuery() {
        auto screen = ScreenInteractive::TerminalOutput();
        // Search Engine Title
        auto title = Renderer([&] {
            return text("Simple Search Engine") | color(Palette.color2) 
                   | flex | bold | center;
        });
        // Instructions
        auto instructions = Renderer([&] {
            return window(text("Instructions:"), 
                          vbox({
                            text(text_1) | color(Color::GrayLight),
                            text(text_2) | color(Color::GrayLight),
                            text(text_3) | color(Color::GrayLight),
                            text(text_4) | color(Color::GrayLight),
                            text(text_5) | color(Palette.color3),
                            text(text_6) | color(Palette.color2),
                            text(text_7) | color(Palette.color2),
                            text(text_8) | color(Palette.color2),
                            text(text_9) | color(Palette.color2),
                            text(text_10) | color(Palette.color3),
                            text(text_11) | color(Palette.color1),
                            text(text_12)| color(Palette.color1)
                          })
                          ) | color(Palette.color3)
                            | size(HEIGHT, EQUAL, 14);
        });
        // Directory Input
        auto dir = Renderer([&] {
            return hbox({
                text("Current directory: ") | color(Palette.color2) 
                    | size(WIDTH, EQUAL, 20) | bold,
                    separator(),
                text(directory_path) | color(Palette.color3) | xflex
            }) | xflex | borderDouble | bold;
        });
        // Search Query Input
        std::string user_query;
        auto query_input = Input(&user_query, "any terms you'd like to search (e.g. \"for AND map\")");
        query_input = Wrap("Search terms:", query_input);
        query_input |= CatchEvent([&](Event event) {
            if (event.input() == "\n") {
                screen.ExitLoopClosure()();
                return true;
            }  
            return false;
        });
        // Exit Button
        bool exit_is_active = false;
        auto action = [&] { 
            exit_is_active = true;
            screen.ExitLoopClosure()();
        };
        auto exit_button = Button("Exit Search Engine", action, ButtonOption::Animated(Palette.color1)) 
                        | border
                        | size(WIDTH, EQUAL, 22);
        // Color Palette Change
        int saved_palette_id = palette_id;
        auto palette_menu = Menu(&PaletteNames, &palette_id, MenuOption::HorizontalAnimated());
        auto palette_title = Renderer([&] {
            return text("Choose a palette:") | color(Palette.color3) 
                   | flex | bold | center;
        });
        // Change Color Palette Button
        bool is_palette_change = false;
        auto change_action = [&] { 
            is_palette_change = true;
            screen.ExitLoopClosure()();
        };
        auto change_button = Button("Change Palette", change_action, ButtonOption::Animated(Palette.color3)) 
                        | border
                        | center
                        | size(WIDTH, EQUAL, 20);
        auto palette_layout = Container::Vertical({
            palette_title,
            palette_menu
        }) | borderRounded | size(WIDTH, EQUAL, 80);
        // Whole lower layout
        auto lower_layout = Container::Horizontal({
            palette_layout,
            change_button,
            exit_button
        }) | xflex;
        // Error message
        Element error_message;
        if (has_error == 3) {
            error_message = text("Error: Invalid search query. Please enter valid search query.") 
                | bgcolor(Palette.error_color_1) | color(Palette.error_color_2) | xflex;
        } else {
            error_message = text("") 
                | xflex;
        }
        Component error = Renderer([&] {
            return hbox({
                error_message,
            }) | xflex;
        });
        // Layout
        auto layout = Container::Vertical({
            title,
            instructions,
            dir,
            query_input,
            lower_layout,
            error
        });
        // Main render component
        auto component = Renderer(layout, [&] {
            return vbox({
                    title->Render(),
                    instructions->Render(),
                    dir->Render(),
                    query_input->Render(),
                    lower_layout->Render(),
                    separator(),
                    error->Render()
                }) | flex | border | color(Palette.color2);
        });
        screen.Loop(component);
        if (saved_palette_id != palette_id) {
            Palette = ColorPalettes[palette_id];
        }
        if (exit_is_active)
            exit(0);
        if (is_palette_change) {
            is_palette_change = false;
            clearScreen();
            SearchQuery();
        }
        return user_query;
    }

    bool printSearchResults(const std::vector<std::vector<std::string>>& table_data) {
        auto screen = ScreenInteractive::FitComponent();
        ColorPalette palette;
        // Table
        Table table(table_data);
        table.SelectAll().Border(LIGHT);
        // Make first row bold with a double border
        table.SelectColumn(0).Border(LIGHT);
        table.SelectRow(0).Decorate(bold);
        table.SelectRow(0).SeparatorVertical(LIGHT);
        table.SelectRow(0).Border(DOUBLE);
        table.SelectColumn(2).DecorateCells(align_right);
        // Alternate decorations between rows
        auto content = table.SelectRows(1, -1);
        content.DecorateCellsAlternateRow(color(Palette.color1), 3, 0);
        content.DecorateCellsAlternateRow(color(Palette.color2), 3, 1);
        content.DecorateCellsAlternateRow(color(Palette.color3), 3, 2);
        // Render final table
        auto document = table.Render();
        auto results = Renderer([&] {
            return hbox({
                document
            });
        });
        // Button
        bool is_active = false;
        auto action = [&] { 
            is_active = true;
            screen.ExitLoopClosure()();
        };
        auto button = Button("Search other terms", action, ButtonOption::Animated(Palette.color1)) 
                        | border
                        | size(WIDTH, EQUAL, 22);
        // Exit Button
        bool exit_is_active = false;
        auto exit_action = [&] { 
            exit_is_active = true;
            screen.ExitLoopClosure()();
        };
        auto exit_button = Button("Exit Search Engine", exit_action, ButtonOption::Animated(Palette.color3)) 
                        | border
                        | size(WIDTH, EQUAL, 22);
        auto buttons = Container::Horizontal({
            button,
            exit_button
        }) | xflex;
        // Layout
        auto layout = Container::Vertical({
            results,
            buttons
        });
        // Main render renderer
        auto renderer = Renderer(layout, [&] {
            return vbox({
                    layout->Render() | frame | flex,
                }) | border | color(Palette.color2);
        });
        screen.Loop(renderer);
        if (exit_is_active) {
            exit(0);
        }
        if (is_active) {
            std::system("cls");
            return true;
        }
        return false;
    }

    bool checkDirectoryPath(const std::string& directoryPath) {
        try {
            for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {}
        } catch (std::filesystem::__cxx11::filesystem_error ex) {
            has_error = 1;
            return false;
        }
        return true;
    }

    void clearScreen() {
        std::system("cls");
    }

    void setErrorType(int error_type) {
        has_error = error_type;
    }

private:
    Component Wrap(std::string name, Component component) {
        return Renderer(component, [name, component] {
            return hbox({
                    text(name) | size(WIDTH, EQUAL, 20),
                    separator(),
                    component->Render() | xflex,
                }) | xflex | borderDouble | bold;
        });
    }
};