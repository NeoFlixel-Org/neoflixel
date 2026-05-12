#pragma once

#include "../FlxSprite.h"
#include <string>
#include <map>

namespace flixel {

struct BitmapCharacter {
    int x, y, width, height;
    int xAdvance;
};

class FlxBitmapFont {
public:
    std::map<char, BitmapCharacter> characters;
    int spaceWidth;
    int lineHeight;
    SDL_Texture* texture;
    
    FlxBitmapFont() : spaceWidth(0), lineHeight(0), texture(nullptr) {}
    
    static FlxBitmapFont* fromMonospace(const std::string& imagePath, const std::string& letters, int charWidth, int charHeight);
};

class FlxBitmapText : public FlxSprite {
public:
    FlxBitmapText(FlxBitmapFont* font);
    virtual ~FlxBitmapText();
    
    void setText(const std::string& text);
    std::string getText() const { return text; }
    
    void draw() override;
    void screenCenter();
    
    int letterSpacing;
    
private:
    void updateGraphic();
    int calculateTextWidth();
    
    FlxBitmapFont* font;
    std::string text;
    bool needsUpdate;
};

}