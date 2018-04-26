#include <cstdint>

#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>

#include "Ashley/core/Engine.hpp"

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

class EntityListenerMock final : public EntityListener {
public:
	uint64_t addedCount = 0;
	uint64_t removedCount = 0;

	void entityAdded(Entity &entity) override {
		++addedCount;
	}

	void entityRemoved(Entity &entity) override {
		++removedCount;
	}
};

class EntitySystemMock : public EntitySystem {
public:
	int64_t updateCalls = {0};
	int64_t addedCalls = {0};
	int64_t removedCalls = {0};

	EntitySystemMock() :
			EntitySystemMock(nullptr) {
	}

	explicit EntitySystemMock(std::shared_ptr<std::vector<uint64_t>> nUpdates) :
			EntitySystem(0),
			updates(std::move(nUpdates)) {
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

private:
	std::shared_ptr<std::vector<uint64_t>> updates;
};

class EntitySystemMockA final : public EntitySystemMock {
public:
	EntitySystemMockA() = default;

	explicit EntitySystemMockA(std::shared_ptr<std::vector<uint64_t>> ptr) :
			EntitySystemMock(ptr) {
	}
};

class EntitySystemMockB final : public EntitySystemMock {
public:
	EntitySystemMockB() = default;

	explicit EntitySystemMockB(std::shared_ptr<std::vector<uint64_t>> ptr) :
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
	engine.addEntity();

	ASSERT_EQ(1u, listenerA.addedCount);
	ASSERT_EQ(1u, listenerB.addedCount);

	engine.removeEntityListener(&listenerB);

	engine.addEntity();

	ASSERT_EQ(2u, listenerA.addedCount);
	ASSERT_EQ(1u, listenerB.addedCount);

	engine.addEntityListener(&listenerB);

	engine.removeAllEntities(); // calls removeEntity(shared_ptr) on each added entity

	ASSERT_EQ(2u, listenerA.removedCount);
	ASSERT_EQ(2u, listenerB.removedCount);
}

// Test the addSystem(EntitySystem*) getSystem() and removeSystem(typeID) methods
TEST_F(EngineTest, AddGetAndRemoveSystem) {
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_TRUE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);

	engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	engine.addSystem(std::unique_ptr<EntitySystemMockB>(new EntitySystemMockB()));

	auto sA = engine.getSystem<EntitySystemMockA>();
	auto sB = engine.getSystem<EntitySystemMockB>();

	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockA)) == nullptr);
	ASSERT_FALSE(engine.getSystem(typeid(EntitySystemMockB)) == nullptr);
	ASSERT_EQ(1u, sA->addedCalls);
	ASSERT_EQ(1u, sB->addedCalls);

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

	engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	engine.addSystem(std::unique_ptr<EntitySystemMockB>(new EntitySystemMockB()));

	auto aptr = engine.getSystem<EntitySystemMockA>();
	auto bptr = engine.getSystem<EntitySystemMockB>();

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
	auto aptr = (EntitySystemMockA *) engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	auto bptr = (EntitySystemMockB *) engine.addSystem(std::unique_ptr<EntitySystemMockB>(new EntitySystemMockB()));

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

	auto aptr = (EntitySystemMockA *) engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	auto bptr = (EntitySystemMockB *) engine.addSystem(std::unique_ptr<EntitySystemMockB>(new EntitySystemMockB()));

	aptr->priority = 2;
	bptr->priority = 1;

	engine.update(deltaTime);

	uint64_t previous = 0;

	for (auto it = updates->begin(); it != updates->end(); it++) {
		ASSERT_TRUE((*it) >= previous);
		previous = (*it);
	}
}

TEST_F(EngineTest, IgnoreSystem) {
	auto aptr = (EntitySystemMockA *) engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	const int numUpdates = 10;

	for (int i = 0; i < numUpdates; i++) {
		aptr->setProcessing(i % 2 == 0); // set to active if i is even

		engine.update(deltaTime);

		ASSERT_EQ(aptr->updateCalls, i / 2 + 1);
	}
}

TEST_F(EngineTest, EntitiesForFamily) {
	auto family = Family::getFor({typeid(ComponentA), typeid(ComponentB)});
	auto familyEntities = engine.getEntitiesFor(family);

	ASSERT_EQ(0u, familyEntities->size());

	auto e1 = engine.addEntity();
	auto e2 = engine.addEntity();
	auto e3 = engine.addEntity();
	auto e4 = engine.addEntity();

	e1->add<ComponentA>().add<ComponentB>();
	e2->add<ComponentA>().add<ComponentC>();
	e3->add<ComponentA>().add<ComponentB>().add<ComponentC>();
	e4->add<ComponentA>().add<ComponentB>().add<ComponentC>();

	ASSERT_EQ(3u, familyEntities->size());

	bool e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(), [&](Entity *&found) {
		if (found == e1) e1Found = true;
		else if (found == e2) e2Found = true;
		else if (found == e3) e3Found = true;
		else if (found == e4) e4Found = true;
	});

	ASSERT_EQ(e1Found, true);
	ASSERT_EQ(e2Found, false);
	ASSERT_EQ(e3Found, true);
	ASSERT_EQ(e4Found, true);
}

TEST_F(EngineTest, EntityForFamilyWithRemoval) {
	auto e = engine.addEntity();
	e->add<ComponentA>();
	auto entities = engine.getEntitiesFor(Family::getFor({typeid(ComponentA)}));

	ASSERT_EQ(1u, entities->size());
	ASSERT_TRUE(
			std::find_if(entities->begin(), entities->end(), [&](Entity *&found) { return found == e; })
			!= entities->end());

	engine.removeEntity(e);

	ASSERT_EQ(0u, entities->size());
	ASSERT_FALSE(
			std::find_if(entities->begin(), entities->end(), [&](Entity *&found) { return found == e; })
			!= entities->end());
}

TEST_F(EngineTest, EntitiesForFamilyWithRemoval) {
	auto family = Family::getFor({typeid(ComponentA), typeid(ComponentB)});
	auto familyEntities = engine.getEntitiesFor(family);

	ASSERT_EQ(0u, familyEntities->size());

	auto e1u = std::unique_ptr<Entity>(new Entity());
	auto e2u = std::unique_ptr<Entity>(new Entity());
	auto e3u = std::unique_ptr<Entity>(new Entity());
	auto e4u = std::unique_ptr<Entity>(new Entity());

	e1u->add<ComponentA>().add<ComponentB>();
	e2u->add<ComponentA>().add<ComponentC>();
	e3u->add<ComponentA>().add<ComponentB>().add<ComponentC>();
	e4u->add<ComponentA>().add<ComponentB>().add<ComponentC>();

	auto e1 = engine.addEntity(std::move(e1u));
	auto e2 = engine.addEntity(std::move(e2u));
	auto e3 = engine.addEntity(std::move(e3u));
	auto e4 = engine.addEntity(std::move(e4u));

	ASSERT_EQ(3u, familyEntities->size());

	bool e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(), [&](Entity *&found) {
		if (found == e1) e1Found = true;
		else if (found == e2) e2Found = true;
		else if (found == e3) e3Found = true;
		else if (found == e4) e4Found = true;
	});

	ASSERT_EQ(e1Found, true);
	ASSERT_EQ(e2Found, false);
	ASSERT_EQ(e3Found, true);
	ASSERT_EQ(e4Found, true);

	e1->remove<ComponentA>();
	engine.removeEntity(e3);

	e1Found = false, e2Found = false, e3Found = false, e4Found = false;

	std::for_each(familyEntities->begin(), familyEntities->end(), [&](Entity *&found) {
		if (found == e1) e1Found = true;
		else if (found == e2) e2Found = true;
		else if (found == e3) e3Found = true;
		else if (found == e4) e4Found = true;
	});

	EXPECT_EQ(e1Found, false);
	EXPECT_EQ(e2Found, false);
	EXPECT_EQ(e3Found, false);
	EXPECT_EQ(e4Found, true);
	ASSERT_EQ(1u, familyEntities->size());
}

TEST_F(EngineTest, EntitiesForFamilyWithRemovalAndFiltering) {
	auto entsWithAOnly = engine.getEntitiesFor(
			Family::getFor(ComponentType::getBitsFor({typeid(ComponentA)}), // must have A
						   ashley::BitsType(), // ignore
						   ComponentType::getBitsFor({typeid(ComponentB)}))); // must not have B

	auto entsWithB = engine.getEntitiesFor(Family::getFor({typeid(ComponentB)}));

	auto e1u = std::unique_ptr<Entity>(new Entity());
	auto e2u = std::unique_ptr<Entity>(new Entity());

	auto e1 = engine.addEntity(std::move(e1u));
	auto e2 = engine.addEntity(std::move(e2u));

	e1->add<ComponentA>();

	e2->add<ComponentA>();
	e2->add<ComponentB>();

	ASSERT_EQ(1u, entsWithAOnly->size());
	ASSERT_EQ(1u, entsWithB->size());

	e2->remove<ComponentB>();

	ASSERT_EQ(2u, entsWithAOnly->size());
	ASSERT_EQ(0u, entsWithB->size());
}

TEST_F(EngineTest, GetSystems) {
	auto sys = engine.getSystems();

	ASSERT_EQ(sys.size(), 0u);

	engine.addSystem(std::unique_ptr<EntitySystemMockA>(new EntitySystemMockA()));
	engine.addSystem(std::unique_ptr<EntitySystemMockB>(new EntitySystemMockB()));

	sys = engine.getSystems();

	ASSERT_EQ(sys.size(), 2u);
}
