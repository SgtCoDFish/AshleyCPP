#include <memory>

#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IntervalSystem.hpp"

#include "gtest/gtest.h"

namespace {
float deltaTime = 0.1f;

class IntervalSystemSpy : public ashley::IntervalSystem {
public:
	int64_t numUpdates = 0;

	IntervalSystemSpy() :
			ashley::IntervalSystem(deltaTime * 2.0f, 0) {
	}

protected:
	void updateInterval() override {
		++numUpdates;
	}
};

class IntervalSystemTest : public ::testing::Test {
protected:
	ashley::Engine engine;
	IntervalSystemSpy *intervalSystemSpy = nullptr;

	IntervalSystemTest() {
		intervalSystemSpy = engine.addSystem<IntervalSystemSpy>();
	}
};
}

TEST_F(IntervalSystemTest, IntervalSystem) {
	for (int i = 1; i <= 10; ++i) {
		engine.update(deltaTime);
		ASSERT_EQ(i / 2, intervalSystemSpy->numUpdates);
	}
}
