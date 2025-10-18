#include "Application.h"
#include "classes/Logger.hpp"
#include "classes/TicTacToe.h"
#include "imgui/imgui.h"

// In addition to the implementation in TicTacToe.cpp, I have also added in my logger class from the previous assignment (with the UI bundled into the class), and adjusted the buildscript/main_macos to theoretically work on linux (though my linux installation is broken so I couldn't test it and can only confirm that this works on windows).

namespace ClassGame {
    //
    // our global variables
    //
    TicTacToe* game       = nullptr;
    bool       gameOver   = false;
    int        gameWinner = -1;

    //
    // game starting point
    // this is called by the main render loop in main.cpp
    //
    void GameStartUp() {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("resources/Noto_Sans/NotoSans-Regular.ttf",
                                     20.0f);

        game = new TicTacToe();
        game->setUpBoard();
    }

    //
    // game render loop
    // this is called by the main render loop in main.cpp
    //
    void RenderGame() {
        ImGui::DockSpaceOverViewport();

        // ImGui::ShowDemoWindow();

        if (!game)
            return;
        if (!game->getCurrentPlayer())
            return;

        ImGui::Begin("Settings");
        ImGui::Text("Current Player Number: %d",
                    game->getCurrentPlayer()->playerNumber());
        ImGui::Text("Current Board State: %s", game->stateString().c_str());

        if (gameOver) {
            ImGui::Text("Game Over!");
            ImGui::Text("Winner: %d", gameWinner);
            if (ImGui::Button("Reset Game")) {
                game->stopGame();
                game->setUpBoard();
                gameOver   = false;
                gameWinner = -1;
            }
        }
        ImGui::End();

        ImGui::Begin("GameWindow", nullptr);
        game->drawFrame();
        ImGui::End();

        Logger::GetInstance().UI();
    }

    //
    // end turn is called by the game code at the end of each turn
    // this is where we check for a winner
    //
    void EndOfTurn() {
        Player* winner = game->checkForWinner();
        if (winner) {
            gameOver   = true;
            gameWinner = winner->playerNumber();
            Logger::GetInstance().LogGameEventInfo("Game over. Won by player {}", winner->playerNumber());
        }
        if (game->checkForDraw()) {
            gameOver   = true;
            gameWinner = -1;
            Logger::GetInstance().LogGameEventInfo("Game over. Draw.");
        }
    }
} // namespace ClassGame
