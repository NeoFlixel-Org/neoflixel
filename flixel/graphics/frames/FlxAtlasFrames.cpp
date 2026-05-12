#include "FlxAtlasFrames.h"
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace tinyxml2;

namespace flixel {
namespace graphics {
namespace frames {

FlxAtlasFrames* FlxAtlasFrames::fromSparrow(const std::string& imagePath, const std::string& xmlText) {
    FlxAtlasFrames* atlas = new FlxAtlasFrames();

    XMLDocument doc;
    doc.Parse(xmlText.c_str());
    XMLElement* root = doc.FirstChildElement("TextureAtlas");
    if (!root) return nullptr;

    int idx = 0;
    for (XMLElement* sub = root->FirstChildElement("SubTexture"); sub; sub = sub->NextSiblingElement("SubTexture")) {
        AtlasFrame frame;
        frame.name = sub->Attribute("name");
        frame.rect.x = sub->IntAttribute("x");
        frame.rect.y = sub->IntAttribute("y");
        frame.rect.w = sub->IntAttribute("width");
        frame.rect.h = sub->IntAttribute("height");
        frame.index = idx++;
        atlas->frames.push_back(frame);

        std::string prefix = frame.name.substr(0, frame.name.find_first_of("0123456789"));
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