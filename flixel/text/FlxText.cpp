#include "FlxText.h"
#include "../FlxG.h"
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cerrno>

namespace flixel {

FlxText::FlxText(float x, float y, float fieldWidth, const std::string& text, int size)
    : FlxSprite(x, y)
    , text(text.empty() ? " " : text)
    , fontSize(size)
    , bold(false)
    , italic(false)
    , underline(false)
    , wordWrap(true)
    , embeddedFont(false)
    , fieldWidth(fieldWidth)
    , alignment(FlxTextAlign::LEFT)
    , borderStyle(FlxTextBorderStyle::NONE)
    , borderColor({0, 0, 0, 255})
    , borderSize(1.0f)
    , borderQuality(1.0f)
    , shadowOffsetX(1.0f)
    , shadowOffsetY(1.0f)
    , font(nullptr)
    , textTexture(nullptr)
    , textSurface(nullptr)
    , needsUpdate(true)
{
    
    if (!TTF_WasInit()) {
        if (TTF_Init() < 0) {
            std::cout << "TTF_Init failed: " << TTF_GetError() << std::endl;
            return;
        }
    }
    
    setSystemFont("arial");
    
    setFieldWidth(fieldWidth);
    updateHitbox();
}

FlxText::~FlxText() {
    destroy();
}

void FlxText::destroy() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
        textTexture = nullptr;
    }
    
    if (textSurface) {
        SDL_FreeSurface(textSurface);
        textSurface = nullptr;
    }
    
    FlxSprite::destroy();
}

void FlxText::setText(const std::string& newText) {
    if (text != newText) {
        text = newText.empty() ? " " : newText;
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setSize(int size) {
    if (fontSize != size) {
        fontSize = size;
        if (font) {
            TTF_CloseFont(font);
            font = TTF_OpenFont(fontPath.c_str(), fontSize);
            TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                                  (italic ? TTF_STYLE_ITALIC : 0) | 
                                  (underline ? TTF_STYLE_UNDERLINE : 0));
        }
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setFont(const std::string& path) {    
    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cout << "Font file does not exist or cannot be opened: " << path << std::endl;
        return;
    }
    fclose(file);
    
    if (fontPath != path) {
        fontPath = path;
        embeddedFont = true;
        
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        
        font = TTF_OpenFont(path.c_str(), fontSize);
        if (!font) {
            std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
            return;
        }
        
        TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                              (italic ? TTF_STYLE_ITALIC : 0) | 
                              (underline ? TTF_STYLE_UNDERLINE : 0));
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setSystemFont(const std::string& fontName) {
    std::string systemFontPath = "C:/Windows/Fonts/" + fontName + ".ttf";
    if (fontPath != systemFontPath) {
        fontPath = systemFontPath;
        embeddedFont = false;
        
        if (font) {
            TTF_CloseFont(font);
        }
        
        font = TTF_OpenFont(systemFontPath.c_str(), fontSize);
        if (!font) {
            font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", fontSize);
        }
        
        if (font) {
            TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                                  (italic ? TTF_STYLE_ITALIC : 0) | 
                                  (underline ? TTF_STYLE_UNDERLINE : 0));
            needsUpdate = true;
            updateHitbox();
        }
    }
}

void FlxText::setAlignment(FlxTextAlign align) {
    if (alignment != align) {
        alignment = align;
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setBorderStyle(FlxTextBorderStyle style, SDL_Color color, float size, float quality) {
    borderStyle = style;
    borderColor = color;
    borderSize = size;
    borderQuality = std::max(0.0f, std::min(1.0f, quality));
    needsUpdate = true;
    updateHitbox();
}

void FlxText::setShadowOffset(float x, float y) {
    shadowOffsetX = x;
    shadowOffsetY = y;
    needsUpdate = true;
    updateHitbox();
}

void FlxText::setBold(bool value) {
    if (bold != value && font) {
        bold = value;
        TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                              (italic ? TTF_STYLE_ITALIC : 0) | 
                              (underline ? TTF_STYLE_UNDERLINE : 0));
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setItalic(bool value) {
    if (italic != value && font) {
        italic = value;
        TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                              (italic ? TTF_STYLE_ITALIC : 0) | 
                              (underline ? TTF_STYLE_UNDERLINE : 0));
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setUnderline(bool value) {
    if (underline != value && font) {
        underline = value;
        TTF_SetFontStyle(font, (bold ? TTF_STYLE_BOLD : 0) | 
                              (italic ? TTF_STYLE_ITALIC : 0) | 
                              (underline ? TTF_STYLE_UNDERLINE : 0));
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setFieldWidth(float width) {
    if (fieldWidth != width) {
        fieldWidth = width;
        wordWrap = (width > 0);
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setWordWrap(bool wrap) {
    if (wordWrap != wrap) {
        wordWrap = wrap && (fieldWidth > 0);
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::setColor(Uint32 newColor) {
    if (color != newColor) {
        color = newColor;
        needsUpdate = true;
        updateHitbox();
    }
}

void FlxText::draw() {
    if (needsUpdate) {
        regenGraphic();
    }
    
    if (!texture) {
        std::cout << "No texture to draw!" << std::endl;
        return;
    }
    
    FlxSprite::draw();
}

void FlxText::updateHitbox() {
    if (needsUpdate) {
        regenGraphic();
    }
    
    FlxSprite::updateHitbox();
}

void FlxText::regenGraphic() {
    if (!font) {
        std::cout << "No font loaded!" << std::endl;
        return;
    }
    
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
        textTexture = nullptr;
    }
    
    if (textSurface) {
        SDL_FreeSurface(textSurface);
        textSurface = nullptr;
    }
    
    SDL_Color textColor = {
        static_cast<Uint8>((color >> 16) & 0xFF),
        static_cast<Uint8>((color >> 8) & 0xFF),
        static_cast<Uint8>(color & 0xFF),
        static_cast<Uint8>((color >> 24) & 0xFF)
    };
    
    if (wordWrap && fieldWidth > 0) {
        textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, static_cast<Uint32>(fieldWidth));
    } else {
        textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    }
    
    if (!textSurface) {
        std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
        return;
    }
    
    if (borderStyle != FlxTextBorderStyle::NONE) {
        applyBorderStyle();
    }
    
    textTexture = SDL_CreateTextureFromSurface(FlxG::renderer, textSurface);
    if (!textTexture) {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
        return;
    }
    
    width = static_cast<float>(textSurface->w);
    height = static_cast<float>(textSurface->h);
    frameWidth = textSurface->w;
    frameHeight = textSurface->h;
    
    sourceRect = {0, 0, frameWidth, frameHeight};
    destRect = {static_cast<int>(x), static_cast<int>(y), frameWidth, frameHeight};
    
    if (alignment == FlxTextAlign::CENTER) {
        centerText();
    }
    
    texture = textTexture;
    
    needsUpdate = false;
}

void FlxText::applyBorderStyle() {
    if (!textSurface || borderStyle == FlxTextBorderStyle::NONE) {
        return;
    }
    
    SDL_Surface* borderSurface = nullptr;
    SDL_Color borderColor = this->borderColor;
    
    switch (borderStyle) {
        case FlxTextBorderStyle::SHADOW:
        case FlxTextBorderStyle::SHADOW_XY: {
            borderSurface = SDL_CreateRGBSurface(0,
                textSurface->w + static_cast<int>(std::abs(shadowOffsetX) * borderSize),
                textSurface->h + static_cast<int>(std::abs(shadowOffsetY) * borderSize),
                32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
                
            if (borderSurface) {
                SDL_Rect shadowRect = {
                    static_cast<int>(shadowOffsetX > 0 ? shadowOffsetX * borderSize : 0),
                    static_cast<int>(shadowOffsetY > 0 ? shadowOffsetY * borderSize : 0),
                    textSurface->w,
                    textSurface->h
                };
                SDL_BlitSurface(textSurface, nullptr, borderSurface, &shadowRect);
                
                SDL_LockSurface(borderSurface);
                Uint32* pixels = static_cast<Uint32*>(borderSurface->pixels);
                for (int i = 0; i < borderSurface->w * borderSurface->h; i++) {
                    Uint8 alpha = (pixels[i] >> 24) & 0xFF;
                    if (alpha > 0) {
                        pixels[i] = (alpha << 24) | (borderColor.r << 16) | (borderColor.g << 8) | borderColor.b;
                    }
                }
                SDL_UnlockSurface(borderSurface);
                
                SDL_Rect textRect = {
                    static_cast<int>(shadowOffsetX < 0 ? -shadowOffsetX * borderSize : 0),
                    static_cast<int>(shadowOffsetY < 0 ? -shadowOffsetY * borderSize : 0),
                    textSurface->w,
                    textSurface->h
                };
                SDL_BlitSurface(textSurface, nullptr, borderSurface, &textRect);
            }
            break;
        }
        
        case FlxTextBorderStyle::OUTLINE:
        case FlxTextBorderStyle::OUTLINE_FAST: {
            borderSurface = SDL_CreateRGBSurface(0,
                textSurface->w + static_cast<int>(borderSize * 2),
                textSurface->h + static_cast<int>(borderSize * 2),
                32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
                
            if (borderSurface) {
                for (int x = -borderSize; x <= borderSize; x++) {
                    for (int y = -borderSize; y <= borderSize; y++) {
                        if (borderStyle == FlxTextBorderStyle::OUTLINE_FAST && 
                            x != -borderSize && x != borderSize && 
                            y != -borderSize && y != borderSize) {
                            continue;
                        }
                        
                        SDL_Rect outlineRect = {
                            static_cast<int>(borderSize + x),
                            static_cast<int>(borderSize + y),
                            textSurface->w,
                            textSurface->h
                        };
                        SDL_BlitSurface(textSurface, nullptr, borderSurface, &outlineRect);
                    }
                }
                
                SDL_LockSurface(borderSurface);
                Uint32* pixels = static_cast<Uint32*>(borderSurface->pixels);
                for (int i = 0; i < borderSurface->w * borderSurface->h; i++) {
                    Uint8 alpha = (pixels[i] >> 24) & 0xFF;
                    if (alpha > 0) {
                        pixels[i] = (alpha << 24) | (borderColor.r << 16) | (borderColor.g << 8) | borderColor.b;
                    }
                }
                SDL_UnlockSurface(borderSurface);
                
                SDL_Rect textRect = {
                    static_cast<int>(borderSize),
                    static_cast<int>(borderSize),
                    textSurface->w,
                    textSurface->h
                };
                SDL_BlitSurface(textSurface, nullptr, borderSurface, &textRect);
            }
            break;
        }
        
        default:
            break;
    }
    
    if (borderSurface) {
        SDL_FreeSurface(textSurface);
        textSurface = borderSurface;
    }
}

void FlxText::centerText() {
    if (!textSurface) {
        return;
    }
    
    switch (alignment) {
        case FlxTextAlign::CENTER:
            offsetX = -0.5f * (width - static_cast<float>(textSurface->w));
            offsetY = -0.5f * (height - static_cast<float>(textSurface->h));
            break;
            
        case FlxTextAlign::RIGHT:
            offsetX = -(width - static_cast<float>(textSurface->w));
            offsetY = 0;
            break;
            
        default:
            offsetX = 0;
            offsetY = 0;
            break;
    }
}

void FlxText::screenCenter() {
    float widthToUse = fieldWidth > 0 ? fieldWidth : width;
    x = (FlxG::width - widthToUse) / 2;
    y = (FlxG::height - height) / 2;
}

} 