#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include <string>
class Camera;

class Map {
public:
    Map() = default;
    Map(const char* const* rowsCStr, int rows, int cols, int tile = 16);
    bool loadFromFile(const std::string& path, int tile, std::string* error = nullptr);
    void render(SDL_Renderer* r) const;
    void render(SDL_Renderer* r, const Camera& cam) const;
    bool isWallAtPixel(float px, float py) const;
    void setCell(int row, int col, uint8_t v);
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    int tileSize() const { return tile_; }
    int worldPixelWidth()  const { return cols_ * tile_; }
    int worldPixelHeight() const { return rows_ * tile_; }
    bool loaded() const { return !grid_.empty(); }

private:
    int rows_{0}, cols_{0}, tile_{16};
    std::vector<uint8_t> grid_; // row-major, 1 = wall, 0 = empty
    static uint8_t decodeTile(char ch); // map file legend
    bool inBounds(int r, int c) const { return r>=0 && c>=0 && r<rows_ && c<cols_; }
    uint8_t at(int r, int c) const { return grid_[r*cols_ + c]; }
    uint8_t& at(int r, int c)      { return grid_[r*cols_ + c]; }
};
