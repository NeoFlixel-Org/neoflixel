#pragma once

#include "flixel/FlxObject.h"
#include <SDL.h>
#include <string>
#include <vector>

namespace flixel {
namespace tile {

class FlxTilemap : public flixel::FlxObject {
public:
    FlxTilemap();
    ~FlxTilemap() override;

    bool loadMapFromArray(const std::vector<int>& mapData, int widthInTiles, int heightInTiles,
                           const std::string& tileGraphicPath, int tileWidth = 0, int tileHeight = 0,
                           int collideIndex = 1, int tileMargin = 0, int tileSpacing = 0);
    bool loadMapFromCSV(const std::string& csvString, const std::string& tileGraphicPath,
                         int tileWidth = 0, int tileHeight = 0, int collideIndex = 1,
                         int tileMargin = 0, int tileSpacing = 0);

    void setTileProperties(int tileIndex, FlxDirectionFlags allowCollisions = FlxDirectionFlags::ANY);
    FlxDirectionFlags getTileCollisions(int tileIndex) const;

    inline int getMapIndex(int column, int row) const {
        if (column < 0 || column >= widthInTiles || row < 0 || row >= heightInTiles) {
            return -1;
        }
        return row * widthInTiles + column;
    }

    inline int getColumn(int mapIndex) const {
        return widthInTiles > 0 ? mapIndex % widthInTiles : -1;
    }

    inline int getRow(int mapIndex) const {
        return widthInTiles > 0 ? mapIndex / widthInTiles : -1;
    }

    FlxPoint getTilePos(int column, int row, bool midpoint = false) const;

    int getTileIndex(int column, int row) const;
    void setTileIndex(int column, int row, int tileIndex);

    int getTileIndexAt(float worldX, float worldY) const;
    bool isSolidAt(float worldX, float worldY) const;

    void draw() override;
    void destroy() override;

    int widthInTiles = 0;
    int heightInTiles = 0;
    int totalTiles = 0;
    int tileWidth = 0;
    int tileHeight = 0;

    SDL_Texture* texture = nullptr;
    bool ownsTexture = false;

private:
    std::vector<int> data;
    std::vector<SDL_Rect> tileFrames;
    std::vector<FlxDirectionFlags> tileCollisions;

    void loadMapHelper(const std::string& tileGraphicPath, int tileWidth, int tileHeight, int collideIndex,
                        int tileMargin, int tileSpacing);
};

}
}
