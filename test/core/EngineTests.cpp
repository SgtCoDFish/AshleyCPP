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

#include <vector>
#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <memory>

#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/EntityListener.hpp"
#include "Ashley/core/EntitySystem.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/core/Family.hpp"

#include "gtest/gtest.h"

using ashley::Engine;
using ashley::Entity;
using ashley::EntityListener;
using ashley::EntitySystem;
using ashley::Component;
using ashley::ComponentType;
using ashley::Family;

namespace {
class ComponentA : public Component {
};
class ComponentB : public Component {
};
class ComponentC : public Component {
};

class EntityListenerMock : public EntityListener {
public:
	uint64_t addedCount = 0;
	uint64_t removedCount = 0;

	virtual void entityAdded(Entity &entity) override {
		++addedCount;
	}

	virtual void entityRemoved(Entity &entity) override {
		++removedCount;
	}
};

class EntitySystemMock : public EntitySystem {
private:
	std::shared_ptr<std::vector<uint64_t>> updates;
	bool active = { true };

public:
	uint64_t updateCalls = { 0 };
	uint64_t addedCalls = { 0 };
	uint64_t removedCalls = { 0 };

	EntitySystemMock() :
			EntitySystemMock(nullptr) {
	}

	EntitySystemMock(std::shared_ptr<std::vector<uint64_t>> nUpdates) :
			EntitySystem(EntitySystem::DEFAULT_PRIORITY), updates(nUpdates) {
	}

	void update(float deltaTime) override {
		++updateCalls;

		if (updates != nullptr)
			updates->push_back(priority);
	}

	void addedToEngine(Engine &engine) override {
		++addedCalls;
	}

	void removedFromEngine(Engine &engine) override {
		++removedCalls;
	}

	bool checkProcessing() override {
		return active;
	}

	void setActive(bool active) {
		this->active = active;
	}
};

class EntitySystemMockA : public EntitySystemMock {
public:
	EntitySystemMockA() :
			EntitySystemMock() {
	}

	EntitySystemMockA(std::shared_ptr<std::vector<uint64_t>> ptr) :
			EntitySystemMock(ptr) {
	}
};

class EntitySystemMockB : public EntitySystemMock {
public:
	EntitySystemMockB() :
			EntitySystemMock() {
	}

	EntitySystemMockB(std::shared_ptr<std::vector<uint64_t>> ptr) :
			EntitySystemMock(ptr) {
	}
};

class EngineTest : public ::testing::Test {
protected:
	const float deltaTime = 0.16f;

	Engine engine;

	EntityListenerMock listenerA;
	EntityListenerMock listenerB;

	EntitySystemMockA systemA;
	EntitySystemMockB systemB;

	EngineTest() {
		engine.addEntityListener(&listenerA);
		engine.addEntityListener(&listenerB);
	}
};
}

// Ensure that listeners work with adding and removing all entities.
TEST_F(EngineTest, AddAndRemoveEntities) {
	auto e1 = std::make_shared<Entity>();
	engine.addEntity(e1);

	ASSERT_EQ(1, listenerA.addedCount);
	ASSERT_EQ(1, listenerB.addedCount);

	engine.removeEntityListener(&listenerB);

	auto e2 = std::make_shared<Entity>();
	engine.addEntity(e2);

	ASSERT_EQ(2, listenerA.addedCount);
	ASSERT_EQ(1, listenerB.addedCount);

	engine.addEntityListener(&listenerB);

	engine.removeAllEntities(); // calls removeEntity(shared_ptr) on each added entity

	ASSERT_EQ(2, listenerA.removedCount);
	ASSERT_EQ(2, listenerB.removedCount);
//	ASSERT_EQ(e1.use_count(), 1);
//	ASSERT_EQ(e2.use_count(), 1);
}

// Test the addSystem(EntitySystem*) getSystem() and removeSystem(typeID) methods
TEST_F(EngineTest, AddGetAndRemoveSystem) {
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);

	engine.addSystem(std::make_shared<EntitySystemMockA>());
	engine.addSystem(std::make_shared<EntitySystemMockB>());

	auto sA = engine.getSystem<EntitySystemMockA>();
	auto sB = engine.getSystem<EntitySystemMockB>();

	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);
	ASSERT_EQ(1, sA->addedCalls);
	ASSERT_EQ(1, sB->addedCalls);

	engine.removeSystem(typeid(EntitySystemMockA));
	engine.removeSystem(typeid(EntitySystemMockB));

	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);
	ASSERT_EQ(1, sA->removedCalls);
	ASSERT_EQ(1, sB->removedCalls);
}

// Test the add and removeSystem methods with shared_ptr input and the getSystem(type_index) method.
TEST_F(EngineTest, AddAndRemoveSystemShared) {
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);

	auto aptr = std::make_shared<EntitySystemMockA>(systemA);
	auto bptr = std::make_shared<EntitySystemMockB>(systemB);

	engine.addSystem(aptr);
	engine.addSystem(bptr);

	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);
	ASSERT_EQ(1, aptr->addedCalls);
	ASSERT_EQ(1, bptr->addedCalls);

	engine.removeSystem(aptr);
	engine.removeSystem(bptr);

	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);
	ASSERT_EQ(1, aptr->removedCalls);
	ASSERT_EQ(1, bptr->removedCalls);
}

TEST_F(EngineTest, SystemUpdate) {
	auto aptr = std::make_shared<EntitySystemMockA>();
	auto bptr = std::make_shared<EntitySystemMockB>();
	engine.addSystem(aptr);
	engine.addSystem(bptr);

	const int numUpdates = 10;

	for (int i = 0; i < numUpdates; i++) {
		ASSERT_EQ(i, aptr->updateCalls);
		ASSERT_EQ(i, bptr->updateCalls);

		engine.update(deltaTime);

		ASSERT_EQ(i + 1, aptr->updateCalls);
		ASSERT_EQ(i + 1, bptr->updateCalls);
	}

	engine.removeSystem(bptr);

	for (int i = 0; i < numUpdates; i++) {
		ASSERT_EQ(i + numUpdates, aptr->updateCalls);
		ASSERT_EQ(numUpdates, bptr->updateCalls);

		engine.update(deltaTime);

		ASSERT_EQ(i + 1 + numUpdates, aptr->updateCalls);
		ASSERT_EQ(numUpdates, bptr->updateCalls);
	}
}

TEST_F(EngineTest, SystemUpdateOrder) {
	auto updates = std::make_shared<std::vector<uint64_t>>();

	EntitySystemMockA system1(updates);
	EntitySystemMockB system2(updates);

	system1.priority = 2;
	system2.priority = 1;

	auto aptr = std::make_shared<EntitySystemMockA>(system1);
	auto bptr = std::make_shared<EntitySystemMockB>(system2);

	engine.addSystem(aptr);
	engine.addSystem(bptr);

	engine.update(deltaTime);

	uint64_t previous = 0;

	for (auto it = updates->begin(); it != updates->end(); it++) {
		ASSERT_TRUE((*it) >= previous);
		previous = (*it);
	}
}

TEST_F(EngineTest, IgnoreSystem) {
	auto aptr = std::make_shared<EntitySystemMockA>(systemA);

	engine.addSystem(aptr);
	const int numUpdates = 10;

	for (int i = 0; i < numUpdates; i++) {
		aptr->setActive(i % 2 == 0); // set to active if i is even

		engine.update(deltaTime);

		ASSERT_EQ(aptr->updateCalls, i / 2 + 1);
	}
}

TEST_F(EngineTest, EntitiesForFamily) {
	auto family = Family::getFor( { typeid(ComponentA), typeid(ComponentB) });
	auto familyEntities = engine.getEntitiesFor(*family);

	ASSERT_EQ(0, familyEntities->size());

	auto e1 = std::make_shared<Entity>();
	auto e2 = std::make_shared<Entity>();
	auto e3 = std::make_shared<Entity>();
	auto e4 = std::make_shared<Entity>();

	e1->add<ComponentA>().add<ComponentB>();
	e2->add<ComponentA>().add<ComponentC>();
	e3->add<ComponentA>().add<ComponentB>().add<ComponentC>();
	e4->add<ComponentA>().add<ComponentB>().add<ComponentC>();

	engine.addEntity(e1);
	engine.addEntity(e2);
	engine.addEntity(e3);
	engine.addEntity(e4);

	ASSERT_EQ(3, familyEntities->size());

	bool e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(),
			[&](std::shared_ptr<ashley::Entity> found) {
				if(found == e1) e1Found = true;
				else if(found == e2) e2Found = true;
				else if(found == e3) e3Found = true;
				else if(found == e4) e4Found = true;});

	ASSERT_EQ(e1Found, true);
	ASSERT_EQ(e2Found, false);
	ASSERT_EQ(e3Found, true);
	ASSERT_EQ(e4Found, true);
}

TEST_F(EngineTest, EntityForFamilyWithRemoval) {
	auto e = std::make_shared<Entity>();

	e->add<ComponentA>();

	engine.addEntity(e);
	auto entities = engine.getEntitiesFor(Family::getFor( { typeid(ComponentA) }));

	ASSERT_EQ(entities->size(), 1);
	ASSERT_TRUE(
			std::find_if(entities->begin(), entities->end(),
					[&](std::shared_ptr<Entity> &found) {return found == e;})
					!= entities->end());

	engine.removeEntity(e);

	ASSERT_EQ(0, entities->size());
	ASSERT_FALSE(
			std::find_if(entities->begin(), entities->end(),
					[&](std::shared_ptr<Entity> found) {return found == e;})
					!= entities->end());
}

TEST_F(EngineTest, EntitiesForFamilyWithRemoval) {
	auto family = Family::getFor( { typeid(ComponentA), typeid(ComponentB) });
	auto familyEntities = engine.getEntitiesFor(*family);

	ASSERT_EQ(0, familyEntities->size());

	auto e1 = std::make_shared<Entity>();
	auto e2 = std::make_shared<Entity>();
	auto e3 = std::make_shared<Entity>();
	auto e4 = std::make_shared<Entity>();

	e1->add<ComponentA>().add<ComponentB>();
	e2->add<ComponentA>().add<ComponentC>();
	e3->add<ComponentA>().add<ComponentB>().add<ComponentC>();
	e4->add<ComponentA>().add<ComponentB>().add<ComponentC>();

	engine.addEntity(e1);
	engine.addEntity(e2);
	engine.addEntity(e3);
	engine.addEntity(e4);

	ASSERT_EQ(3, familyEntities->size());

	bool e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(),
			[&](std::shared_ptr<ashley::Entity> &found) {
				if(found == e1) e1Found = true;
				else if(found == e2) e2Found = true;
				else if(found == e3) e3Found = true;
				else if(found == e4) e4Found = true;});

	ASSERT_EQ(e1Found, true);
	ASSERT_EQ(e2Found, false);
	ASSERT_EQ(e3Found, true);
	ASSERT_EQ(e4Found, true);

	e1->remove<ComponentA>();
	engine.removeEntity(e3);

	e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(),
			[&](std::shared_ptr<ashley::Entity> &found) {
				if(found == e1) e1Found = true;
				else if(found == e2) e2Found = true;
				else if(found == e3) e3Found = true;
				else if(found == e4) e4Found = true;});

	EXPECT_EQ(e1Found, false);
	EXPECT_EQ(e2Found, false);
	EXPECT_EQ(e3Found, false);
	EXPECT_EQ(e4Found, true);
	ASSERT_EQ(1, familyEntities->size());
}

TEST_F(EngineTest, EntitiesForFamilyWithRemovalAndFiltering) {
	auto entsWithAOnly = engine.getEntitiesFor(Family::getFor(ComponentType::getBitsFor( {
			typeid(ComponentA) }), // must have A
	ashley::BitsType(), // ignore
			ComponentType::getBitsFor( { typeid(ComponentB) }))); // must not have B

	auto entsWithB = engine.getEntitiesFor(Family::getFor({typeid(ComponentB)}));

	auto e1 = std::make_shared<Entity>();
	auto e2 = std::make_shared<Entity>();

	engine.addEntity(e1);
	engine.addEntity(e2);

	e1->add<ComponentA>();

	e2->add<ComponentA>();
	e2->add<ComponentB>();

	ASSERT_EQ(1, entsWithAOnly->size());
	ASSERT_EQ(1, entsWithB->size());

	e2->remove<ComponentB>();

	ASSERT_EQ(2, entsWithAOnly->size());
	ASSERT_EQ(0, entsWithB->size());
}
