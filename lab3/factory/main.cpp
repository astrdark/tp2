#include <cstdint>
#include <optional>
#include <print>
#include <span>
#include <type_traits>

template<typename T>
struct Box {
private:
	T* ptr_;

public:
	explicit Box(T* ptr) noexcept : ptr_(ptr) {}

	Box(const Box&) = delete;
	Box& operator=(const Box&) = delete;

	Box(Box&& rhs) noexcept {
		ptr_ = rhs.ptr_;
		rhs.ptr_ = nullptr;
	}

	Box& operator=(Box&& rhs) noexcept {
		if (&rhs == this) {
			return *this;
		}

		this->ptr_ = std::exchange(rhs.ptr_, nullptr);

		return *this;
	};

	const T* operator->() const {
		return ptr_;
	}

	T* operator->() {
		return ptr_;
	}

	T* forget(this Box& self) {
		return std::exchange(self.ptr_, nullptr);
	}
};

template<typename T>
struct Impl {
private:
	Box<T> data_;

public:
	template<std::derived_from<T> U>
	explicit Impl(Box<U>&& data)
		: data_(Box(std::bit_cast<T*>(data.forget())))
	{}

	T* operator->() {
		return data_.operator->();
	}

	const T* operator->() const {
		return data_.operator->();
	}
};

/**** FACTORY METHOD ****/
struct ILogger {
	virtual void log(std::string_view message) = 0;
	virtual ~ILogger() = default;
};

struct FileLogger : ILogger {
	FileLogger() = default;
	~FileLogger() = default;

	void log(std::string_view message) override {
		std::println("[FileLogger] {}", message);
	}
};

struct ConsoleLogger : ILogger {
	ConsoleLogger() = default;
	~ConsoleLogger() = default;

	void log(std::string_view message) override {
		std::println("[ConsoleLogger] {}", message);
	}
};

struct ILoggerFactory {
	virtual Impl<ILogger> create_logger() = 0;
	virtual ~ILoggerFactory() = default;
};

struct FileLoggerFactory : ILoggerFactory {
	FileLoggerFactory() = default;
	~FileLoggerFactory() = default;

	Impl<ILogger> create_logger() override {
		return Impl<ILogger>(Box(new FileLogger));
	}

	static Impl<ILoggerFactory> create() {
		return Impl<ILoggerFactory>(Box(new FileLoggerFactory));
	}
};

struct ConsoleLoggerFactory : ILoggerFactory {
	ConsoleLoggerFactory() = default;
	~ConsoleLoggerFactory() = default;

	Impl<ILogger> create_logger() override {
		return Impl<ILogger>(Box(new ConsoleLogger));
	}

	static Impl<ILoggerFactory> create() {
		return Impl<ILoggerFactory>(Box(new ConsoleLoggerFactory));
	}
};

/**** ABSTRACT FACTORY ****/
struct IActor {
	virtual void tick() = 0;
	virtual ~IActor() = default;
};

struct Game1Character : IActor {
	Game1Character() = default;
	~Game1Character() = default;

	void tick() override {
		std::println("[Game1Character tick]");
	}
};
struct Game1Enemy : IActor {
	Game1Enemy() = default;
	~Game1Enemy() = default;

	void tick() override {
		std::println("[Game1Enemy tick]");
	}
};

struct Game2Character : IActor {
	Game2Character() = default;
	~Game2Character() = default;

	void tick() override {
		std::println("[Game2Character tick]");
	}
};
struct Game2Enemy : IActor {
	Game2Enemy() = default;
	~Game2Enemy() = default;

	void tick() override {
		std::println("[Game2Enemy tick]");
	}
};

struct IGameFactory {
	virtual Impl<IActor> create_player() = 0;
	virtual Impl<IActor> create_enemy() = 0;
	virtual ~IGameFactory() = default;
};

struct Game1Factory : IGameFactory {
	Game1Factory() = default;
	~Game1Factory() = default;

	Impl<IActor> create_player() override {
		return Impl<IActor>(Box(new Game1Character));
	}

	Impl<IActor> create_enemy() override {
		return Impl<IActor>(Box(new Game1Enemy));
	}

	static Impl<IGameFactory> create() {
		return Impl<IGameFactory>(Box(new Game1Factory));
	}
};

struct Game2Factory : IGameFactory {
	Game2Factory() = default;
	~Game2Factory() = default;

	Impl<IActor> create_player() override {
		return Impl<IActor>(Box(new Game2Character));
	}

	Impl<IActor> create_enemy() override {
		return Impl<IActor>(Box(new Game2Enemy));
	}

	static Impl<IGameFactory> create() {
		return Impl<IGameFactory>(Box(new Game2Factory));
	}
};

int main() {
	// Factory method
	Impl<ILoggerFactory> file_logger_factory = FileLoggerFactory::create();
	Impl<ILogger> file_logger = file_logger_factory->create_logger();
	file_logger->log("Lorem ipsum");

	Impl<ILoggerFactory> console_logger_factory = ConsoleLoggerFactory::create();
	Impl<ILogger> console_logger = console_logger_factory->create_logger();
	console_logger->log("... dolor sit amet.");

	// Abstract factory
	Impl<IGameFactory> game_factory = ([] {
		std::srand(time(nullptr));
		if (std::rand() % 2 == 0) {
			return Game1Factory::create();
		} else return Game2Factory::create();
	})();

	Impl<IActor> player = game_factory->create_player();
	Impl<IActor> enemy = game_factory->create_enemy();

	std::span actors = { &enemy, 2 };

	do {
		for (Impl<IActor>& actor : actors) {
			actor->tick();
		}
	} while (false);
}
