#include <grassland/util/util.h>
#include <grassland/vulkan/buffer.h>
#include <grassland/vulkan/framework/texture_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace grassland::vulkan::framework {

namespace {

uint8_t SFLOATtoUNORM(float s) {
  int value = std::min(std::max(int(std::lround(s * 255.0f + 0.5f)), 0), 255);
  return value;
}

void HostRGBA8ToDeviceBuffer(Core *core_,
                             const std::vector<uint8_t> &host_rgba8,
                             std::unique_ptr<Buffer> &buffer,
                             VkFormat format) {
  if (format == VK_FORMAT_R8G8B8A8_UNORM) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(uint8_t) * host_rgba8.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    std::memcpy(buffer->Map(), host_rgba8.data(),
                sizeof(uint8_t) * host_rgba8.size());
    buffer->Unmap();
  } else if (format == VK_FORMAT_B8G8R8A8_UNORM) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(uint8_t) * host_rgba8.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba8.size(); i += 4) {
      device_buffer[i + 2] = SFLOATtoUNORM(host_rgba8[i]);
      device_buffer[i + 1] = SFLOATtoUNORM(host_rgba8[i + 1]);
      device_buffer[i] = SFLOATtoUNORM(host_rgba8[i + 2]);
      device_buffer[i + 3] = SFLOATtoUNORM(host_rgba8[i + 3]);
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_R32G32B32A32_SFLOAT) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(float) * host_rgba8.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    auto *device_buffer = reinterpret_cast<float *>(buffer->Map());
    float inv_255 = 1.0f / 255.0f;
    for (int i = 0; i < host_rgba8.size(); i++) {
      device_buffer[i] = float(host_rgba8[i]) * inv_255;
    }
    buffer->Unmap();
  } else {
    LAND_ERROR("[Vulkan] format #{} doesn't support conversion.", format);
  }
}

void DeviceBufferToHostRGBA8(Core *core_,
                             std::vector<uint8_t> &host_rgba8,
                             const std::unique_ptr<Buffer> &buffer,
                             VkFormat format) {
  if (format == VK_FORMAT_R8G8B8A8_UNORM) {
    std::memcpy(host_rgba8.data(), buffer->Map(),
                sizeof(uint8_t) * host_rgba8.size());
    buffer->Unmap();
  } else if (format == VK_FORMAT_B8G8R8A8_UNORM) {
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba8.size(); i += 4) {
      host_rgba8[i] = device_buffer[i + 2];
      host_rgba8[i + 1] = device_buffer[i + 1];
      host_rgba8[i + 2] = device_buffer[i];
      host_rgba8[i + 3] = device_buffer[i + 3];
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_R32G32B32A32_SFLOAT) {
    auto *device_buffer = reinterpret_cast<float *>(buffer->Map());
    for (int i = 0; i < host_rgba8.size(); i++) {
      host_rgba8[i] = SFLOATtoUNORM(device_buffer[i]);
    }
    buffer->Unmap();
  } else {
    LAND_ERROR("[Vulkan] format #{} doesn't support conversion.", format);
  }
}

void HostRGBA32FToDeviceBuffer(Core *core_,
                               const std::vector<float> &host_rgba32f,
                               std::unique_ptr<Buffer> &buffer,
                               VkFormat format) {
  if (format == VK_FORMAT_R8G8B8A8_UNORM) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(uint8_t) * host_rgba32f.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba32f.size(); i++) {
      device_buffer[i] = SFLOATtoUNORM(host_rgba32f[i]);
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_B8G8R8A8_UNORM) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(uint8_t) * host_rgba32f.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba32f.size(); i += 4) {
      device_buffer[i + 2] = SFLOATtoUNORM(host_rgba32f[i]);
      device_buffer[i + 1] = SFLOATtoUNORM(host_rgba32f[i + 1]);
      device_buffer[i] = SFLOATtoUNORM(host_rgba32f[i + 2]);
      device_buffer[i + 3] = SFLOATtoUNORM(host_rgba32f[i + 3]);
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_R32G32B32A32_SFLOAT) {
    buffer = std::make_unique<Buffer>(
        core_->GetDevice(), sizeof(float) * host_rgba32f.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    std::memcpy(buffer->Map(), host_rgba32f.data(),
                sizeof(float) * host_rgba32f.size());
    buffer->Unmap();
  } else {
    LAND_ERROR("[Vulkan] format #{} doesn't support conversion.", format);
  }
}

void DeviceBufferToHostRGBA32F(Core *core_,
                               std::vector<float> &host_rgba32f,
                               const std::unique_ptr<Buffer> &buffer,
                               VkFormat format) {
  constexpr float inv_255 = 1.0f / 255.0f;
  if (format == VK_FORMAT_R8G8B8A8_UNORM) {
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba32f.size(); i++) {
      host_rgba32f[i] = float(device_buffer[i]) * inv_255;
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_B8G8R8A8_UNORM) {
    auto *device_buffer = reinterpret_cast<uint8_t *>(buffer->Map());
    for (int i = 0; i < host_rgba32f.size(); i += 4) {
      host_rgba32f[i] = float(device_buffer[i + 2]) * inv_255;
      host_rgba32f[i + 1] = float(device_buffer[i + 1]) * inv_255;
      host_rgba32f[i + 2] = float(device_buffer[i]) * inv_255;
      host_rgba32f[i + 3] = float(device_buffer[i + 3]) * inv_255;
    }
    buffer->Unmap();
  } else if (format == VK_FORMAT_R32G32B32A32_SFLOAT) {
    std::memcpy(host_rgba32f.data(), buffer->Map(),
                sizeof(float) * host_rgba32f.size());
    buffer->Unmap();
  } else {
    LAND_ERROR("[Vulkan] format #{} doesn't support conversion.", format);
  }
}
}  // namespace

TextureImage::TextureImage(Core *core,
                           uint32_t width,
                           uint32_t height,
                           VkFormat format,
                           VkImageUsageFlags usage) {
  core_ = core;
  image_ =
      std::make_unique<Image>(core_->GetDevice(), width, height, format, usage);
  image_view_ = std::make_unique<ImageView>(image_.get());
}

TextureImage::~TextureImage() = default;

Core *TextureImage::GetCore() const {
  return core_;
}

void TextureImage::ReadImage(const char *path) {
  int x, y, c;
  auto buffer = stbi_load(path, &x, &y, &c, 4);
  if (!buffer) {
    LAND_ERROR("[Vulkan] failed to load image \"{}\".", path);
  }
  Resize(x, y);
  std::vector<uint8_t> host_image_buffer(x * y * 4);
  std::memcpy(host_image_buffer.data(), buffer, x * y * 4);
  stbi_image_free(buffer);
  std::unique_ptr<Buffer> image_buffer;
  HostRGBA8ToDeviceBuffer(core_, host_image_buffer, image_buffer,
                          image_->GetFormat());
  UploadImage(core_->GetCommandPool(), image_.get(), image_buffer.get());
}

void TextureImage::WriteImage(const char *path) {
  std::vector<uint8_t> buffer(image_->GetWidth() * image_->GetHeight() * 4);
  size_t channel_size = 1;
  if (image_->GetFormat() == VK_FORMAT_R32G32B32A32_SFLOAT) {
    channel_size = 4;
  }
  std::unique_ptr<Buffer> image_buffer = std::make_unique<Buffer>(
      core_->GetDevice(),
      sizeof(uint8_t) * image_->GetWidth() * image_->GetHeight() * 4 *
          channel_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  DownloadImage(core_->GetCommandPool(), image_.get(), image_buffer.get());
  DeviceBufferToHostRGBA8(core_, buffer, image_buffer, image_->GetFormat());
  auto tail = path + std::strlen(path);
  while (*tail != '.' && tail > path) {
    tail--;
  }
  std::string suffix{tail};
  if (suffix == ".bmp") {
    stbi_write_bmp(path, int(image_->GetWidth()), int(image_->GetHeight()), 4,
                   buffer.data());
  } else if (suffix == ".jpg" || suffix == ".jpeg") {
    stbi_write_jpg(path, int(image_->GetWidth()), int(image_->GetHeight()), 4,
                   buffer.data(), 100);
  } else {
    stbi_write_png(path, int(image_->GetWidth()), int(image_->GetHeight()), 4,
                   buffer.data(), int(image_->GetWidth()) * 4);
  }
}

Image *TextureImage::GetImage() const {
  return image_.get();
}

ImageView *TextureImage::GetImageView() const {
  return image_view_.get();
}

void TextureImage::ReadHDRImage(const char *path) {
  stbi_hdr_to_ldr_gamma(1.0f);
  int x, y, c;
  auto buffer = stbi_loadf(path, &x, &y, &c, STBI_rgb_alpha);
  if (!buffer) {
    LAND_ERROR("[Vulkan] failed to load image \"{}\".", path);
  }
  Resize(x, y);
  std::vector<float> host_image_buffer(x * y * 4);
  std::memcpy(host_image_buffer.data(), buffer, x * y * 4 * sizeof(float));
  stbi_image_free(buffer);
  std::unique_ptr<Buffer> image_buffer;
  HostRGBA32FToDeviceBuffer(core_, host_image_buffer, image_buffer,
                            image_->GetFormat());
  UploadImage(core_->GetCommandPool(), image_.get(), image_buffer.get());
}

void TextureImage::WriteHDRImage(const char *path) {
  std::vector<float> buffer(image_->GetWidth() * image_->GetHeight() * 4);
  size_t channel_size = 1;
  if (image_->GetFormat() == VK_FORMAT_R32G32B32A32_SFLOAT) {
    channel_size = 4;
  }
  std::unique_ptr<Buffer> image_buffer = std::make_unique<Buffer>(
      core_->GetDevice(),
      sizeof(uint8_t) * image_->GetWidth() * image_->GetHeight() * 4 *
          channel_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  DownloadImage(core_->GetCommandPool(), image_.get(), image_buffer.get());
  DeviceBufferToHostRGBA32F(core_, buffer, image_buffer, image_->GetFormat());
  stbi_write_hdr(path, int(image_->GetWidth()), int(image_->GetHeight()), 4,
                 buffer.data());
}

void TextureImage::ClearColor(VkClearColorValue clear_color) const {
  grassland::vulkan::TransitImageLayout(
      core_->GetCommandBuffer()->GetHandle(), image_->GetHandle(),
      VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

  VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  vkCmdClearColorImage(core_->GetCommandBuffer()->GetHandle(),
                       image_->GetHandle(), VK_IMAGE_LAYOUT_GENERAL,
                       &clear_color, 1, &range);
}

void TextureImage::ClearDepth(
    VkClearDepthStencilValue clear_depth_stencil) const {
  TransitImageLayout(core_->GetCommandBuffer()->GetHandle(),
                     image_->GetHandle(), VK_IMAGE_LAYOUT_GENERAL,
                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

  VkImageSubresourceRange range{VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
  vkCmdClearDepthStencilImage(core_->GetCommandBuffer()->GetHandle(),
                              image_->GetHandle(), VK_IMAGE_LAYOUT_GENERAL,
                              &clear_depth_stencil, 1, &range);
}

void TextureImage::Resize(uint32_t width, uint32_t height) {
  image_view_.reset();
  image_ = std::make_unique<Image>(core_->GetDevice(), width, height,
                                   image_->GetFormat(), image_->GetUsage());
  image_view_ = std::make_unique<ImageView>(image_.get());
}

}  // namespace grassland::vulkan::framework
