#pragma once

#include "VE_window.hpp"
#include "VE_device.hpp"
#include "VE_swap_chain.hpp"
#include "VE_model.hpp"
#include <memory>
#include <vector>
namespace VE {
	class VERenderer {
	public:
		VERenderer(VEWindow& window, VEDevice& device);
		~VERenderer();

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
		bool isFrameinProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const { return commandBuffers[currentFrameIndex]; }
		VkRenderPass getSwapChainRenderPass() const { return veSwapChain->getRenderPass(); }
		float getAspectRatio() const {
			return veSwapChain->extentAspectRatio();
		}
		int getFrameIndex() const {
			return currentFrameIndex;
		}
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		VEWindow& veWindow;
		VEDevice& veDevice;
		std::unique_ptr<VESwapChain> veSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		VERenderer(const VERenderer&);
		VERenderer& operator=(const VERenderer&);
		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{ false};
	};
}