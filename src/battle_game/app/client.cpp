#include "battle_game/app/app.h"

namespace battle_game {
using asio::ip::tcp;
App::Client::Client(App *app)
    : app_(app), io_context_(app->io_context_), socket_(app->io_context_) {
}

void App::Client::Connect(const tcp::resolver::results_type &endpoint) {
  auto self(shared_from_this());
  asio::async_connect(socket_, endpoint,
                      [this, self](asio::error_code ec, tcp::endpoint) {
                        if (!ec) {
                          app_->SetMessage(u8"连接成功");
                          RegisterTimer();
                          DoReadHeader();
                        } else if (ec != asio::error::operation_aborted) {
                          app_->SetMessage(u8"错误：" + ec.message());
                          Close();
                        }
                      });
}

void App::Client::Close() {
  if (socket_.is_open()) {
    socket_.cancel();
    socket_.close();
  }
  CloseTimer();
}

void App::Client::CloseTimer() {
  if (timer_) {
    timer_->cancel();
    timer_.reset();
  }
}

uint32_t App::Client::GetPlayerCount() const {
  return init_message_.player_cnt;
}
uint32_t App::Client::GetPlayerId() const {
  return init_message_.player_id;
}

void App::Client::Quit() {
  app_->SetMessage(u8"连接中断");
  app_->Stop();
  Close();
}

void App::Client::DoReadHeader() {
  auto self(shared_from_this());
  asio::async_read(socket_, asio::buffer(buffer_, 1u),
                   [this, self](asio::error_code ec, std::size_t /*length*/) {
                     if (!ec) {
                       if (buffer_[0] == (uint8_t)'{') {
                         DoStart();
                       } else if (buffer_[0] == (uint8_t)':') {
                         input_data_.reserve(GetPlayerCount() + 1);
                         input_data_.resize(1);
                         DoReadBody();
                       } else if (buffer_[0] == (uint8_t)'}') {
                         DoStop();
                       } else {
                         Quit();
                       }
                     } else if (ec != asio::error::operation_aborted) {
                       Quit();
                     }
                   });
}

void App::Client::DoReadBody() {
  auto self(shared_from_this());
  if (input_data_.size() > GetPlayerCount()) {
    app_->AppendInputData(input_data_);
    DoReadHeader();
    return;
  }
  asio::async_read(socket_, asio::buffer(buffer_, MessageInputData::length),
                   [this, self](asio::error_code ec, std::size_t /*length*/) {
                     if (!ec) {
                       input_data_.emplace_back(buffer_);
                       DoReadBody();
                     } else if (ec != asio::error::operation_aborted) {
                       Quit();
                     }
                   });
}

void App::Client::DoWrite() {
  auto self(shared_from_this());
  asio::async_write(
      socket_, asio::buffer(write_message_.data(), write_message_.length()),
      [this, self](std::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          write_message_.clear();
        } else if (ec != asio::error::operation_aborted) {
          Quit();
        }
      });
}

void App::Client::DoStart() {
  auto self(shared_from_this());
  asio::async_read(socket_, asio::buffer(buffer_, MessageInitial::length),
                   [this, self](asio::error_code ec, std::size_t /*length*/) {
                     if (!ec) {
                       init_message_ = buffer_;
                       app_->Start();
                       DoReadHeader();
                     } else if (ec != asio::error::operation_aborted) {
                       Quit();
                     }
                   });
}

void App::Client::DoStop() {
  init_message_ = MessageInitial();
  app_->Stop();
  DoReadHeader();
}

void App::Client::RegisterTimer() {
  auto self(shared_from_this());
  CloseTimer();
  timer_ = std::make_unique<asio::steady_timer>(
      io_context_,
      asio::chrono::nanoseconds((long long)std::roundl(1e9 * kSecondPerTick)));
  timer_->async_wait([this, self](asio::error_code ec) {
    if (!ec) {
      RegisterTimer();
      Write();
    }
  });
}

void App::Client::Write() {
  auto self(shared_from_this());
  asio::post(io_context_, [this, self]() {
    bool write_in_progress = !write_message_.empty();
    write_message_ = MessageInputData(app_->selected_unit_, app_->input_data_);
    if (!write_in_progress) {
      DoWrite();
    }
  });
}
}  // namespace battle_game
