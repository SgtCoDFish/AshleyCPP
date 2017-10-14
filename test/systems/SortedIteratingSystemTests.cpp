/*******************************************************************************
 * Copyright 2017 See AUTHORS file.
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

#include <string>
#include <list>
#include <utility>

#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/core/ComponentMapper.hpp"

#include "Ashley/systems/SortedIteratingSystem.hpp"

#include "AshleyTestCommon.hpp"

#include "gtest/gtest.h"

using ashley::Engine;
using ashley::Entity;
using ashley::Component;
using ashley::Family;
using ashley::ComponentMapper;
using ashley::SortedIteratingSystem;

class ComponentB : public Component {

};

class ComponentC : public Component {

};

class OrderedComponent : public ashley::Component {
public:
	std::string name { "" };
	int zLayer = 0;

	OrderedComponent() {
	}

	OrderedComponent(std::string &&name, int zLayer) :
		name { std::move(name) },
		zLayer { zLayer } {
	}
};

bool zComparator(Entity *e1, Entity *e2) {
	const ComponentMapper<OrderedComponent> zMapper = ComponentMapper<OrderedComponent>::getMapper();
	const auto o1 = zMapper.get(e1);
	const auto o2 = zMapper.get(e2);
	return o1->zLayer < o2->zLayer;
}

class SortedIteratingSystemMock final : public SortedIteratingSystem {
public:
	std::list<std::string> expectedNames;
	const ComponentMapper<OrderedComponent> zMapper = ComponentMapper<OrderedComponent>::getMapper();
	uint64_t numUpdates = 0;

	explicit SortedIteratingSystemMock(Family *family) :
		SortedIteratingSystem(family, zComparator, 0) {
	}

	void update(float deltaTime) override final {
		SortedIteratingSystem::update(deltaTime);
		ASSERT_TRUE(expectedNames.empty());
	}

	void processEntity(Entity * const entity, float deltaTime) override final {
		++numUpdates;
		const auto z = zMapper.get(entity);

		ASSERT_NE(z, nullptr);
		ASSERT_FALSE(expectedNames.empty());
		ASSERT_EQ(expectedNames.front(), z->name);
		expectedNames.pop_front();
	}
};

namespace {
class SortedIteratingSystemTest : public ::testing::Test {
protected:
	const ComponentMapper<OrderedComponent> zMapper = ComponentMapper<OrderedComponent>::getMapper();
	Engine engine;

	Family *sortFamily { Family::getFor( { typeid(OrderedComponent) }) };

	const float delta = 1.0f;
};

TEST_F(SortedIteratingSystemTest, ShouldIterateEntitiesWithCorrectFamily) {
	const auto family = Family::getFor( { typeid(OrderedComponent), typeid(ComponentB) });
	auto mockSystem = engine.addSystem<SortedIteratingSystemMock>(family);
	auto e = engine.addEntity();

	e->add<OrderedComponent>("A", 0);
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 0);

	e->add<ComponentB>();
	mockSystem->expectedNames.emplace_back("A");
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 1);

	e->add<ComponentC>();
	mockSystem->expectedNames.emplace_back("A");
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2);

	e->remove<OrderedComponent>();
	e->add<ComponentC>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2);
}

TEST_F(SortedIteratingSystemTest, EntityOrdering) {
	auto mockSystem = engine.addSystem<SortedIteratingSystemMock>(sortFamily);

	auto a_t = std::unique_ptr<Entity>(new Entity());
	a_t->add<OrderedComponent>("A", 0);

	auto b_t = std::unique_ptr<Entity>(new Entity());
	b_t->add<OrderedComponent>("B", 1);

	auto c_t = std::unique_ptr<Entity>(new Entity());
	c_t->add<OrderedComponent>("C", 3);

	auto d_t = std::unique_ptr<Entity>(new Entity());
	d_t->add<OrderedComponent>("D", 2);

	auto a = engine.addEntity(std::move(a_t));
	auto b = engine.addEntity(std::move(b_t));
	auto c = engine.addEntity(std::move(c_t));

	{
		SCOPED_TRACE("A, B & C");

		mockSystem->expectedNames.emplace_back("A");
		mockSystem->expectedNames.emplace_back("B");
		mockSystem->expectedNames.emplace_back("C");
		engine.update(0.0f);
	}

	auto d = engine.addEntity(std::move(d_t));

	{
		SCOPED_TRACE("A, B, D & C");
		mockSystem->expectedNames.emplace_back("A");
		mockSystem->expectedNames.emplace_back("B");
		mockSystem->expectedNames.emplace_back("D");
		mockSystem->expectedNames.emplace_back("C");
		engine.update(0.0f);
	}

	zMapper.get(a)->zLayer = 3;
	zMapper.get(b)->zLayer = 2;
	zMapper.get(c)->zLayer = 1;
	zMapper.get(d)->zLayer = 0;
	mockSystem->forceSort();

	{
		SCOPED_TRACE("D, C, B, A");
		mockSystem->expectedNames.emplace_back("D");
		mockSystem->expectedNames.emplace_back("C");
		mockSystem->expectedNames.emplace_back("B");
		mockSystem->expectedNames.emplace_back("A");
		engine.update(0.0f);
	}

}

}
