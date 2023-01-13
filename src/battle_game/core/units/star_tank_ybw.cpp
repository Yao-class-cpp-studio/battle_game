#include "star_tank_ybw.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

    namespace {
        uint32_t tank_body_model_index = 0xffffffffu;
        uint32_t tank_turret_model_index = 0xffffffffu;
    }  // namespace

    StarTank::StarTank(GameCore* game_core, uint32_t id, uint32_t player_id)
        : Unit(game_core, id, player_id) {
        if (!~tank_body_model_index) {
            auto mgr = AssetsManager::GetInstance();
            {
                /* Tank Body */
                tank_body_model_index = mgr->RegisterModel(
                    {
                        {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        // distinguish front and back
                        {{0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{-0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                    },
                    { 0, 1, 3, 0, 2, 3, 0, 2, 5, 2, 4, 5});
            }

            {
                /* Tank Turret */
                tank_turret_model_index =
                    mgr->RegisterModel(
                    {
                        {{0.0f, 1.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{0.96f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{0.0f, -1.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        {{-0.96f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
                        //inner vertices
                        {{0.3f, 0.4f}, {0.0f, 0.0f}, {0.9f, 0.9f, 0.0f, 1.0f}},
                        {{0.3f, -0.4f}, {0.0f, 0.0f}, {0.9f, 0.9f, 0.0f, 1.0f}},
                        {{-0.3f, -0.4f}, {0.0f, 0.0f}, {0.9f, 0.9f, 0.0f, 1.0f}},
                        {{-0.3f, 0.4f}, {0.0f, 0.0f}, {0.9f, 0.9f, 0.0f, 1.0f}},
                    },
                    {0, 4, 7, 1, 4, 5, 2, 5, 6, 3, 6, 7, 4, 5, 6, 4, 6, 7});
            }
        }
    }

    void StarTank::Render() {
        battle_game::SetTransformation(position_, rotation_);
        battle_game::SetTexture(0);
        battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
        battle_game::DrawModel(tank_body_model_index);
        battle_game::SetRotation(turret_rotation_);
        battle_game::DrawModel(tank_turret_model_index);
    }

    void StarTank::Update() {
        StarTankMove(3.0f, glm::radians(180.0f));
        TurretRotate();
        Fire();
    }

    void StarTank::StarTankMove(float move_speed, float rotate_angular_speed) {
        auto player = game_core_->GetPlayer(player_id_);
        if (player) {
            auto& input_data = player->GetInputData();
            glm::vec2 offset{ 0.0f };
            if (input_data.key_down[GLFW_KEY_W]) {
                offset.y += 1.0f;
            }
            if (input_data.key_down[GLFW_KEY_S]) {
                offset.y -= 1.0f;
            }
            float speed = move_speed * GetSpeedScale();
            offset *= kSecondPerTick * speed;
            auto new_position =
                position_ + glm::vec2{ glm::rotate(glm::mat4{1.0f}, rotation_,
                                                  glm::vec3{0.0f, 0.0f, 1.0f}) *
                                      glm::vec4{offset, 0.0f, 0.0f} };
            if (!game_core_->IsBlockedByObstacles(new_position)) {
                game_core_->PushEventMoveUnit(id_, new_position);
            }
            float rotation_offset = 0.0f;
            if (input_data.key_down[GLFW_KEY_A]) {
                rotation_offset += 1.0f;
            }
            if (input_data.key_down[GLFW_KEY_D]) {
                rotation_offset -= 1.0f;
            }
            rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
            game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
        }
    }

    void StarTank::TurretRotate() {
        auto player = game_core_->GetPlayer(player_id_);
        if (player) {
            auto& input_data = player->GetInputData();
            auto diff = input_data.mouse_cursor_position - position_;
            if (glm::length(diff) < 1e-4) {
                turret_rotation_ = rotation_;
            }
            else {
                turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
            }
        }
    }

    void StarTank::CreateBullet(float angle) {
        auto velocity = Rotate(glm::vec2{ 0.0f, 20.0f }, angle);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({ 0.0f, 1.2f }, angle),
            angle, GetDamageScale(), velocity);
    }

    void StarTank::Fire() {
        //Front x3  Sides x1  Back x2
        if (attack_count_down_) {
            attack_count_down_--;
            CreateBullet(turret_rotation_);
            if (attack_count_down_)
                CreateBullet(turret_rotation_ + glm::radians(180.0f));
        }
        if (fire_count_down_ > 0) {
            fire_count_down_--;
        }
        else {
            auto player = game_core_->GetPlayer(player_id_);
            if (player) {
                auto& input_data = player->GetInputData();
                if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
                    CreateBullet(turret_rotation_);
                    CreateBullet(turret_rotation_ + glm::radians(90.0f));
                    CreateBullet(turret_rotation_ - glm::radians(90.0f));
                    CreateBullet(turret_rotation_ + glm::radians(180.0f));
                    attack_count_down_ = 2;
                    fire_count_down_ = kTickPerSecond * 1.5f;  // Fire interval 1.5 second.
                }
            }
        }
    }

    bool StarTank::IsHit(glm::vec2 position) const {
        position = WorldToLocal(position);
        return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
            position.y < 1.0f && position.x + position.y < 1.6f &&
            position.y - position.x < 1.6f;
    }

    const char* StarTank::UnitName() const {
        return "Star Tank";
    }

    const char* StarTank::Author() const {
        return "ybweybwe";
    }
}  // namespace battle_game::unit
