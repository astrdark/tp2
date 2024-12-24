#include <cstdint>
#include <optional>
#include <print>
#include <span>
#include <type_traits>
#include <vector>

struct VkInstance {};

struct VkInstanceCreateInfo {
	std::string_view application_name;
	uint64_t application_version;

	std::string_view engine_name;
	uint64_t engine_version;

	std::vector<std::string_view> extensions;
};

struct VkInstanceBuilder {
	VkInstanceCreateInfo ci;

#define BUILDER_MEMBER($type, $name) VkInstanceBuilder& $name($type $name) { \
		ci. $name = $name; \
		return *this; \
	}

	BUILDER_MEMBER(std::string_view, application_name);
	BUILDER_MEMBER(uint64_t, application_version);

	BUILDER_MEMBER(std::string_view, engine_name);
	BUILDER_MEMBER(uint64_t, engine_version);

#undef BUILDER_MEMBER

	VkInstanceBuilder& add_extension(std::string_view extension) {
		ci.extensions.push_back(extension);
		return *this;
	}

	VkInstance build() {
		return VkInstance{};
	}
};

struct VkDevice {};

struct VkDeviceCreateInfo {
	VkInstance instance;
	std::vector<std::string_view> extensions;
};

struct VkDeviceBuilder {
	VkDeviceCreateInfo ci;

	VkDeviceBuilder& instance(VkInstance instance) {
		ci.instance = instance;
		return *this;
	}

	VkDeviceBuilder& extension(std::string_view name) {
		ci.extensions.push_back(name);
		return *this;
	}

	VkDevice build() {
		return VkDevice{};
	}
};

int main() {
	VkDeviceBuilder{}
		.instance(VkInstanceBuilder{}
			.application_name("Unreal Tournament")
			.application_version(0x4)
			.engine_name("Unreal Engine 4")
			.engine_version(0x4'11'1)
			.add_extension("VK_KHR_surface")
			.add_extension("VK_KHR_debug_object")
			.build())
		.extension("VK_KHR_acceleration_structure")
		.build();
}
