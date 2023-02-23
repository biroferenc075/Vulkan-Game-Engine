#pragma once

#include "VE_window.hpp"
#include "VE_device.hpp"
#include "VE_model.hpp"
#include "VE_gameobject.hpp"
#include "VE_renderer.hpp"
#include "VE_descriptors.hpp"
#include "VE_texture.hpp"

#include <memory>
#include <vector>
#include <chrono>
namespace VE {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();
		void run();
	private:
		void loadGameObjects();
		VEWindow veWindow{ WIDTH, HEIGHT, "Vulkan" };
		VEDevice veDevice{ veWindow };
		VERenderer veRenderer{ veWindow, veDevice };

		std::unique_ptr<VEDescriptorPool> globalPool{};
		std::vector<VEGameObject> gameObjects;
		App(const App&);
		App& operator=(const App&);
	};
}