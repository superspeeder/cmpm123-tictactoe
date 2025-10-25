#include "TicTacToe.h"

#include "classes/Logger.hpp"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------


/*
 * IMPLEMENTATION DESCRIPTION
 *
 * For the most part, I implemented this class by following the provided TODOs.
 * I did do a couple things that weren't in the TODOs:
 * - I added some logging to various actions (for debugging purposes).
 * - I added a new helper function called `boardCheckHelper`, which performs both the isDraw check and the isWon check at the same time (which can be used to improve performance by not doing extra unnecessary work).
 * - I use a char array in my state string generation function instead of an std::string (only creating an std::string at the end of the function when I return the array) to avoid unnecessary heap allocations (in this case, there *shouldn't* be any more than if I just used the string normally, but this way I can't accidentally create any).
 * - I added a bit of padding to the top of the grid (24 pixels) so that the grid wasn't overlapping with the imgui window title.
 */

const int AI_PLAYER    = 1;  // index of the AI player (O)
const int HUMAN_PLAYER = -1; // index of the human player (X)

TicTacToe::TicTacToe() {}

TicTacToe::~TicTacToe() {}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber) {
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard() {
    // here we should call setNumberOfPlayers to 2 and then set up the game options so the mouse knows to draw a 3x3 grid
    // _gameOptions has a rowX and rowY property we should set to 3
    // then we need to setup our 3x3 array in _grid with the correct position of the square, and load the "square.png" sprite for each square
    // we will use the initHolder function on each square to do this
    // finally we should call startGame to get everything going

    setNumberOfPlayers(2);
    setAIPlayer(1);

    _gameOptions.rowX = _gameOptions.rowY = 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            _grid[i][j].initHolder(ImVec2(j * 100.0f, i * 100.0f + 24.0f), "square.png", j, i);
        }
    }

    Logger::GetInstance().LogGameEventInfo("Game board set up");
    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder* holder) {
    // 1) Guard clause: if holder is nullptr, fail fast.
    //    (Beginner hint: always check pointers before using them.)
    //    if (!holder) return false;
    if (!holder) return false;

    // 2) Is it actually empty?
    //    Ask the holder for its current Bit using the bit() function.
    //    If there is already a Bit in this holder, return false.
    if (!holder->empty()) return false;

    // 3) Place the current player's piece on this holder:
    //    - Figure out whose turn it is (getCurrentPlayer()->playerNumber()).
    //    - Create a Bit via PieceForPlayer(currentPlayerIndex).
    //    - Position it at the holder's position (holder->getPosition()).
    //    - Assign it to the holder: holder->setBit(newBit);
    if (!getCurrentPlayer()) return false;

    Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    bit->setPosition(holder->getPosition());
    holder->setBit(bit);
    Logger::GetInstance().LogGameEventInfo("Player {} placed bit at ({}, {})", getCurrentPlayer()->playerNumber(),
                                           holder->getPosition().x, holder->getPosition().y);


    // 4) Return whether we actually placed a piece. true = acted, false = ignored.
    return true; // replace with true if you complete a successful placement
}

bool TicTacToe::canBitMoveFrom(Bit* bit, BitHolder* src) {
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder* src, BitHolder* dst) {
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame() {
    // clear out the board
    // loop through the 3x3 array and call destroyBit on each square
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            _grid[i][j].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index) const {
    // index is 0..8, convert to x,y using:
    // y = index / 3
    // x = index % 3
    // if there is no bit at that location (in _grid) return nullptr
    // otherwise return the owner of the bit at that location using getOwner()

    int x = index % 3;
    int y = index / 3;
    if (y >= 3) return nullptr;

    const BitHolder& holder = _grid[y][x];

    Bit* bit = holder.bit();
    if (!bit) return nullptr;

    return bit->getOwner();
}

/**
 * @brief Helper for checking board state
 * @param isDraw optional output for checking if the board is in a draw state (full with no winner)
 * @return The winning player (or nullptr if no player has won).
 */
Player* TicTacToe::boardCheckHelper(bool* isDraw) {
    static constexpr int WINNING_TRIPLES[8][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6},
    };

    /// These are the remaining indices to check for the isFull check (which is used to determine draws). This is necessary because not all squares are checked (but this way we do less overall checks than iterating over all squares after this part).
    static constexpr int REMAINING_CHECKS[4] = {4, 5, 7, 8};

    bool isFull = true;

    for (unsigned i = 0; i < 8; i++) {
        // Check if the bits represented by the triple are the same player, if so that player has won.
        const int* triple = WINNING_TRIPLES[i];
        Player*    p      = ownerAt(triple[0]);
        if (!p) {
            // bit holder is empty, board is not full
            isFull = false;
            continue;
        }

        if (ownerAt(triple[1]) != p) continue;
        if (ownerAt(triple[2]) != p) continue;

        if (isDraw) {
            *isDraw = false; // a player has won, not a draw
        }
        Logger::GetInstance().LogGameEventInfo("Detected win by player {} with triple ({}, {}, {})", p->playerNumber(),
                                               triple[0], triple[1], triple[2]);
        return p;
    }


    // Check the extra bits for the isDraw check (only run if the isDraw parameter is not a nullptr, since this is otherwise unnecessary).
    if (isDraw) {
        for (unsigned i = 0; i < 4; i++) {
            if (!ownerAt(REMAINING_CHECKS[i])) {
                isFull = false;
            }
        }

        *isDraw = isFull; // no winning triple, only a draw if the board is full.
    }

    return nullptr;
}

Player* TicTacToe::checkForWinner() {
    // check all the winning triples
    // if any of them have the same owner return that player
    // winning triples are:
    // 0,1,2
    // 3,4,5
    // 6,7,8
    // 0,3,6
    // 1,4,7
    // 2,5,8
    // 0,4,8
    // 2,4,6
    // you can use the ownerAt helper function to get the owner of a square
    // for example, ownerAt(0) returns the owner of the top-left square
    // if there is no bit in that square, it returns nullptr
    // if you find a winning triple, return the player who owns that triple
    // otherwise return nullptr

    // Hint: Consider using an array to store the winning combinations
    // to avoid repetitive code

    // the code for this function has been abstracted to avoid any duplication or double-work.

    return boardCheckHelper(nullptr);
}

bool TicTacToe::checkForDraw() {
    // is the board full with no winner?
    // if any square is empty, return false
    // otherwise return true
    bool isDraw;
    boardCheckHelper(&isDraw);
    return isDraw;
}

//
// state strings
//
std::string TicTacToe::initialStateString() {
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const {
    // return a string representing the current state of the board
    // the string should be 9 characters long, one for each square
    // each character should be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    // the order should be left-to-right, top-to-bottom
    // for example, the starting state is "000000000"
    // if player 1 has placed an X in the top-left and player 2 an O in the center, the state would be "100020000"
    // you can build the string using a loop and the to_string function
    // for example, to convert an integer to a string, you can use std::to_string(1) which returns "1"
    // you can get the bit at each square using _grid[y][x].bit()
    // if the bit is not null, you can get its owner using bit->getOwner()->playerNumber()
    // remember that player numbers are zero-based, so add 1 to get '1' or '2'
    // if the bit is null, add '0' to the string
    // finally, return the constructed string
    char state[10];
    state[9] = '\0';
    for (int i = 0; i < 9; i++) {
        Player* p = ownerAt(i);
        if (!p) {
            state[i] = '0';
        }
        else {
            state[i] = static_cast<char>(p->playerNumber() + '1');
            // quick way to get the player number as a character (since the current state string won't even work with player numbers >= 10).
        }
    }
    return state;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string& s) {
    // set the state of the board from the given string
    // the string will be 9 characters long, one for each square
    // each character will be '0' for empty, '1' for player 1 (X), and '2' for player 2 (O)
    // the order will be left-to-right, top-to-bottom
    // for example, the starting state is "000000000"
    // if player 1 has placed an X in the top-left and player 2 an O in the center, the state would be "100020000"
    // you can loop through the string and set each square in _grid accordingly
    // for example, if s[0] is '1', you would set _grid[0][0] to have player 1's piece
    // if s[4] is '2', you would set _grid[1][1] to have player 2's piece
    // if s[8] is '0', you would set _grid[2][2] to be empty
    // you can use the PieceForPlayer function to create a new piece for a player
    // remember to convert the character to an integer by subtracting '0'
    // for example, int playerNumber = s[index] - '0';
    // if playerNumber is 0, set the square to empty (nullptr)
    // if playerNumber is 1 or 2, create a piece for that player and set it in the square
    // finally, make sure to position the piece at the holder's position
    // you can get the position of a holder using holder->getPosition()
    // loop through the 3x3 array and set each square accordingly
    // the string should always be valid, so you don't need to check its length or contents
    // but you can assume it will always be 9 characters long and only contain '0', '1', or '2'
    for (int i = 0; i < 9; i++) {
        int        pn     = s.at(i) - '0';
        int        x      = i % 3;
        int        y      = i / 3;
        BitHolder& holder = _grid[y][x];
        if (pn == 0) {
            holder.destroyBit();
        }
        else {
            Bit* bit = PieceForPlayer(pn);
            bit->setPosition(holder.getPosition());
            holder.setBit(bit);
        }
    }

    Logger::GetInstance().LogGameEventInfo("Game state set via string \"{}\"", s);
}

static int negamax(std::string& state, const int depth, const int player_color);

//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() {
    auto state       = stateString();
    int  best_move   = -1000;
    int  best_square = -1;

    for (int i = 0; i < 9; i++) {
        if (state[i] != '0') continue;

        state[i]   = '2';
        const int result = -negamax(state, 0, HUMAN_PLAYER);
        Logger::GetInstance().LogGameEventInfo("Space {} has value {}", i, result);
        if (result > best_move) {
            best_move   = result;
            best_square = i;
        }
        state[i] = '0';
    }

    if (best_square != -1) {
        int x = best_square % 3;
        int y = best_square / 3;
        actionForEmptyHolder(&getHolderAt(x, y));
        endTurn();
    }
}

static char check_winner(const std::string& state) {
    static constexpr int WINNING_TRIPLES[8][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6},
    };

    // Check for the winner over the triples. We also track a draw (indicated by a return value of 'd').
    bool full = true;
    for (unsigned i = 0; i < 8; i++) {
        const int* triple = WINNING_TRIPLES[i];
        const char s0     = state.at(triple[0]);
        if (s0 == '0') {
            full = false;
            continue;
        }
        const char s1 = state.at(triple[1]);
        const char s2 = state.at(triple[2]);
        if (s0 == s1 && s1 == s2) {
            return s0;
        }
    }

    for (unsigned i = 0; full && i < 9; i++) {
        if (state[i] == '0') full = false;
    }

    if (full) {
        return 'd';
    }

    // no winner
    return '0';
}


static int negamax(std::string& state, const int depth, const int player_color) {
    if (const char active_winner = check_winner(state); active_winner != '0') {
        // active_winner == '0' when the state is not a terminal state.
        if (depth <= 2) {
            Logger::GetInstance().LogGameEventInfo("Win within 2: {}", active_winner);
        }
        return active_winner == 'd' ? 0 : -10;
    }

    int value = -1000;
    for (int i = 0; i < 9; i++) {
        if (state[i] != '0') continue;
        state[i] = player_color == HUMAN_PLAYER ? '1' : '2';
        value    = std::max(value, -negamax(state, depth + 1, -player_color));
        state[i] = '0';
    }

    return value;
}
