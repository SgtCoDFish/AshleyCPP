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

#include <cstdint>

#include <memory>

#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentMapper.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/systems/IntervalIteratingSystem.hpp"

#include "gtest/gtest.h"

namespace {
constexpr static float deltaTime = 0.1f;

class IntervalComponentSpy : public ashley::Component {
public:
	int64_t numUpdates = 0;
};

class IntervalIteratingSystemSpy : public ashley::IntervalIteratingSystem {
private:
	const ashley::ComponentMapper<IntervalComponentSpy> im;

public:
	IntervalIteratingSystemSpy() :
			        ashley::IntervalIteratingSystem(ashley::Family::getFor( { typeid(IntervalComponentSpy) }),
			                deltaTime * 2.0f, 0),
			        im(ashley::ComponentMapper<IntervalComponentSpy>::getMapper()) {
	}

protected:
	void processEntity(ashley::Entity * const entity) override final {
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
	auto entities = engine.getEntitiesFor(ashley::Family::getFor( { typeid(IntervalComponentSpy) }));
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
