#include "battle_game/core/obstacles/safety_declaration.h"

#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

namespace {
uint32_t declaration_model_index = 0xffffffffu;
}  // namespace

SafetyDeclaration::SafetyDeclaration(GameCore *game_core,
                                     uint32_t id,
                                     glm::vec2 position,
                                     float rotation,
                                     glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
  if (!~declaration_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* ThreeBodyMan Body */
      declaration_model_index = mgr->RegisterModel(
          {{{0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 4, 5, 0, 1, 5, 1, 2, 5, 2, 5, 6,
           2, 3, 6, 3, 6, 7, 3, 0, 4, 3, 4, 7});
    }
  }
  valid_time_ = 3 * kTickPerSecond;
}

bool SafetyDeclaration::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void SafetyDeclaration::HandleCollision(glm::vec2 &p) const {
  auto local_p = WorldToLocal(p);

  auto del1 = scale_.x - local_p.x;
  auto del2 = local_p.x + scale_.x;
  auto del3 = scale_.y - local_p.y;
  auto del4 = local_p.y + scale_.y;
  if (del1 <= del2 && del1 <= del3 && del1 <= del4) {
    local_p.x = scale_.x;
  } else if (del2 <= del1 && del2 <= del3 && del2 <= del4) {
    local_p.x = -scale_.x;
  } else if (del3 <= del1 && del3 <= del2 && del3 <= del4) {
    local_p.y = scale_.y;
  } else {
    local_p.y = -scale_.y;
  }

  p = LocalToWorld(local_p);
}

void SafetyDeclaration::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(declaration_model_index);
}

void SafetyDeclaration::Update() {
  if (valid_time_) {
    valid_time_--;
  } else {
    game_core_->PushEventRemoveObstacle(id_);
  }
}
}  // namespace battle_game::obstacle
