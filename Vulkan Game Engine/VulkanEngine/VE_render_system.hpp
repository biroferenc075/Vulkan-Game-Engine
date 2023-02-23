#pragma once

#include "VE_pipeline.hpp"
#include "VE_device.hpp"
#include "VE_model.hpp"
#include "VE_gameobject.hpp"
#include "VE_camera.hpp"
#include "VE_frame_info.hpp"

#include <memory>
#include <vector>
namespace VE {
	class VERenderSystem {
	public:

		VERenderSystem(VEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~VERenderSystem();

		void renderGameObjects(FrameInfo& frameInfo, std::vector<VEGameObject>& gameobjects);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		VEDevice& veDevice;
		std::unique_ptr<VEPipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
		VERenderSystem(const VERenderSystem&);
		VERenderSystem& operator=(const VERenderSystem&);
	};
}