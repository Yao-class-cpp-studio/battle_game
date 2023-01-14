#pragma once
#include "battle_game/graphics/assets_manager.h"
#include "battle_game/graphics/texture.h"
#include "battle_game/graphics/util.h"

namespace battle_game {

void NewFrame();

void SetColor(const glm::vec4 &color = glm::vec4{1.0f});
glm::vec4 GetColor();

void SetPosition(glm::vec2 position);
void SetRotation(float rotation);
glm::vec2 GetPosition();
float GetRotation();

void SetScale(glm::vec2 scale);
glm::vec2 GetScale();

void SetTransformation(glm::vec2 position,
                       float rotation = 0.0f,
                       glm::vec2 scale = glm::vec2{1.0f});

uint32_t RegisterTexture(const std::string &file_path);
uint32_t SetTexture(const std::string &file_path);
void SetTexture(uint32_t texture_id = 0);
uint32_t GetTexture();

void DrawModel(uint32_t model_id = 0);
void DrawTexture(const std::string &file_path);

const std::vector<ObjectSettings> &GetObjectSettings();
const std::vector<TextureInfo> &GetTextureInfos();
const std::vector<uint32_t> &GetModelIds();
}  // namespace battle_game
