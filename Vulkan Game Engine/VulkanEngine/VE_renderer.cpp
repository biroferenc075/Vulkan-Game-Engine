#include "VE_renderer.hpp"
#include <stdexcept>
#include <array>


namespace VE {
	VERenderer::VERenderer(VEWindow& window, VEDevice& device) : veWindow{ window }, veDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	VERenderer::~VERenderer() {
		freeCommandBuffers();
	}


	void VERenderer::recreateSwapChain() {
		auto extent = veWindow.getExtent();

		while (extent.width == 0 || extent.height == 0) {
			extent = veWindow.getExtent();
			glfwWaitEvents();
		}


		vkDeviceWaitIdle(veDevice.device());
		if (veSwapChain == nullptr)
		{
			veSwapChain = std::make_unique<VESwapChain>(veDevice, extent);
		}
		else {
			std::shared_ptr<VESwapChain> oldSwapChain = std::move(veSwapChain);
			veSwapChain = std::make_unique<VESwapChain>(veDevice, extent, oldSwapChain);
			if (!oldSwapChain->compareSwapFormats(*veSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		
		}

	}


	void VERenderer::createCommandBuffers() {
		commandBuffers.resize(VESwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = veDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(veDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VERenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(veDevice.device(), veDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer VERenderer::beginFrame() {
		auto result = veSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void VERenderer::endFrame() {
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = veSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result != VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || veWindow.wasWindowResized()) {
			veWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VESwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void VERenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = veSwapChain->getRenderPass();
		renderPassInfo.framebuffer = veSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = veSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(veSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(veSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, veSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void VERenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {

		vkCmdEndRenderPass(commandBuffer);

	}

}