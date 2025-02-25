#include "TextRenderer.h"
#include <glad/glad.h>
#include <GL/freeglut.h>
#include <vector>
#include <algorithm>

// Define the static member variable
const std::string TextRenderer::SCOREBOARD_FILE = "scores.txt";

// Function to draw text at a specific position
void TextRenderer::renderTextOnScreen(const std::string& text, float x, float y, const float* color) {
    glColor3fv(color); // Set the text color
    glRasterPos2f(x, y); // Set the position for text rendering

    // Render each character
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Write the current score to the file
void TextRenderer::writeScoreToFile(int score) {
    std::ofstream file(SCOREBOARD_FILE, std::ios::app);
    if (file.is_open()) {
        file << score << std::endl;
        file.close();
    }
}

// Read scores from the file and sort them in descending order
std::vector<int> TextRenderer::readScoresFromFile() {
    std::ifstream file(SCOREBOARD_FILE);
    std::vector<int> scores;
    int score;
    if (file.is_open()) {
        while (file >> score) {
            scores.push_back(score);
        }
        file.close();
    }
    std::sort(scores.rbegin(), scores.rend()); // Sort scores in descending order
    return scores;
}

// Render the scoreboard on the screen
void TextRenderer::renderScoreboard(const std::vector<int>& scores, int currentScore) {
    float y = 0.6f; // Starting Y position for displaying scores
    float defaultTextColor[] = { 1.0f, 1.0f, 1.0f }; // Color for top 5 scores
    float highlightTextColor[] = { 0.0f, 1.0f, 0.0f }; // Color for the current score in top 5
    float outsideTextColor[] = { 1.0f, 0.0f, 0.0f }; // Color for the current score outside top 5

    renderTextOnScreen("=== Scoreboard ===", -0.3f, y, defaultTextColor);
    y -= 0.1f;

    bool currentScoreDisplayed = false; // Track if the current score has been displayed
    int count = 0;

    for (int score : scores) {
        if (count >= 5) break; // Display only top 5 scores

        // Determine the color for the current score
        const float* textColor = (score == currentScore && !currentScoreDisplayed)
            ? highlightTextColor
            : defaultTextColor;

        renderTextOnScreen(std::to_string(count + 1) + ". " + std::to_string(score), -0.2f, y, textColor);
        y -= 0.1f;
        count++;

        // Mark the current score as displayed if it matches
        if (score == currentScore) {
            currentScoreDisplayed = true;
        }
    }

    // If the current score is not in the top 5, display it below
    if (!currentScoreDisplayed) {
        y -= 0.1f; // Add some space before displaying the current score
        renderTextOnScreen("Your Score: " + std::to_string(currentScore), -0.2f, y, outsideTextColor);
    }
}

