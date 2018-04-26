#include <cstdint>

#include <memory>

#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentMapper.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IntervalIteratingSystem.hpp"

#include "gtest/gtest.h"

namespace {
constexpr float deltaTime = 0.1f;

class IntervalComponentSpy : public ashley::Component {
public:
	int64_t numUpdates = 0;
};

class IntervalIteratingSystemSpy : public ashley::IntervalIteratingSystem {
private:
	const ashley::ComponentMapper<IntervalComponentSpy> im;

public:
	IntervalIteratingSystemSpy() :
			ashley::IntervalIteratingSystem(ashley::Family::getFor({typeid(IntervalComponentSpy)}),
											deltaTime * 2.0f, 0),
			im(ashley::ComponentMapper<IntervalComponentSpy>::getMapper()) {
	}

protected:
	void processEntity(ashley::Entity *entity) override final {
		im.get(entity)->numUpdates++;
	}
};

class IntervalIteratingSystemTest : public ::testing::Test {
protected:
	ashley::Engine engine;

	std::shared_ptr<IntervalIteratingSystemSpy> intervalIteratingSystemSpy = nullptr;

	IntervalIteratingSystemTest() {
		engine.addSystem<IntervalIteratingSystemSpy>();
	}
};

}

TEST_F(IntervalIteratingSystemTest, IntervalIteratingSystem) {
	auto entities = engine.getEntitiesFor(ashley::Family::getFor({typeid(IntervalComponentSpy)}));
	auto im = ashley::ComponentMapper<IntervalComponentSpy>::getMapper();

	for (int i = 0; i < 10; ++i) {
		auto entity = engine.addEntity();
		entity->add<IntervalComponentSpy>();
	}

	for (int i = 1; i <= 10; ++i) {
		engine.update(deltaTime);

		for (unsigned int j = 0; j < entities->size(); ++j) {
			ASSERT_EQ(i / 2, im.get(entities->at(j))->numUpdates);
		}
	}
}
