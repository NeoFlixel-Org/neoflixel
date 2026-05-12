#pragma once

#include "../FlxSprite.h"
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

namespace flixel {

enum class FlxTextAlign {
    LEFT,
    CENTER,
    RIGHT,
    JUSTIFY
};

enum class FlxTextBorderStyle {
    NONE,
    SHADOW,
    SHADOW_XY,
    OUTLINE,
    OUTLINE_FAST
};

class FlxText : public FlxSprite {
public:
    FlxText(float x = 0, float y = 0, float fieldWidth = 0, const std::string& text = "", int size = 8);
    virtual ~FlxText();

    void setText(const std::string& text);
    std::string getText() const { return text; }
    
    void setSize(int size);
    int getSize() const { return fontSize; }
    
    void setFont(const std::string& fontPath);
    void setSystemFont(const std::string& fontName);
    
    void setAlignment(FlxTextAlign align);
    FlxTextAlign getAlignment() const { return alignment; }
    
    void setBorderStyle(FlxTextBorderStyle style, SDL_Color color = {0, 0, 0, 255}, float size = 1.0f, float quality = 1.0f);
    
    void setShadowOffset(float x, float y);
    
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    
    void setFieldWidth(float width);
    float getFieldWidth() const { return fieldWidth; }
    
    void setWordWrap(bool wrap);
    bool getWordWrap() const { return wordWrap; }

    void setColor(Uint32 color);
    Uint32 getColor() const { return color; }
    
    void draw() override;
    void destroy() override;
    void updateHitbox() override;

    void screenCenter();

private:
    void regenGraphic();
    void applyBorderStyle();
    void drawTextField();
    void updateTextFormat();
    void centerText();
    
    std::string text;
    std::string fontPath;
    int fontSize;
    bool bold;
    bool italic;
    bool underline;
    bool wordWrap;
    bool embeddedFont;
    
    float fieldWidth;
    FlxTextAlign alignment;
    
    FlxTextBorderStyle borderStyle;
    SDL_Color borderColor;
    float borderSize;
    float borderQuality;
    float shadowOffsetX;
    float shadowOffsetY;
    
    TTF_Font* font;
    SDL_Texture* textTexture;
    SDL_Surface* textSurface;
    
    bool needsUpdate;
};

} 