#include <cstdint>
#include <optional>
#include <print>
#include <span>
#include <type_traits>

template<typename T>
struct SingletonStorage {
private:
	union {
		alignas(T) T data_;
		alignas(T) uint8_t __pad[sizeof(T)];
	};
	bool _initialized = false;

public:
	SingletonStorage() = default;

	SingletonStorage(const SingletonStorage&) = delete;
	SingletonStorage(SingletonStorage&&) = delete;

	bool is_initialized(this const SingletonStorage& self) {
		return self._initialized;
	}

	template<typename... Ty>
	void construct(this SingletonStorage& self, Ty&&... args) {
		self._initialized = true;
		new (&self.data_) T(std::forward<Ty>(args)...);
	}

	template<typename Self>
	T& ref(this Self& self) {
		return self.data_;
	}

	~SingletonStorage() {
		if (_initialized) {
			(&data_)->~T();
		}
		_initialized = false;
	}
};

template<class T>
struct Singleton {
private:
	static inline SingletonStorage<T> _singleton_storage;
public:
	static T& instance() {
		if (!_singleton_storage.is_initialized()) {
			_singleton_storage.construct();
		}

		return _singleton_storage.ref();
	}
};

struct EventLoop : Singleton<EventLoop> {
	EventLoop() = default;

	void foo(this EventLoop& self) {
		std::println("[Singleton] EventLoop is at {}", std::bit_cast<void*>(&self));
	}
};

int main() {
	EventLoop::instance().foo();
}
