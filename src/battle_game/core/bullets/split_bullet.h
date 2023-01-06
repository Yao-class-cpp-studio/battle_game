//
// Created by 13552 on 2023/1/8.
//
#pragma once
#include "battle_game/core/bullet.h"
#include "battle_game/core/bullets/cannon_ball.h"
namespace battle_game::bullet{
    class split_bullet : public Bullet {
    public:
        split_bullet(GameCore *core,
                uint32_t id,
                uint32_t unit_id,
                uint32_t player_id,
                glm::vec2 position,
                float rotation,
                float damage_scale,
                glm::vec2 velocity);
        ~split_bullet() override;
        void Render() override;
        void Update() override;
    private:
        glm::vec2 velocity_{};
        int split_countdown;
    };
}