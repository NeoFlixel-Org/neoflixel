#include "FlxAtlasFrames.h"
#include "../../FlxG.h"
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace tinyxml2;

namespace flixel {
namespace graphics {
namespace frames {

FlxAtlasFrames* FlxAtlasFrames::fromSparrow(const std::string& imagePath, const std::string& xmlText) {
    FlxAtlasFrames* atlas = new FlxAtlasFrames();
    
    atlas->texture = FlxG::loadTextureCached(imagePath);
    if (!atlas->texture) {
        fprintf(stderr, "[ATLAS] Texture load failed for %s, skipping\n", imagePath.c_str()); fflush(stderr);
        delete atlas;
        return nullptr;
    }

    XMLDocument doc;
    doc.Parse(xmlText.c_str());
    XMLElement* root = doc.FirstChildElement("TextureAtlas");
    if (!root) {
        delete atlas;
        return nullptr;
    }

    int idx = 0;
    for (XMLElement* sub = root->FirstChildElement("SubTexture"); sub; sub = sub->NextSiblingElement("SubTexture")) {
        AtlasFrame frame;
        frame.name = sub->Attribute("name");
        frame.rect.x = sub->IntAttribute("x");
        frame.rect.y = sub->IntAttribute("y");
        frame.rect.w = sub->IntAttribute("width");
        frame.rect.h = sub->IntAttribute("height");
        int frameWidth = sub->IntAttribute("frameWidth", frame.rect.w);
        int frameHeight = sub->IntAttribute("frameHeight", frame.rect.h);
        int frameX = sub->IntAttribute("frameX", 0);
        int frameY = sub->IntAttribute("frameY", 0);
        
        frame.sourceSize.x = -frameX;
        frame.sourceSize.y = -frameY;
        frame.sourceSize.w = frameWidth;
        frame.sourceSize.h = frameHeight;
        
        frame.index = idx++;
        atlas->frames.push_back(frame);

        size_t lastNonDigit = frame.name.find_last_not_of("0123456789");
        std::string prefix = (lastNonDigit != std::string::npos) 
            ? frame.name.substr(0, lastNonDigit + 1) 
            : frame.name;
        atlas->prefixToIndices[prefix].push_back(frame.index);
    }
    return atlas;
}

int FlxAtlasFrames::getFrameIndexByName(const std::string& name) const {
    for (const auto& frame : frames) {
        if (frame.name == name) return frame.index;
    }
    return -1;
}

std::vector<int> FlxAtlasFrames::getFramesByPrefix(const std::string& prefix) const {
    auto it = prefixToIndices.find(prefix);
    if (it != prefixToIndices.end()) return it->second;
    return {};
}

}
}
}