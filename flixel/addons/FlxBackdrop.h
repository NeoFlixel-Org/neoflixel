#pragma once

#include "../FlxSprite.h"
#include <vector>

namespace flixel {
namespace addons {

class FlxBackdrop : public FlxSprite {
public:
    bool repeatX;
    bool repeatY;
    int spaceX;
    int spaceY;
    bool useScaleHack;
    int numTiles;
    
    FlxBackdrop(const std::string& graphic = "", float scrollX = 1.0f, float scrollY = 1.0f, 
                bool repeatX = true, bool repeatY = true, int spaceX = 0, int spaceY = 0);
    FlxBackdrop(SDL_Texture* tex, float scrollX = 1.0f, float scrollY = 1.0f, 
                bool repeatX = true, bool repeatY = true, int spaceX = 0, int spaceY = 0);
    virtual ~FlxBackdrop();
    
    bool loadGraphic(const std::string& path);
    void setTexture(SDL_Texture* tex);
    
    virtual void draw() override;
    virtual void update(float elapsed) override;
    
private:
    int scrollW;
    int scrollH;
    std::vector<float> tileInfo;
    float ppointX;
    float ppointY;
    
    void regenTileInfo();
};
}
}