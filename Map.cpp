#include "Map.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

// Legend (extend anytime):
//   '#' => wall
//   '.' => empty
//   ' ' => empty
//   (everything else defaults to empty)
uint8_t Map::decodeTile(char ch)
{
    switch (ch)
    {
        case '#': return 1;
        case '.': return 0;
        case ' ': return 0;
        default:  return 0;
    }
}

Map::Map(const char* const* rowsCStr, int rows, int cols, int tile): rows_(rows), cols_(cols), tile_(tile), grid_(rows * cols, 0)
{
    for (int r = 0; r < rows_; ++r)
    {
        const char* line = rowsCStr[r];
        for (int c = 0; c < cols_; ++c)
        {
            grid_[r * cols_ + c] = decodeTile(line[c]);
        }
    }
}

bool Map::loadFromFile(const std::string& path, int tile, std::string* error)
{
    std::ifstream in(path);
    if (!in)
    {
        if (error) *error = "Could not open file: " + path;
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // Skip lines starting with ';' or '//'
        if (!line.empty() && (line[0] == ';' || (line.size() >= 2 && line[0] == '/' && line[1] == '/')))
            continue;
        if (!line.empty()) lines.push_back(line);
    }
    if (lines.empty())
    {
        if (error) *error = "File had no map lines: " + path;
        return false;
    }

    // Determine width
    size_t maxW = 0;
    for (auto& s : lines) maxW = std::max(maxW, s.size());
    for (auto& s : lines) if (s.size() < maxW) s.append(maxW - s.size(), '.');

    rows_ = static_cast<int>(lines.size());
    cols_ = static_cast<int>(maxW);
    tile_ = tile;
    grid_.assign(rows_ * cols_, 0);

    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            grid_[r * cols_ + c] = decodeTile(lines[r][c]);

    return true;
}

void Map::render(SDL_Renderer* r) const
{
    int w = 0, h = 0;
    SDL_GetRenderOutputSize(r, &w, &h);
    const int endCol = std::min(cols_ - 1, (w + tile_ - 1) / tile_);
    const int endRow = std::min(rows_ - 1, (h + tile_ - 1) / tile_);

    SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
    for (int y = 0; y <= endRow; ++y)
    {
        for (int x = 0; x <= endCol; ++x)
        {
            if (at(y, x) == 1)
            {
                SDL_FRect fr{ float(x * tile_), float(y * tile_), float(tile_), float(tile_) };

                // Fill green
                SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
                SDL_RenderFillRect(r, &fr);

                // Draw border orange
                SDL_SetRenderDrawColor(r, 255, 165, 0, 255);
                SDL_RenderRect(r, &fr);
            }
        }
    }
}

bool Map::isWallAtPixel(float px, float py) const
{
    const int cx = int(std::floor(px / tile_));
    const int cy = int(std::floor(py / tile_));
    if (!inBounds(cy, cx)) return true;
    return at(cy, cx) == 1;
}

void Map::setCell(int row, int col, uint8_t v)
{
    if (inBounds(row, col)) at(row, col) = v;
}