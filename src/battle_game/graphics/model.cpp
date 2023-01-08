#include "battle_game/graphics/model.h"

namespace battle_game {
Model::Model(const std::vector<ObjectVertex> &vertices,
             const std::vector<uint32_t> &indices)
    : vertices_(vertices), indices_(indices) {
}
}  // namespace battle_game
