#include "battle_game/app/app.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

int main() {
  battle_game::GameCore game_core;
  battle_game::AppSettings app_settings;
  battle_game::App app(app_settings, &game_core);
  app.Run();
}
