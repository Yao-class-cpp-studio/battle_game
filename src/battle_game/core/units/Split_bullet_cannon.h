#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"
//
// Created by 13552 on 2023/1/8.
//
namespace battle_game::unit{
    class TNL : public Tank {
    public:
        TNL(GameCore *game_core, uint32_t id, uint32_t player_id);
        void Render() override;
        void Update() override;
    protected:
        void Fire();
        [[nodiscard]] const char *UnitName() const override;
        [[nodiscard]] const char *Author() const override;
    };
}
