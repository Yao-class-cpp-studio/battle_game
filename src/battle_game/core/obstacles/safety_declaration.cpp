#include "battle_game/core/game_core.h"
#include "battle_game/core/obstacles/safety_declaration.h"

namespace battle_game::obstacle {

  namespace {
    uint32_t declaration_model_index = 0xffffffffu;
  }  // namespace

  SafetyDeclaration::SafetyDeclaration(GameCore* game_core,
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
          { {{0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}} },
          { 0, 4, 5, 0, 1, 5, 1, 2, 5, 2, 5, 6, 2, 3, 6, 3, 6, 7,
            3, 0, 4, 3, 4, 7 });
      }
    }
    valid_time_ = 3 * kTickPerSecond;
  }

  bool SafetyDeclaration::IsBlocked(glm::vec2 p) const {
    p = WorldToLocal(p);
    return p.x <= scale_.x && p.x >= -scale_.x &&
      p.y <= scale_.y && p.y >= -scale_.y;
  }

  void SafetyDeclaration::Render() {
    battle_game::SetColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
    battle_game::SetTexture(0);
    battle_game::SetTransformation(position_, rotation_, scale_);
    battle_game::DrawModel(declaration_model_index);
  }

  void SafetyDeclaration::Update() {
    if (valid_time_) {
      valid_time_--;
    }
    else {
      game_core_->PushEventRemoveObstacle(id_);
    }
  }
}  // namespace battle_game::obstacle
