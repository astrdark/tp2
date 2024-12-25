#include <cstdint>
#include <string_view>
#include <vector>
#include <algorithm>
#include <span>
#include <ranges>
#include <array>
#include <print>

enum class VkPhysicalDeviceType {
	INTEGRATED_GPU,
	DEDICATED_GPU,
	CPU
};

using VkPhysicalDevice = uint64_t;
struct VkPhysicalDeviceProperties {
	std::string_view name;
	VkPhysicalDeviceType hardware_type;
	uint64_t dedicated_ram_bytes;
	uint64_t shared_ram_bytes;
};

using InputTuple = std::tuple<VkPhysicalDevice, VkPhysicalDeviceProperties>;
using StrategyTy = int32_t(VkPhysicalDevice, const VkPhysicalDeviceProperties&);

template<std::invocable<VkPhysicalDevice, const VkPhysicalDeviceProperties&> S>
const InputTuple& pick_physical_device(
	const std::span<const InputTuple> devices,
	S strategy
) {
	return std::ranges::max(
		devices,
		{},
		[&] (const auto& tup) {
			return strategy(std::get<0>(tup), std::get<1>(tup));
		});
}

constexpr auto PHYSICAL_DEVICES = std::to_array<std::tuple<VkPhysicalDevice, VkPhysicalDeviceProperties>>({
	{
		(VkPhysicalDevice)1,
		VkPhysicalDeviceProperties {
			.name = "NVIDIA Geforce GT 1030",
			.hardware_type = VkPhysicalDeviceType::DEDICATED_GPU,
			.dedicated_ram_bytes = 2 * 1024 * 1024,
			.shared_ram_bytes = 8 * 1024 * 1024
		}
	},
	{
		(VkPhysicalDevice)2,
		VkPhysicalDeviceProperties {
			.name = "NVIDIA Geforce RTX 4060",
			.hardware_type = VkPhysicalDeviceType::DEDICATED_GPU,
			.dedicated_ram_bytes = 8 * 1024 * 1024,
			.shared_ram_bytes = 16 * 1024 * 1024
		}
	},
	{
		(VkPhysicalDevice)3,
		VkPhysicalDeviceProperties {
			.name = "AMD Radeon 860M iGPU",
			.hardware_type = VkPhysicalDeviceType::INTEGRATED_GPU,
			.dedicated_ram_bytes = 0,
			.shared_ram_bytes = 16 * 1024 * 1024
		}
	},
});

int main() {
	const auto MAX_PERF_STRATEGY = [](VkPhysicalDevice device, const VkPhysicalDeviceProperties& props) -> int32_t {
		std::println("looking at: {}", props.name);
		int32_t score = 0;
		if (props.hardware_type == VkPhysicalDeviceType::DEDICATED_GPU)
			score += 100000;
		else score -= 100000;

		score += props.dedicated_ram_bytes / 1024;
		score += props.shared_ram_bytes / 2048;

		return score;
	};

	std::span<const std::tuple<VkPhysicalDevice, VkPhysicalDeviceProperties>> a = PHYSICAL_DEVICES;
	const auto& dev = pick_physical_device(PHYSICAL_DEVICES, MAX_PERF_STRATEGY);
	const VkPhysicalDeviceProperties& props = std::get<1>(dev);
	std::println("Best device: {}", props.name);
}