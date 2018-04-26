#include <cstdint>

#include <memory>

#include "Ashley/core/Component.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/core/ComponentMapper.hpp"

#include "Ashley/systems/IteratingSystem.hpp"

#include "AshleyTestCommon.hpp"

using ashley::test::PositionComponent;
using ashley::test::VelocityComponent;

using ashley::Entity;
using ashley::Engine;
using ashley::Family;
using ashley::Component;
using ashley::ComponentMapper;
using ashley::IteratingSystem;

class ComponentA final : public Component {

};

class ComponentB final : public Component {

};

class ComponentC final : public Component {

};

namespace {
class IteratingSystemTest : public ::testing::Test {
protected:
	constexpr static float delta = 0.15f;

	Engine engine;

	Family *family = nullptr;
	Entity *e = nullptr;

	IteratingSystemTest() :
			family(Family::getFor({typeid(ComponentA), typeid(ComponentB)})),
			e{nullptr} {
	}
};

class IteratingSystemMock final : public IteratingSystem {
public:
	explicit IteratingSystemMock(Family *family) :
			IteratingSystem(family, 0),
			numUpdates(0) {
	}

	~IteratingSystemMock() override = default;

	uint64_t numUpdates;

	void processEntity(Entity *entity, float deltaTime) override {
		++numUpdates;
	}
};

class SpyComponent : public ashley::Component {
public:
	int64_t updates = 0;
};

class IndexComponent : public ashley::Component {
public:
	explicit IndexComponent(int nindex = 0) :
			index(nindex) {
	}

	int64_t index = 0;
};

class IteratingComponentRemovalMock : public IteratingSystem {
private:
	ComponentMapper<SpyComponent> sm;
	ComponentMapper<IndexComponent> im;

public:
	explicit IteratingComponentRemovalMock(int64_t priority = 0) :
			IteratingSystem(ashley::Family::getFor({typeid(SpyComponent), typeid(IndexComponent)}),
							priority),
			sm(ComponentMapper<SpyComponent>::getMapper()),
			im(ComponentMapper<IndexComponent>::getMapper()) {
	}

	void processEntity(Entity *entity, float deltaTime) override {
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
	ComponentMapper<SpyComponent> sm;
	ComponentMapper<IndexComponent> im;

public:
	explicit IteratingRemovalMock(int64_t priority = 0) :
			IteratingSystem(ashley::Family::getFor({typeid(SpyComponent), typeid(IndexComponent)}),
							priority),
			sm(ComponentMapper<SpyComponent>::getMapper()),
			im(ComponentMapper<IndexComponent>::getMapper()) {
	}

	void addedToEngine(Engine &engine) override {
		IteratingSystem::addedToEngine(engine);
	}

	void removedFromEngine(Engine &engine) override {
		IteratingSystem::removedFromEngine(engine);
	}

	void processEntity(Entity *entity, float deltaTime) override {
		auto index = im.get(entity)->index;

		if (index % 2 == 0) {
			getEngine()->removeEntity(entity);
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

	ASSERT_EQ(mockSystem->numUpdates, 0u);

	e->add<ComponentB>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 1u);

	e->add<ComponentC>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2u);

	e->remove<ComponentA>();
	e->add<ComponentC>();
	engine.update(delta);

	ASSERT_EQ(mockSystem->numUpdates, 2u);
}

TEST_F(IteratingSystemTest, EntityRemovalWhileIterating) {
	// only uses the engine from IteratingSystemTest

	auto entities = engine.getEntitiesFor(Family::getFor({typeid(SpyComponent), typeid(IndexComponent)}));
	auto sm = ComponentMapper<SpyComponent>::getMapper();

	engine.addSystem<IteratingRemovalMock>(10);

	constexpr uint64_t numEntities = 10u;

	for (uint64_t i = 0u; i < numEntities; i++) {
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

	auto entities = engine.getEntitiesFor(Family::getFor({typeid(SpyComponent), typeid(IndexComponent)}));
	auto sm = ComponentMapper<SpyComponent>::getMapper();

	engine.addSystem<IteratingComponentRemovalMock>(10);

	constexpr uint64_t numEntities = 10u;

	for (uint64_t i = 0u; i < numEntities; i++) {
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
