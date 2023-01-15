#include <grassland/util/logging.h>
#include <grassland/vulkan/sampler.h>

namespace grassland::vulkan {

Sampler::Sampler(Device *device,
                 VkFilter filter,
                 VkSamplerAddressMode address_mode,
                 VkBool32 anisotropy_enable,
                 VkBorderColor border_color,
                 VkSamplerMipmapMode mipmap_mode)
    : Sampler(device,
              filter,
              filter,
              address_mode,
              address_mode,
              address_mode,
              anisotropy_enable,
              border_color,
              mipmap_mode) {
}

Sampler::Sampler(Device *device,
                 VkFilter mag_filter,
                 VkFilter min_filter,
                 VkSamplerAddressMode address_mode_u,
                 VkSamplerAddressMode address_mode_v,
                 VkSamplerAddressMode address_mode_w,
                 VkBool32 anisotropy_enable,
                 VkBorderColor border_color,
                 VkSamplerMipmapMode mipmap_mode) {
  device_ = device;
  Reset(mag_filter, min_filter, address_mode_u, address_mode_v, address_mode_w,
        anisotropy_enable, border_color, mipmap_mode);
}

Sampler::~Sampler() {
  Clear();
}

void Sampler::Reset(VkFilter filter,
                    VkSamplerAddressMode address_mode,
                    VkBool32 anisotropy_enable,
                    VkBorderColor border_color,
                    VkSamplerMipmapMode mipmap_mode) {
  Reset(filter, filter, address_mode, address_mode, address_mode,
        anisotropy_enable, border_color, mipmap_mode);
}

void Sampler::Reset(VkFilter mag_filter,
                    VkFilter min_filter,
                    VkSamplerAddressMode address_mode_u,
                    VkSamplerAddressMode address_mode_v,
                    VkSamplerAddressMode address_mode_w,
                    VkBool32 anisotropy_enable,
                    VkBorderColor border_color,
                    VkSamplerMipmapMode mipmap_mode) {
  Clear();
  if (!device_->GetPhysicalDevice()->GetFeatures().samplerAnisotropy) {
    anisotropy_enable = VK_FALSE;
  }
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = mag_filter;
  samplerInfo.minFilter = min_filter;
  samplerInfo.addressModeU = address_mode_u;
  samplerInfo.addressModeV = address_mode_v;
  samplerInfo.addressModeW = address_mode_w;
  samplerInfo.anisotropyEnable = anisotropy_enable;
  samplerInfo.maxAnisotropy = anisotropy_enable
                                  ? device_->GetPhysicalDevice()
                                        ->GetProperties()
                                        .limits.maxSamplerAnisotropy
                                  : 1.0f;
  samplerInfo.borderColor = border_color;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = mipmap_mode;

  if (vkCreateSampler(device_->GetHandle(), &samplerInfo, nullptr, &handle_) !=
      VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create texture sampler!");
  }
}

void Sampler::Clear() {
  if (handle_) {
    vkDestroySampler(device_->GetHandle(), GetHandle(), nullptr);
    handle_ = nullptr;
  }
}

}  // namespace grassland::vulkan
