#pragma once

#include "VE_device.hpp"
#include "VE_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace VE {
	class VEModel {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec2 texCoord{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			bool operator==(const Vertex& other) const {
				return position == other.position &&
					texCoord == other.texCoord;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& fpath);
		};
		VEModel(VEDevice& device, const VEModel::Builder &builder);
		~VEModel();
		static std::unique_ptr<VEModel> createModelFromFile(VEDevice& device, const std::string& fpath);
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		VEDevice& veDevice;
		std::unique_ptr<VEBuffer> vertexBuffer;
		uint32_t vertexCount;


		bool hasIndexBuffer = false;
		std::unique_ptr<VEBuffer> indexBuffer;
		uint32_t indexCount;

		std::unique_ptr<VEBuffer> textureBuffer;

		VEModel(const VEModel&);
		VEModel& operator=(const VEModel&);
	};
}