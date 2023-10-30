#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

struct Player {
    std::string name;
    int gamesPlayed = 0;
    int wordsGuessed = 0;
    int wordsMissed = 0;
};

std::string getRandomWord(const std::vector<std::string>& wordList) {
    int randomIndex = std::rand() % wordList.size();
    return wordList[randomIndex];
}

void drawHangman(int incorrectGuesses) {
    if (incorrectGuesses >= 1) std::cout << "  O" << std::endl;
    if (incorrectGuesses >= 2) std::cout << " /" << (incorrectGuesses >= 3 ? "|" : " ") << "\\" << std::endl;
    if (incorrectGuesses >= 4) std::cout << " /" << (incorrectGuesses >= 5 ? " \\" : " ") << std::endl;
}

std::string getHint(const std::string& word, const std::string& guessedWord) {
    // Check if the word is medium or hard difficulty (you can adjust the criteria)
    if (word.length() >= 5) {
        std::cout << "Choose a hint:" << std::endl;
        std::cout << "1. Reveal the first letter" << std::endl;
        std::cout << "2. Reveal the last letter" << std::endl;
        std::cout << "3. Reveal any one vowel" << std::endl;

        int hintChoice;
        std::cin >> hintChoice;

        if (hintChoice == 1) {
            // Hint: Reveal the first letter
            return word.substr(0, 1);
        } else if (hintChoice == 2) {
            // Hint: Reveal the last letter
            return word.substr(word.length() - 1, 1);
        } else if (hintChoice == 3) {
            // Hint: Reveal any one vowel
            for (char c : "aeiouAEIOU") {
                if (word.find(c) != std::string::npos && guessedWord.find(c) == std::string::npos) {
                    // Found an unguessed vowel, reveal it
                    return std::string(1, c);
                }
            }
        }
    }

    // No valid hint to provide
    return "No hint available for this word.";
}

void updateLeaderboard(Player& player, std::vector<Player>& leaderboard) {
    // Check if the player is already in the leaderboard
    for (Player& entry : leaderboard) {
        if (entry.name == player.name) {
            entry.gamesPlayed++;
            entry.wordsGuessed += player.wordsGuessed;
            entry.wordsMissed += player.wordsMissed;
            return;
        }
    }

    // If the player is not in the leaderboard, add them
    leaderboard.push_back(player);
}

void displayLeaderboard(const std::vector<Player>& leaderboard) {
    std::cout << "Leaderboard:" << std::endl;
    for (const Player& player : leaderboard) {
        std::cout << "Player: " << player.name << " | Games Played: " << player.gamesPlayed
                  << " | Words Guessed: " << player.wordsGuessed << " | Words Missed: " << player.wordsMissed << std::endl;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::vector<std::string> easyWords;
    std::vector<std::string> mediumWords;
    std::vector<std::string> hardWords;

    // Load words from a text file into the vectors
    std::ifstream wordFile("data.txt");
    if (wordFile.is_open()) {
        std::string word;
        while (std::getline(wordFile >> std::ws, word)) {
            // Remove leading and trailing whitespace
            word = word.substr(word.find_first_not_of(" \t\n\r\f\v"), word.find_last_not_of(" \t\n\r\f\v") + 1);
            if (word.length() >= 7) {
                hardWords.push_back(word);
            } else if (word.length() >= 5) {
                mediumWords.push_back(word);
            } else {
                easyWords.push_back(word);
            }
        }
        wordFile.close();
    } else {
        std::cerr << "Error opening words.txt." << std::endl;
        return 1;
    }

    int selectedLevel = 0;
    std::string wordToGuess;
    bool multiplayerMode = false; // Flag for multiplayer mode
    bool playAgain = true;
    bool usedHint = false; // Track if the player has used a hint

    // Create a leaderboard
    std::vector<Player> leaderboard;

    while (playAgain) {
        // Prompt the player to enter their name
        Player player;
        std::cout << "Enter your name: ";
        std::cin >> player.name;

        std::cout << "Welcome to Hangman, " << player.name << "!" << std::endl;
        std::cout << "Choose a game mode:" << std::endl;
        std::cout << "1. Single Player" << std::endl;
        std::cout << "2. Multiplayer" << std::endl;
        int gameMode;
        std::cin >> gameMode;

        if (gameMode == 2) {
            multiplayerMode = true;
            std::cout << "Player 1, enter the word for Player 2 to guess: ";
            std::cin >> wordToGuess;
            std::cin.ignore(10000, '\n');
        }

        if (gameMode == 1) {
            std::cout << "Choose a difficulty level:" << std::endl;
            std::cout << "1. Easy" << std::endl;
            std::cout << "2. Medium" << std::endl;
            std::cout << "3. Hard" << std::endl;

            std::cin >> selectedLevel;  // Move this line here

            const std::vector<std::string>* wordList = nullptr;

            if (selectedLevel == 1) {
                wordList = &easyWords;
            } else if (selectedLevel == 2) {
                wordList = &mediumWords;
            } else if (selectedLevel == 3) {
                wordList = &hardWords;
            } else {
                std::cout << "Invalid selection. Exiting." << std::endl;
                return 1;
            }

            wordToGuess = getRandomWord(*wordList);
        }

        std::string guessedWord(wordToGuess.length(), '_');
        int maxAttempts = 6;
        int attempts = 0;
        std::vector<char> incorrectGuesses;

        bool wordGuessed = false;

        while (attempts < maxAttempts) {
            std::cout << "Word: " << guessedWord << std::endl;
            drawHangman(attempts);
            std::cout << "Incorrect guesses: ";
            for (char incorrectGuess : incorrectGuesses) {
                std::cout << incorrectGuess << " ";
            }
            std::cout << std::endl;

            if (!usedHint && (selectedLevel == 2 || selectedLevel == 3)) {
                std::cout << "Do you want to use a hint? (1 for Yes, 0 for No): ";
                int useHint;
                std::cin >> useHint;
                if (useHint == 1) {
                    std::string hint = getHint(wordToGuess, guessedWord);
                    std::cout << "Hint: " << hint << std::endl;
                    usedHint = true;
                }
            }

            char guess;
            std::cout << "Guess a letter: ";
            std::cin >> guess;

            if (std::cin.fail() || !std::isalpha(guess)) {
                std::cout << "Invalid input. Please enter a letter." << std::endl;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            bool found = false;
            for (int i = 0; i < wordToGuess.length(); i++) {
                if (std::tolower(wordToGuess[i]) == std::tolower(guess)) {
                    guessedWord[i] = guess;
                    found = true;
                }
            }

            if (!found) {
                incorrectGuesses.push_back(guess);
                attempts++;
            }

            if (guessedWord.length() == wordToGuess.length() && guessedWord == wordToGuess) {
                wordGuessed = true;
                std::cout << "Congratulations! You've guessed the word: " << wordToGuess << std::endl;
                break;
            }
        }

        if (!wordGuessed) {
            std::cout << "You're out of attempts! The word was: " << wordToGuess << std::endl;
            drawHangman(attempts);
        }

        // Update player's statistics and the leaderboard
        player.gamesPlayed++;
        if (wordGuessed) {
            player.wordsGuessed++;
        } else {
            player.wordsMissed++;
        }
        updateLeaderboard(player, leaderboard);

        // Ask the player if they want to play again
        std::cout << "Do you want to play again? (1 for Yes, 0 for No): ";
        std::cin >> playAgain;
        if (playAgain) {
            // Clear previous game data and start a new game
            attempts = 0;
            incorrectGuesses.clear();
            wordGuessed = false;
            usedHint = false;
        }
    }

    // Display the leaderboard
    displayLeaderboard(leaderboard);

    return 0;
}
