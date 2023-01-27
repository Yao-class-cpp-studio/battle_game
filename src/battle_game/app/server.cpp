#include "battle_game/app/server.h"

namespace battle_game {
using asio::ip::tcp;
Server::Participant::Participant(asio::ip::tcp::socket socket, Server &server)
    : socket_(std::move(socket)), server_(server) {
}

void Server::Participant::Start() {
  server_.Join(shared_from_this());
  DoRead();
}

void Server::Participant::Close() {
  if (socket_.is_open()) {
    socket_.cancel();
    socket_.close();
  }
}

void Server::Participant::Deliver(const ByteString &message) {
  bool write_in_progress = !write_messages_.empty();
  write_messages_.push(message);
  if (!write_in_progress) {
    DoWrite();
  }
}

uint32_t Server::Participant::GetPlayerId() const {
  return player_id_;
}
void Server::Participant::SetPlayerId(uint32_t player_id) {
  player_id_ = player_id;
}
const MessageInputData &Server::Participant::GetInputData() const {
  return input_data_;
}

void Server::Participant::DoRead() {
  auto self(shared_from_this());
  asio::async_read(socket_, asio::buffer(buffer_, MessageInputData::length),
                   [this, self](asio::error_code ec, std::size_t /*length*/) {
                     if (!ec) {
                       input_data_ = buffer_;
                       if (player_id_) {
                         server_.input_data_[player_id_] = input_data_;
                       }
                       DoRead();
                     } else if (ec != asio::error::operation_aborted) {
                       server_.Leave(shared_from_this());
                     }
                   });
}

void Server::Participant::DoWrite() {
  auto self(shared_from_this());
  asio::async_write(socket_,
                    asio::buffer(write_messages_.front().data(),
                                 write_messages_.front().length()),
                    [this, self](asio::error_code ec, std::size_t /*length*/) {
                      if (!ec) {
                        write_messages_.pop();
                        if (!write_messages_.empty()) {
                          DoWrite();
                        }
                      } else if (ec != asio::error::operation_aborted) {
                        server_.Leave(shared_from_this());
                      }
                    });
}

Server::Server(
    asio::io_context &io_context,
    const tcp::endpoint &endpoint,
    std::function<void(const CompleteInputData &)> deliver_input_data)
    : io_context_(io_context),
      acceptor_(io_context, endpoint),
      player_cnt_(0),
      deliver_input_data_(deliver_input_data) {
}

void Server::Build() {
  DoAccept();
}

void Server::Close() {
  CloseTimer();
  if (acceptor_.is_open()) {
    acceptor_.cancel();
    acceptor_.close();
  }
}

void Server::Join(ParticipantPtr participant) {
  participants_.insert(participant);
}

void Server::Leave(ParticipantPtr participant) {
  if (participant->GetPlayerId()) {
    input_data_[participant->GetPlayerId()].input_data = InputData();
  }
  participant->Close();
  participants_.erase(participant);
}

void Server::DoAccept() {
  auto self(shared_from_this());
  acceptor_.async_accept([this, self](asio::error_code ec, tcp::socket socket) {
    if (!ec) {
      std::make_shared<Participant>(std::move(socket), *this)->Start();
    }
    DoAccept();
  });
}

bool Server::IsRunning() const {
  return running_;
}

void Server::Start() {
  auto self(shared_from_this());
  asio::post(io_context_, [this, self]() {
    player_cnt_ = participants_.size();
    if (player_cnt_ > kMaxPlayerNumber) {
      player_cnt_ = 0;
      return;
    }
    uint32_t current_id = 0;
    input_data_.resize(player_cnt_ + 1);
    for (auto participant : participants_) {
      participant->SetPlayerId(++current_id);
      input_data_[current_id] = participant->GetInputData();
      participant->Deliver((uint8_t)'{' +
                           ByteString(MessageInitial(player_cnt_, current_id)));
    }
    running_ = true;
    timer_ = std::make_unique<asio::steady_timer>(
        io_context_, asio::chrono::nanoseconds(
                         (long long)std::roundl(1e9 * kSecondPerTick * 0.9f)));
    RegisterTimer();
  });
}

void Server::Stop() {
  auto self(shared_from_this());
  asio::post(io_context_, [this, self]() {
    if (timer_) {
      timer_->cancel();
      timer_.reset();
    }
    Deliver({(uint8_t)'}'});
    for (auto participant : participants_) {
      participant->SetPlayerId(0);
    }
    input_data_.clear();
    running_ = false;
    player_cnt_ = 0;
  });
}

void Server::RegisterTimer() {
  auto self(shared_from_this());
  timer_->async_wait([this, self](asio::error_code ec) {
    if (!ec) {
      if (timer_) {
        timer_->cancel();
        timer_.reset();
      }
      timer_ = std::make_unique<asio::steady_timer>(
          io_context_, asio::chrono::nanoseconds(
                           (long long)std::roundl(1e9 * kSecondPerTick)));
      Broadcast();
      RegisterTimer();
    }
  });
}

void Server::CloseTimer() {
  if (timer_) {
    timer_->cancel();
    timer_.reset();
  }
}

void Server::Broadcast() {
  if (!running_) {
    return;
  }
  if (deliver_input_data_) {
    deliver_input_data_(input_data_);
  }
  ByteString message{};
  message.reserve(1 + MessageInputData::length * player_cnt_);
  message += (uint8_t)':';
  for (uint32_t i = 1; i <= player_cnt_; ++i) {
    message += input_data_[i];
  }
  Deliver(message);
}

void Server::Deliver(const ByteString &message) {
  for (auto participant : participants_) {
    if (participant->GetPlayerId()) {
      participant->Deliver(message);
    }
  }
}
}  // namespace battle_game
