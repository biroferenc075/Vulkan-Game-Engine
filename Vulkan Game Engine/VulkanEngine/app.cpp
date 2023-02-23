#include "app.hpp"
#include "VE_camera.hpp"
#include "VE_render_system.hpp"
#include "VE_buffer.hpp"
#include "keyboard_test.hpp"

#include <stdexcept>
#include <array>
#include <numeric>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace VE {

    struct UBO {
        glm::mat4 projectionView{ 1.f };
        glm::vec3 lightDir = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
    };
	App::App() {
        globalPool = VEDescriptorPool::Builder(veDevice)
            .setMaxSets(VESwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VESwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VESwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
		loadGameObjects();

	}

	App::~App() {}

    auto currentTime = std::chrono::high_resolution_clock::now();
    void App::run() {
        std::vector<std::unique_ptr<VEBuffer>> uboBuffers(VESwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++ ){
            uboBuffers[i] = std::make_unique<VEBuffer>(
                veDevice, sizeof(UBO), VESwapChain::MAX_FRAMES_IN_FLIGHT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = VEDescriptorSetLayout::Builder(veDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(VESwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = gameObjects[0].texture->textureImageView;
            imageInfo.sampler = gameObjects[0].texture->textureSampler;

            VEDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .writeImage(1, &imageInfo)
                .build(globalDescriptorSets[i]);
        }

        VEBuffer UBOBuffer{
            veDevice, sizeof(UBO), VESwapChain::MAX_FRAMES_IN_FLIGHT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, veDevice.properties.limits.minUniformBufferOffsetAlignment, };
        UBOBuffer.map();

        VERenderSystem renderSystem(veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        VECamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = VEGameObject::createGameObject();
        KeyboardController cameraController{};



        while (!veWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = veRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
            if (auto commandBuffer = veRenderer.beginFrame()) {
                int frameIndex = veRenderer.getFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};
                // update
                UBO ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();


                //rendering
                veRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo, gameObjects);
                veRenderer.endSwapChainRenderPass(commandBuffer);
                veRenderer.endFrame();
            }

        }

        vkDeviceWaitIdle(veDevice.device());
    }
	void App::loadGameObjects() {
        std::shared_ptr<VEModel> veModel = VEModel::createModelFromFile(veDevice, "models/Scaniverse.obj");
        std::shared_ptr<VETexture> veTexture = VETexture::createTextureFromFile(veDevice, "textures/Scaniverse.jpg");
        auto obj = VEGameObject::createGameObject();
        obj.model = veModel;
        obj.texture = veTexture;
        obj.transform.translation = { 0.0f, 0.0f, 2.5f };
        obj.transform.scale = { 1.5f, 1.5f, 1.5f };

        gameObjects.push_back(std::move(obj));
    }
}