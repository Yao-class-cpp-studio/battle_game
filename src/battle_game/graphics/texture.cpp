#include "battle_game/graphics//texture.h"

#include "absl/strings/match.h"
#include "grassland/util/util.h"
#include "stb_image.h"
#include "stb_image_write.h"

namespace battle_game {

Texture::Texture(uint32_t width,
                 uint32_t height,
                 const glm::vec4 &color,
                 SampleType sample_type) {
  width_ = width;
  height_ = height;
  buffer_.resize(width * height);
  sample_type_ = sample_type;
  std::fill(buffer_.data(), buffer_.data() + width * height, color);
}

Texture::Texture(uint32_t width,
                 uint32_t height,
                 const glm::vec4 *color_buffer,
                 SampleType sample_type) {
  width_ = width;
  height_ = height;
  buffer_.resize(width * height);
  sample_type_ = sample_type;
  std::memcpy(buffer_.data(), color_buffer,
              sizeof(glm::vec4) * width_ * height_);
}

void Texture::Resize(uint32_t width, uint32_t height) {
  std::vector<glm::vec4> new_buffer(width * height);
  for (int i = 0; i < std::min(height, height_); i++) {
    std::memcpy(new_buffer.data() + width * i, buffer_.data() + width_ * i,
                sizeof(glm::vec4) * std::min(width, width_));
  }
  width_ = width;
  height_ = height;
  buffer_ = new_buffer;
}

bool Texture::Load(const std::string &file_path, Texture &texture) {
  int x, y, c;
  if (absl::EndsWithIgnoreCase(file_path, ".hdr")) {
    auto result = stbi_loadf(file_path.c_str(), &x, &y, &c, 4);
    if (result) {
      texture = Texture(x, y, reinterpret_cast<glm::vec4 *>(result),
                        SAMPLE_TYPE_LINEAR);
      stbi_image_free(result);
    } else {
      return false;
    }
  } else {
    auto result = stbi_load(file_path.c_str(), &x, &y, &c, 4);
    if (result) {
      std::vector<glm::vec4> convert_buffer(x * y);
      const float inv_255 = 1.0f / 255.0f;
      for (int i = 0; i < x * y; i++) {
        convert_buffer[i] = glm::vec4{result[i * 4], result[i * 4 + 1],
                                      result[i * 4 + 2], result[i * 4 + 3]} *
                            inv_255;
      }
      texture = Texture(x, y, convert_buffer.data(), SAMPLE_TYPE_LINEAR);
      stbi_image_free(result);
    } else {
      return false;
    }
  }
  return true;
}

void Texture::Store(const std::string &file_path) {
  if (absl::EndsWithIgnoreCase(file_path, ".hdr")) {
    stbi_write_hdr(file_path.c_str(), width_, height_, 4,
                   reinterpret_cast<float *>(buffer_.data()));
  } else {
    std::vector<uint8_t> convert_buffer(width_ * height_ * 4);
    auto float_to_uint8 = [](float x) {
      return std::min(std::max(std::lround(x * 255.0f), 0l), 255l);
    };
    for (int i = 0; i < width_ * height_; i++) {
      convert_buffer[i * 4] = float_to_uint8(buffer_[i].x);
      convert_buffer[i * 4 + 1] = float_to_uint8(buffer_[i].y);
      convert_buffer[i * 4 + 2] = float_to_uint8(buffer_[i].z);
      convert_buffer[i * 4 + 3] = float_to_uint8(buffer_[i].w);
    }
    if (absl::EndsWithIgnoreCase(file_path, ".png")) {
      stbi_write_png(file_path.c_str(), width_, height_, 4,
                     convert_buffer.data(), width_ * 4);
    } else if (absl::EndsWithIgnoreCase(file_path, ".bmp")) {
      stbi_write_bmp(file_path.c_str(), width_, height_, 4,
                     convert_buffer.data());
    } else if (absl::EndsWithIgnoreCase(file_path, ".jpg") ||
               absl::EndsWithIgnoreCase(file_path, ".jpeg")) {
      stbi_write_jpg(file_path.c_str(), width_, height_, 4,
                     convert_buffer.data(), 100);
    } else {
      LAND_ERROR("Unknown file format \"{}\"", file_path.c_str());
    }
  }
}

void Texture::SetSampleType(SampleType sample_type) {
  sample_type_ = sample_type;
}

SampleType Texture::GetSampleType() const {
  return sample_type_;
}

glm::vec4 &Texture::operator()(int x, int y) {
  x = std::min(int(width_ - 1), std::max(x, 0));
  y = std::min(int(height_ - 1), std::max(y, 0));
  return buffer_[y * width_ + x];
}

const glm::vec4 &Texture::operator()(int x, int y) const {
  x = std::min(int(width_ - 1), std::max(x, 0));
  y = std::min(int(height_ - 1), std::max(y, 0));
  return buffer_[y * width_ + x];
}

glm::vec4 Texture::Sample(glm::vec2 tex_coord) const {
  tex_coord = tex_coord - glm::floor(tex_coord);
  tex_coord *= glm::vec2{width_, height_};
  if (sample_type_ == SAMPLE_TYPE_LINEAR) {
    int x = std::lround(tex_coord.x - 0.5f);
    int y = std::lround(tex_coord.y - 0.5f);
    float fx = tex_coord.x - float(x);
    float fy = tex_coord.y - float(y);
    return operator()(x, y) * (1.0f - fx) * (1.0f - fy) +
           operator()(x + 1, y) * (fx) * (1.0f - fy) +
           operator()(x, y + 1) * (1.0f - fx) * (fy) +
           operator()(x + 1, y + 1) * (fx) * (fy);
  } else {
    return operator()(std::lround(tex_coord.x), std::lround(tex_coord.y));
  }
}

uint32_t Texture::GetWidth() const {
  return width_;
}

uint32_t Texture::GetHeight() const {
  return height_;
}

glm::vec4 *Texture::GetBuffer() {
  return buffer_.data();
}

const glm::vec4 *Texture::GetBuffer() const {
  return buffer_.data();
}

Texture Texture::Load(const std::string &file_path) {
  Texture result;
  Load(file_path, result);
  return result;
}

}  // namespace battle_game
