#include "FlxTilemap.h"
#include "FlxTileFormat.h"
#include "flixel/FlxG.h"
#include "flixel/FlxCamera.h"

namespace flixel {
namespace tile {

FlxTilemap::FlxTilemap()
    : FlxObject(0, 0, 0, 0)
{
    immovable = true;
    moves = false;
}

FlxTilemap::~FlxTilemap() {
    destroy();
}

void FlxTilemap::loadMapHelper(const std::string& tileGraphicPath, int requestedTileWidth, int requestedTileHeight,
                                int collideIndex, int tileMargin, int tileSpacing) {
    texture = FlxG::loadTextureCached(tileGraphicPath);
    ownsTexture = false;

    if (!texture) {
        FlxG::log.error("FlxTilemap: failed to load tileset " + tileGraphicPath);
        return;
    }

    int textureW = 0;
    int textureH = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &textureW, &textureH);

    tileWidth = requestedTileWidth > 0 ? requestedTileWidth : textureH;
    tileHeight = requestedTileHeight > 0 ? requestedTileHeight : textureW;

    tileFrames = sliceTileFrames(textureW, textureH, tileWidth, tileHeight, tileMargin, tileSpacing);

    // data value 0 is always "no tile"; data value N (N >= 1) maps to tileFrames[N - 1],
    // so every physical tile slice is reachable (no slice is wasted as a blank sentinel).
    tileCollisions.assign(tileFrames.size() + 1, FlxDirectionFlags::NONE);
    for (size_t i = collideIndex > 0 ? static_cast<size_t>(collideIndex) : 0; i < tileCollisions.size(); i++) {
        tileCollisions[i] = FlxDirectionFlags::ANY;
    }

    totalTiles = widthInTiles * heightInTiles;
    width = static_cast<float>(widthInTiles) * static_cast<float>(tileWidth);
    height = static_cast<float>(heightInTiles) * static_cast<float>(tileHeight);
}

bool FlxTilemap::loadMapFromArray(const std::vector<int>& mapData, int newWidthInTiles, int newHeightInTiles,
                                   const std::string& tileGraphicPath, int requestedTileWidth, int requestedTileHeight,
                                   int collideIndex, int tileMargin, int tileSpacing) {
    widthInTiles = newWidthInTiles;
    heightInTiles = newHeightInTiles;
    data = mapData;

    loadMapHelper(tileGraphicPath, requestedTileWidth, requestedTileHeight, collideIndex, tileMargin, tileSpacing);
    return texture != nullptr;
}

bool FlxTilemap::loadMapFromCSV(const std::string& csvString, const std::string& tileGraphicPath,
                                 int requestedTileWidth, int requestedTileHeight, int collideIndex,
                                 int tileMargin, int tileSpacing) {
    if (!parseTileCSV(csvString, data, widthInTiles, heightInTiles)) {
        FlxG::log.error("FlxTilemap: invalid CSV map data");
        return false;
    }

    loadMapHelper(tileGraphicPath, requestedTileWidth, requestedTileHeight, collideIndex, tileMargin, tileSpacing);
    return texture != nullptr;
}

void FlxTilemap::setTileProperties(int tileIndex, FlxDirectionFlags allowCollisions) {
    if (tileIndex < 0 || tileIndex >= static_cast<int>(tileCollisions.size())) {
        return;
    }
    tileCollisions[tileIndex] = allowCollisions;
}

FlxDirectionFlags FlxTilemap::getTileCollisions(int tileIndex) const {
    if (tileIndex < 0 || tileIndex >= static_cast<int>(tileCollisions.size())) {
        return FlxDirectionFlags::NONE;
    }
    return tileCollisions[tileIndex];
}

FlxPoint FlxTilemap::getTilePos(int column, int row, bool midpoint) const {
    float px = x + static_cast<float>(column) * static_cast<float>(tileWidth) + (midpoint ? static_cast<float>(tileWidth) * 0.5f : 0.0f);
    float py = y + static_cast<float>(row) * static_cast<float>(tileHeight) + (midpoint ? static_cast<float>(tileHeight) * 0.5f : 0.0f);
    return FlxPoint(px, py);
}

int FlxTilemap::getTileIndex(int column, int row) const {
    int idx = getMapIndex(column, row);
    if (idx < 0 || idx >= static_cast<int>(data.size())) {
        return -1;
    }
    return data[idx];
}

void FlxTilemap::setTileIndex(int column, int row, int tileIndex) {
    int idx = getMapIndex(column, row);
    if (idx < 0 || idx >= static_cast<int>(data.size())) {
        return;
    }
    data[idx] = tileIndex;
}

int FlxTilemap::getTileIndexAt(float worldX, float worldY) const {
    if (tileWidth <= 0 || tileHeight <= 0) {
        return -1;
    }
    int column = static_cast<int>((worldX - x) / static_cast<float>(tileWidth));
    int row = static_cast<int>((worldY - y) / static_cast<float>(tileHeight));
    return getTileIndex(column, row);
}

bool FlxTilemap::isSolidAt(float worldX, float worldY) const {
    int type = getTileIndexAt(worldX, worldY);
    if (type <= 0) {
        return false;
    }
    return getTileCollisions(type) != FlxDirectionFlags::NONE;
}

void FlxTilemap::draw() {
    if (!texture || !visible || tileWidth <= 0 || tileHeight <= 0) {
        return;
    }

    FlxCamera* cam = camera ? camera : FlxG::camera;

    float camScrollX = 0.0f;
    float camScrollY = 0.0f;
    float camZoom = 1.0f;
    float viewW = static_cast<float>(widthInTiles * tileWidth);
    float viewH = static_cast<float>(heightInTiles * tileHeight);

    if (cam) {
        camScrollX = cam->scroll.x * scrollFactor.x;
        camScrollY = cam->scroll.y * scrollFactor.y;
        camZoom = cam->zoom > 0.0f ? cam->zoom : 1.0f;
        viewW = static_cast<float>(cam->width) / camZoom;
        viewH = static_cast<float>(cam->height) / camZoom;
    }

    int firstCol = static_cast<int>((camScrollX - x) / static_cast<float>(tileWidth)) - 1;
    int firstRow = static_cast<int>((camScrollY - y) / static_cast<float>(tileHeight)) - 1;
    int lastCol = static_cast<int>((camScrollX + viewW - x) / static_cast<float>(tileWidth)) + 1;
    int lastRow = static_cast<int>((camScrollY + viewH - y) / static_cast<float>(tileHeight)) + 1;

    if (firstCol < 0) firstCol = 0;
    if (firstRow < 0) firstRow = 0;
    if (lastCol > widthInTiles - 1) lastCol = widthInTiles - 1;
    if (lastRow > heightInTiles - 1) lastRow = heightInTiles - 1;

    for (int row = firstRow; row <= lastRow; row++) {
        for (int col = firstCol; col <= lastCol; col++) {
            int type = data[row * widthInTiles + col];
            if (type <= 0 || type > static_cast<int>(tileFrames.size())) {
                continue;
            }

            const SDL_Rect& src = tileFrames[type - 1];
            float finalX = (x + static_cast<float>(col * tileWidth) - camScrollX) * camZoom;
            float finalY = (y + static_cast<float>(row * tileHeight) - camScrollY) * camZoom;
            SDL_FRect destRectF = {
                finalX, finalY,
                static_cast<float>(tileWidth) * camZoom,
                static_cast<float>(tileHeight) * camZoom
            };

            SDL_RenderCopyF(FlxG::renderer, texture, &src, &destRectF);
        }
    }
}

void FlxTilemap::destroy() {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
    }
    texture = nullptr;
    ownsTexture = false;
    FlxObject::destroy();
}

}
}
