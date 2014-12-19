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
#include "Ashley/core/ComponentMapper.hpp"
#include "Ashley/systems/IteratingSystem.hpp"
#include "AshleyTestCommon.hpp"

#include "gtest/gtest.h"

using ashley::test::PositionComponent;
using ashley::test::VelocityComponent;

using ashley::Entity;
using ashley::Engine;
using ashley::Family;
using ashley::Component;
using ashley::ComponentMapper;
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
	constexpr static float delta = 0.15f;

	Engine engine;

	Family *family = nullptr;
	Entity *e = nullptr;

	IteratingSystemTest() :
			family(Family::getFor( { typeid(ComponentA), typeid(ComponentB) })), e { nullptr } {
	}
};

class IteratingSystemMock : public IteratingSystem {
public:
	IteratingSystemMock(Family *family) :
			IteratingSystem(family), numUpdates(0) {
	}
	virtual ~IteratingSystemMock() {
	}

	uint64_t numUpdates;

	void processEntity(Entity * const &entity, float deltaTime) override {
		++numUpdates;
	}
};

class SpyComponent : public ashley::Component {
public:
	int64_t updates = 0;
};

class IndexComponent : public ashley::Component {
public:
	IndexComponent(int nindex = 0) :
			index(nindex) {
	}

	int64_t index = 0;
};

class IteratingComponentRemovalMock : public IteratingSystem {
private:
	ComponentMapper<SpyComponent> sm;
	ComponentMapper<IndexComponent> im;

public:
	IteratingComponentRemovalMock(int priority = 0) :
			IteratingSystem(
					ashley::Family::getFor( { typeid(SpyComponent), typeid(IndexComponent) }),
					priority), sm(ComponentMapper<SpyComponent>::getMapper()), im(
					ComponentMapper<IndexComponent>::getMapper()) {
	}

	void processEntity(Entity * const &entity, float deltaTime) override {
		auto index = im.get(entity)->index;

		if (index % 2 == 0) {
			entity->remove<SpyComponent>();
			entity->remove<IndexComponent>();
		} else {
			sm.get(entity)->updates++;
		}
	}
};

class IteratingRemovalMock : public IteratingSystem {
private:
	Engine *engine = nullptr;

	ComponentMapper<SpyComponent> sm;
	ComponentMapper<IndexComponent> im;

public:
	IteratingRemovalMock(int priority = 0) :
			IteratingSystem(
					ashley::Family::getFor( { typeid(SpyComponent), typeid(IndexComponent) }),
					priority), sm(ComponentMapper<SpyComponent>::getMapper()), im(
					ComponentMapper<IndexComponent>::getMapper()) {
	}

	void addedToEngine(Engine &engine) override {
		IteratingSystem::addedToEngine(engine);
		this->engine = &engine;
	}

	void removedFromEngine(Engine &engine) override {
		IteratingSystem::removedFromEngine(engine);
		this->engine = nullptr;
	}

	void processEntity(Entity * const &entity, float deltaTime) override {
		auto index = im.get(entity)->index;

		if (index % 2 == 0) {
			engine->removeEntity(entity);
		} else {
			sm.get(entity)->updates++;
		}
	}
};

}

TEST_F(IteratingSystemTest, ShouldIterateEntitiesWithCorrectFamily) {
	auto mockSystem = engine.addSystem<IteratingSystemMock>(family);
	auto e = engine.addEntity();

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

TEST_F(IteratingSystemTest, EntityRemovalWhileIterating) {
	// only uses the engine from IteratingSystemTest

	auto entities = engine.getEntitiesFor(Family::getFor( { typeid(SpyComponent),
			typeid(IndexComponent) }));
	auto sm = ComponentMapper<SpyComponent>::getMapper();

	engine.addSystem<IteratingRemovalMock>(10);

	constexpr int numEntities = 10;

	for (int i = 0; i < numEntities; i++) {
		auto e = std::unique_ptr<Entity>(new Entity());

		e->add<SpyComponent>();
		e->add<IndexComponent>(i + 1);

		engine.addEntity(std::move(e));
	}

	engine.update(delta);

	ASSERT_EQ(numEntities / 2, entities->size());

	for (unsigned int i = 0; i < entities->size(); i++) {
		auto e = entities->at(i);

		ASSERT_EQ(1, sm.get(e)->updates);
	}
}

TEST_F(IteratingSystemTest, ComponentRemovalWhileIterating) {
	// only uses the engine from IteratingSystemTest

	auto entities = engine.getEntitiesFor(Family::getFor( { typeid(SpyComponent),
			typeid(IndexComponent) }));
	auto sm = ComponentMapper<SpyComponent>::getMapper();

	engine.addSystem<IteratingComponentRemovalMock>(10);

	constexpr int numEntities = 10;

	for (int i = 0; i < numEntities; i++) {
		auto e = std::unique_ptr<Entity>(new Entity());

		e->add<SpyComponent>();
		e->add<IndexComponent>(i + 1);

		engine.addEntity(std::move(e));
	}

	engine.update(delta);

	ASSERT_EQ(numEntities / 2, entities->size());

	for (unsigned int i = 0; i < entities->size(); i++) {
		auto e = entities->at(i);

		ASSERT_EQ(1, sm.get(e)->updates);
	}
}
