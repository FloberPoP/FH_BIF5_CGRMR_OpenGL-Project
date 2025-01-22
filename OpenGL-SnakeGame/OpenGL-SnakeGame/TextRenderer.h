#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

class TextRenderer {
public:
    static void renderTextOnScreen(const std::string& text, float x, float y, const float* color);
    static void writeScoreToFile(int score);
    static std::vector<int> readScoresFromFile();
    static void renderScoreboard(const std::vector<int>& scores, int currentScore);

private:
    static const std::string SCOREBOARD_FILE;
};
