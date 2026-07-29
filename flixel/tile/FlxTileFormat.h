#pragma once

#include <SDL.h>
#include <string>
#include <vector>

namespace flixel {
namespace tile {

inline std::vector<SDL_Rect> sliceTileFrames(int textureW, int textureH, int tileWidth, int tileHeight, int margin = 0, int spacing = 0) {
    std::vector<SDL_Rect> frames;
    if (tileWidth <= 0 || tileHeight <= 0) {
        return frames;
    }

    for (int ty = margin; ty + tileHeight <= textureH; ty += tileHeight + spacing) {
        for (int tx = margin; tx + tileWidth <= textureW; tx += tileWidth + spacing) {
            frames.push_back(SDL_Rect{tx, ty, tileWidth, tileHeight});
        }
    }

    return frames;
}

inline bool parseTileCSV(const std::string& csv, std::vector<int>& outData, int& outWidth, int& outHeight) {
    outData.clear();
    outWidth = 0;
    outHeight = 0;

    size_t lineStart = 0;
    while (lineStart <= csv.size()) {
        size_t lineEnd = csv.find('\n', lineStart);
        if (lineEnd == std::string::npos) {
            lineEnd = csv.size();
        }

        std::string line = csv.substr(lineStart, lineEnd - lineStart);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!line.empty()) {
            int columns = 0;
            size_t colStart = 0;
            while (colStart <= line.size()) {
                size_t colEnd = line.find(',', colStart);
                if (colEnd == std::string::npos) {
                    colEnd = line.size();
                }

                std::string cell = line.substr(colStart, colEnd - colStart);
                if (!cell.empty()) {
                    outData.push_back(std::atoi(cell.c_str()));
                    columns++;
                }

                if (colEnd == line.size()) {
                    break;
                }
                colStart = colEnd + 1;
            }

            if (columns > 0) {
                if (outWidth == 0) {
                    outWidth = columns;
                }
                outHeight++;
            }
        }

        if (lineEnd == csv.size()) {
            break;
        }
        lineStart = lineEnd + 1;
    }

    return outWidth > 0 && outHeight > 0 && outData.size() == static_cast<size_t>(outWidth * outHeight);
}

}
}
