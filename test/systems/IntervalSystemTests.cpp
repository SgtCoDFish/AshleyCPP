/*******************************************************************************
 * Copyright 2014, 2015 See AUTHORS file.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include <memory>

#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IntervalSystem.hpp"

#include "gtest/gtest.h"

namespace {
static float deltaTime = 0.1f;

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
