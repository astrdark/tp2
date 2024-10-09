#include <iostream>

namespace detail
{
	int gen_id();
	void impl();
}

struct GameObject;
struct Unit;

// -----------------------------------------------------------

struct GameObject {
private:
	int _id;

protected:
	std::string _name;
	int _x, _y;

public:
	GameObject(std::string&& name, int x, int y)
		: _id(detail::gen_id())
		, _name(name)
		, _x(x), _y(y)
	{}

	int id(this const GameObject& self) {
		return self._id;
	}

	std::string_view name(this const GameObject& self) {
		return self._name;
	}

	int x(this const GameObject& self) {
		return self._x;
	}

	int y(this const GameObject& self) {
		return self._y;
	}
};

struct Movable {
	virtual void move(int dx, int dy) = 0;
	virtual ~Movable() = default;
};

struct Attacker {
	virtual void attack(Unit& unit) = 0;
	virtual ~Attacker() = default;
};

struct Unit : GameObject {
protected:
	float _hp;

public:
	Unit(float hp, std::string&& name, int x, int y)
		: GameObject(std::forward<std::string>(name), x, y)
		, _hp(hp)
	{}

	float hp(this const Unit& self)	{
		return self._hp;
	}

	bool alive(this const Unit& self) {
		return self.hp() > 0.f;
	}

	void receive_damage(this Unit& self, float damage) {
		self._hp -= damage;
	}
};

struct Archer : Unit, Movable, Attacker {
	Archer(float hp, std::string&& name, int x, int y)
		: Unit(hp, std::forward<std::string>(name), x, y)
	{}

	void move(int dx, int dy) override {
		this->_x += dx;
		this->_y += dy;
	}

	void attack(Unit& unit) override {
		unit.receive_damage(5.f);
	}
};

struct Building : GameObject {
protected:
	bool _built = false;

public:
	Building(std::string&& name, int x, int y)
		: GameObject(std::forward<std::string>(name), x, y)
	{}

	bool built(this const Building& self) {
		return self._built;
	}
};

struct MobileHouse : Building, Movable {
	MobileHouse(std::string&& name, int x, int y)
		: Building(std::forward<std::string>(name), x, y)
	{}

	void move(int dx, int dy) override {
		this->_x += dx;
		this->_y += dy;
	}
};

struct Fort : Building, Movable, Attacker {
	Fort(std::string&& name, int x, int y)
		: Building(std::forward<std::string>(name), x, y)
	{}

	void move(int dx, int dy) override {
		this->_x += dx;
		this->_y += dy;
	}

	void attack(Unit& unit) override {
		unit.receive_damage(10.f);
	}
};

int main() {}
