#include "battle_game/core/object.h"

#include "glm/gtc/matrix_transform.hpp"

namespace battle_game {
glm::vec2 Object::LocalToWorld(const glm::vec2 p) const {
  return glm::vec2{
      glm::translate(glm::mat4{1.0f}, glm::vec3{position_, 0.0f}) *
      glm::rotate(glm::mat4{1.0f}, rotation_, glm::vec3{0.0f, 0.0f, 1.0f}) *
      glm::vec4{p, 0.0f, 1.0f}};
}

glm::vec2 Object::WorldToLocal(const glm::vec2 p) const {
  return glm::vec2{
      glm::inverse(glm::translate(glm::mat4{1.0f}, glm::vec3{position_, 0.0f}) *
                   glm::rotate(glm::mat4{1.0f}, rotation_,
                               glm::vec3{0.0f, 0.0f, 1.0f})) *
      glm::vec4{p, 0.0f, 1.0f}};
}

Object::Object(GameCore *game_core,
               uint32_t id,
               glm::vec2 position,
               float rotation) {
  game_core_ = game_core;
  position_ = position;
  rotation_ = rotation;
  id_ = id;
}

Object::~Object() = default;
}  // namespace battle_game
