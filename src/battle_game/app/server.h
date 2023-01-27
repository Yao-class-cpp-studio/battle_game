#pragma once
#include <queue>
#include <set>
#include <vector>

#include "asio.hpp"
#include "battle_game/app/message.h"
#include "battle_game/core/game_core.h"

namespace battle_game {
constexpr int kMaxPlayerNumber = 100;
class Server : public std::enable_shared_from_this<Server> {
 public:
  Server(asio::io_context &io_context,
         const asio::ip::tcp::endpoint &endpoint,
         std::function<void(const CompleteInputData &)> deliver_input_data =
             nullptr);

  void Build();
  void Close();
  bool IsRunning() const;
  void Start();
  void Stop();

 private:
  class Participant : public std::enable_shared_from_this<Participant> {
   public:
    Participant(asio::ip::tcp::socket socket, Server &server);
    void Start();
    void Close();
    void Deliver(const ByteString &message);
    uint32_t GetPlayerId() const;
    void SetPlayerId(uint32_t player_id);
    const MessageInputData &GetInputData() const;

   private:
    void DoRead();
    void DoWrite();

    asio::ip::tcp::socket socket_;
    ByteString write_messages_;
    MessageInputData input_data_{};
    Server &server_;
    uint32_t player_id_{0};
    uint8_t buffer_[MessageInputData::length];
  };

  typedef std::shared_ptr<Participant> ParticipantPtr;

  void DoAccept();
  void RegisterTimer();
  void CloseTimer();
  void Broadcast();
  void Join(ParticipantPtr participant);
  void Leave(ParticipantPtr participant);
  void Deliver(const ByteString &message);

  asio::io_context &io_context_;
  asio::ip::tcp::acceptor acceptor_;
  std::unique_ptr<asio::steady_timer> timer_;
  std::set<ParticipantPtr> participants_;
  CompleteInputData input_data_;
  std::function<void(const CompleteInputData &)> deliver_input_data_;
  std::atomic<bool> running_{false};
  uint32_t player_cnt_;
};
}  // namespace battle_game
