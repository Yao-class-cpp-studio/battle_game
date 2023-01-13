#pragma once
#include "battle_game/graphics/graphics.h"
#include "glm/glm.hpp"
#define SKILL_ADD_FUNCTION(function_) std::bind(&function_, this)
#define SKILL_SWITCH_BULLET(function_) \
  std::bind(&function_, this, std::placeholders::_1)
#define SKILL_ADD_NULL nullptr

namespace battle_game {
class GameCore;
enum SkillType { E, Q, R, P, B };
struct Skill {
  std::string name;
  std::string description;
  std::string src;
  uint32_t time_remain;
  uint32_t time_total;
  uint32_t bullet_type;
  uint32_t bullet_total_number;
  SkillType type;
  std::function<void(void)> function;
  std::function<void(uint32_t)> switch_bullet;
  Skill(){};
  Skill(std::string name, SkillType type) : name(name), type(type){};
  Skill(std::string name,
        SkillType type,
        uint32_t time_remain,
        uint32_t time_total)
      : name(name),
        type(type),
        time_remain(time_remain),
        time_total(time_total){};
  Skill(std::string name,
        SkillType type,
        uint32_t time_remain,
        uint32_t time_total,
        uint32_t bullet_type,
        uint32_t bullet_total_number)
      : name(name),
        type(type),
        time_remain(time_remain),
        time_total(time_total),
        bullet_type(bullet_type),
        bullet_total_number(bullet_total_number){};
  Skill(std::string name,
        SkillType type,
        uint32_t time_remain,
        uint32_t time_total,
        uint32_t bullet_type,
        uint32_t bullet_total_number,
        std::string description,
        std::string src,
        std::function<void(void)> function)
      : name(name),
        type(type),
        time_remain(time_remain),
        time_total(time_total),
        bullet_type(bullet_type),
        bullet_total_number(bullet_total_number),
        description(description),
        src(src),
        function(function){};
};

class Object {
 public:
  Object(GameCore *game_core,
         uint32_t id,
         glm::vec2 position = glm::vec2{0.0f},
         float rotation = 0.0f);
  virtual ~Object();
  [[nodiscard]] glm::vec2 LocalToWorld(glm::vec2 p) const;
  [[nodiscard]] glm::vec2 WorldToLocal(glm::vec2 p) const;
  [[nodiscard]] glm::vec2 GetPosition() const {
    return position_;
  }
  [[nodiscard]] float GetRotation() const {
    return rotation_;
  }
  [[nodiscard]] GameCore *GetGameCore() const {
    return game_core_;
  }
  [[nodiscard]] uint32_t GetId() const {
    return id_;
  }

  virtual void Render() = 0;
  virtual void Update() = 0;

 protected:
  GameCore *game_core_{nullptr};
  glm::vec2 position_{0.0f};  // offset from the origin (0, 0)
  float rotation_{0.0f};      // angle in radians
  uint32_t id_{0};
};
}  // namespace battle_game
