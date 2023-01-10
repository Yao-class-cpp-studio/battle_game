#pragma once
#include "battle_game/core/object.h"
#include "glm/glm.hpp"

namespace battle_game {
enum ObstacleType // use lowercase to avoid class name
{
  untracked_obstacle,
  block
  // add your obstacle here:

};

class Obstacle : public Object {
 public:
  Obstacle(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation = 0.0f);
  [[nodiscard]] virtual bool IsBlocked(glm::vec2 p) const = 0;
  void Update() override;
  void Render() override;
  virtual std::pair<glm::vec2, glm::vec2> GetSurfaceNormal(glm::vec2 origin,
                                                           glm::vec2 terminus) {
    return std::make_pair(glm::vec2(0, 0), glm::vec2(0, 0));
  }
  [[nodiscard]] ObstacleType GetObstacleType() const {
    return obstacle_type_;
  }

 protected:
  ObstacleType obstacle_type_{untracked_obstacle};
};
}  // namespace battle_game
