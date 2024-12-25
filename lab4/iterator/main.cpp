#include <optional>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <vector>
#include <print>

template<typename T>
struct Iterator {
private:
	T* start_;
	T* current_;
	T* end_;

public:
	template<std::ranges::range R>
	constexpr Iterator(R&& range)
		: start_(&*range.begin())
		, current_(start_)
		, end_(start_ + range.size())
	{}

	std::optional<T*> next(this Iterator& self) {
		if (self.current_ < self.end_) {
			return self.current_++;
		}

		return std::nullopt;
	}
};

int main() {
	std::vector<uint32_t> vec = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
	Iterator<uint32_t> it(vec);

	//while (uint32_t* v = it.next().value()) {
	std::optional<uint32_t*> opt;
	while ((opt = it.next(), opt.has_value())) {
		std::println("{}", *opt.value());
	}
}