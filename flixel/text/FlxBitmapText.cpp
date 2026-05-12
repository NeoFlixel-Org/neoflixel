#include "FlxBitmapText.h"
#include "../FlxG.h"
#include <iostream>

namespace flixel {

FlxBitmapFont* FlxBitmapFont::fromMonospace(const std::string& imagePath, const std::string& letters, int charWidth, int charHeight) {
    FlxBitmapFont* font = new FlxBitmapFont();
    
    font->texture = FlxG::loadTexture(imagePath);
    if (!font->texture) {
        std::cout << "Failed to load bitmap font texture: " << imagePath << std::endl;
        delete font;
        return nullptr;
    }
    
    font->lineHeight = charHeight;
    font->spaceWidth = charWidth;
    
    int gridX = 0;
    int gridY = 0;
    int charsPerRow = 16;
    
    int textureWidth, textureHeight;
    SDL_QueryTexture(font->texture, nullptr, nullptr, &textureWidth, &textureHeight);
    
    if (textureWidth > 0 && charWidth > 0) {
        charsPerRow = textureWidth / charWidth;
    }
    
    for (size_t i = 0; i < letters.length(); i++) {
        char c = letters[i];
        
        BitmapCharacter bChar;
        bChar.x = gridX * charWidth;
        bChar.y = gridY * charHeight;
        bChar.width = charWidth;
        bChar.height = charHeight;
        bChar.xAdvance = charWidth;
        
        font->characters[c] = bChar;
                
        gridX++;
        if (gridX >= charsPerRow) {
            gridX = 0;
            gridY++;
        }
    }    
    return font;
}

FlxBitmapText::FlxBitmapText(FlxBitmapFont* font)
    : FlxSprite()
    , font(font)
    , text("")
    , letterSpacing(0)
    , needsUpdate(true)
{
}

FlxBitmapText::~FlxBitmapText() {
}

void FlxBitmapText::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        needsUpdate = true;
        width = static_cast<float>(calculateTextWidth());
        height = font ? static_cast<float>(font->lineHeight) : 0.0f;
    }
}

int FlxBitmapText::calculateTextWidth() {
    if (!font || text.empty()) return 0;
    
    int totalWidth = 0;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        
        if (c == ' ') {
            totalWidth += font->spaceWidth + letterSpacing;
        } else {
            auto it = font->characters.find(c);
            if (it != font->characters.end()) {
                totalWidth += it->second.xAdvance + letterSpacing;
            }
        }
    }
    
    if (totalWidth > 0 && !text.empty()) {
        totalWidth -= letterSpacing;
    }
    
    return totalWidth;
}

void FlxBitmapText::draw() {    
    if (!visible || !font || !font->texture || text.empty()) {
        return;
    }
    
    FlxCamera* cam = camera ? camera : FlxG::camera;
    if (!cam) {
        std::cout << "  No camera!" << std::endl;
        return;
    }
    
    float renderX = x;
    float renderY = y;
    
    if (scrollFactor.x != 0.0f || scrollFactor.y != 0.0f) {
        renderX -= cam->scroll.x * scrollFactor.x;
        renderY -= cam->scroll.y * scrollFactor.y;
    }
        
    float currentX = renderX;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        
        if (c == ' ') {
            currentX += (font->spaceWidth + letterSpacing) * scale.x;
            continue;
        }
        
        auto it = font->characters.find(c);
        if (it == font->characters.end()) {
            std::cout << "Character '" << c << "' not found in font!" << std::endl;
            continue;
        }
        
        const BitmapCharacter& bChar = it->second;
        
        SDL_Rect srcRect;
        srcRect.x = bChar.x;
        srcRect.y = bChar.y;
        srcRect.w = bChar.width;
        srcRect.h = bChar.height;
        
        SDL_FRect destRect;
        destRect.x = currentX;
        destRect.y = renderY;
        destRect.w = static_cast<float>(bChar.width) * scale.x;
        destRect.h = static_cast<float>(bChar.height) * scale.y;
        
        SDL_SetTextureAlphaMod(font->texture, static_cast<Uint8>(alpha * 255.0f));
        SDL_SetTextureColorMod(font->texture, 
                              static_cast<Uint8>((color >> 16) & 0xFF),
                              static_cast<Uint8>((color >> 8) & 0xFF),
                              static_cast<Uint8>(color & 0xFF));
        
        SDL_RenderCopyExF(FlxG::renderer, font->texture, &srcRect, &destRect, 
                         angle, nullptr, SDL_FLIP_NONE);
        
        currentX += (bChar.xAdvance + letterSpacing) * scale.x;
    }
}

void FlxBitmapText::screenCenter() {
    x = (FlxG::width - width) / 2.0f;
}
}