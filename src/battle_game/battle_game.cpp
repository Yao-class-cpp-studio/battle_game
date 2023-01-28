#include <iostream>

#include "battle_game/app/app.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

using asio::ip::tcp;

constexpr int max_length = 4096;

void PrintUsage() {
  std::printf("Usage: ./battle_game\n");
  std::printf("       ./battle_game --server [<port>]\n");
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    asio::io_context io_context;
    battle_game::GameCore game_core;
    battle_game::AppSettings app_settings;
    battle_game::App app(app_settings, &game_core, io_context);
    app.Run();
  } else if (argc <= 3 && !std::strcmp(argv[1], "--server")) {
    try {
      asio::io_context io_context;
      uint16_t port = battle_game::default_port;
      if (argc == 3) {
        if (sscanf(argv[2], "%hu", &port) != 1) {
          throw std::invalid_argument("Invalid port number");
        }
      }
      std::shared_ptr<battle_game::Server> server =
          std::make_shared<battle_game::Server>(io_context,
                                                tcp::endpoint(tcp::v4(), port));
      server->Build();
      std::printf("Start server at port %hu\n", port);
      std::thread thread([&io_context]() { io_context.run(); });
      static char line[max_length + 1];
      for (;;) {
        if (server->IsRunning()) {
          std::printf(
              "Type 'stop' to stop the game, 'quit' to stop the server\n> ");
        } else {
          std::printf(
              "Type 'start' to start the game, 'quit' to stop the server\n> ");
        }
        if (!std::cin.getline(line, max_length) || !strcmp(line, "quit")) {
          break;
        }
        if (server->IsRunning() && !strcmp(line, "stop")) {
          server->Stop();
          while (server->IsRunning())
            ;
          std::printf("Stopped\n");
        } else if (!server->IsRunning() && !strcmp(line, "start")) {
          server->Start();
          while (!server->IsRunning())
            ;
          std::printf("Started\n");
        }
      }
      if (server->IsRunning()) {
        server->Stop();
      }
      server->Close();
      io_context.stop();
      thread.join();
    } catch (std::exception &e) {
      std::fprintf(stderr, "Error: %s\n", e.what());
      return 1;
    }
  } else {
    PrintUsage();
  }
  return 0;
}
