#pragma once
#include "battle_game/core/obstacle.h"

namespace battle_game::obstacle {
    class SafetyDeclaration : public Obstacle {
    public:
        SafetyDeclaration(GameCore* game_core,
            uint32_t id,
            glm::vec2 position,
            float rotation = 0.0f,
            glm::vec2 scale = glm::vec2{ 3.0f, 3.0f });

    private:
        [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
        void Render() override;
        void Update() override;
        glm::vec2 scale_{ 3.0f };
        uint32_t valid_time_{ 0 };
    };
}  // namespace battle_game::obstacle
