/*******************************************************************************
 * Copyright 2014 See AUTHORS file.
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
#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/systems/IteratingSystem.hpp"
#include "AshleyTestCommon.hpp"

#include "gtest/gtest.h"

using ashley::test::PositionComponent;
using ashley::test::VelocityComponent;

using ashley::Entity;
using ashley::Engine;
using ashley::Family;
using ashley::Component;
using ashley::IteratingSystem;

class ComponentA : public Component {

};

class ComponentB : public Component {

};

class ComponentC : public Component {

};

namespace {
class IteratingSystemTest : public ::testing::Test {
protected:
	Engine engine;

	std::shared_ptr<Family> family;
	std::shared_ptr<Entity> e;

	IteratingSystemTest() :
			family(Family::getFor( { typeid(ComponentA), typeid(ComponentB) })), e(
					std::make_shared<Entity>()) {
	}
};

class IteratingSystemMock : public IteratingSystem {
public:
	IteratingSystemMock(std::shared_ptr<ashley::Family> &family) :
			IteratingSystem(family), numUpdates(0) {
	}
	virtual ~IteratingSystemMock() {
	}

	uint64_t numUpdates;

	void processEntity(std::shared_ptr<ashley::Entity> &entity, float deltaTime) override {
		++numUpdates;
	}
};

}

TEST_F(IteratingSystemTest, ShouldIterateEntitiesWithCorrectFamily) {
	const float delta = 0.15f;

	auto mockSystem = std::make_shared<IteratingSystemMock>(family);

	engine.addSystem(mockSystem);
	engine.addEntity(e);

	e->add<ComponentA>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 0);

	e->add<ComponentB>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 1);

	e->add<ComponentC>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2);

	e->remove<ComponentA>();
	e->add<ComponentC>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2);
}
