#pragma once

#include "VE_camera.hpp"

#include <vulkan/vulkan.h>

namespace VE {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VECamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}