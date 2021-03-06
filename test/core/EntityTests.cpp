#include <cstdint>

#include <vector>
#include <bitset>
#include <typeinfo>
#include <typeindex>
#include <memory>

#include "Ashley/core/Entity.hpp"

#include "AshleyTestCommon.hpp"

namespace {
class EntityTest : public ::testing::Test {
protected:
	EntityTest() = default;

	~EntityTest() override = default;

	void SetUp() override {
		onlyPosition.add<ashley::test::PositionComponent>(initialXPos, initialYPos);
		onlyVelocity.add<ashley::test::VelocityComponent>(initialXVel, initialXVel);
		positionAndVelocity.add<ashley::test::PositionComponent>(initialXPos, initialYPos).add<
				ashley::test::VelocityComponent>(initialXVel, initialYVel);
	}

	void TearDown() override {
		emptyEntity.removeAll();
		onlyPosition.removeAll();
		onlyVelocity.removeAll();
		positionAndVelocity.removeAll();
	}

	int initialXPos = 5;
	int initialYPos = 6;
	int initialXVel = 10;
	int initialYVel = 2;

	ashley::Entity emptyEntity;
	ashley::Entity onlyPosition;
	ashley::Entity onlyVelocity;
	ashley::Entity positionAndVelocity;
};
}

class EntityListenerMock final : public ashley::Listener<ashley::Entity> {
public:
	uint64_t counter;

	EntityListenerMock() :
			counter(0) {
	}

	void receive(ashley::Signal<ashley::Entity> *signal, ashley::Entity *object) override {
		++counter;
	}
};

// Ensure that all entities obtain different IDs.
TEST_F(EntityTest, UniqueIndex) {
	const int numEntities = 1000;
	std::bitset<numEntities * 4> ids;

	for (int i = 0; i < numEntities; i++) {
		ashley::Entity e;
		ASSERT_FALSE(ids[(e.getIndex())] == 1) << "Non-unique entity ID generated: " << e.getIndex() << ".";
	}
}

// Ensure that an empty entity is treated correctly by various functions.
TEST_F(EntityTest, NoComponents) {
	ashley::test::assertValidComponentAndBitSize(emptyEntity, 0);
}

// Ensure that adding and removing components works correctly.
TEST_F(EntityTest, AddAndRemoveComponents) {
	ashley::test::assertValidComponentAndBitSize(onlyPosition, 1);
	ashley::test::assertValidComponentAndBitSize(onlyVelocity, 1);
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);

	auto bits = onlyPosition.getComponentBits();
	auto positionIndex = ashley::ComponentType::getIndexFor(std::type_index(typeid(ashley::test::PositionComponent)));

	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(i == (positionIndex), bits[i]);
	}

	onlyPosition.remove<ashley::test::PositionComponent>();

	ashley::test::assertValidComponentAndBitSize(onlyPosition, 0);
	ASSERT_FALSE(onlyPosition.hasComponent<ashley::test::PositionComponent>());

	bits = onlyPosition.getComponentBits();

	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(0, bits[i]) << "i = " << i << ".";
	}

	positionAndVelocity.remove<ashley::test::VelocityComponent>();
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 1);
	ASSERT_FALSE(positionAndVelocity.hasComponent<ashley::test::VelocityComponent>());
	ASSERT_TRUE(positionAndVelocity.hasComponent<ashley::test::PositionComponent>());

	bits = positionAndVelocity.getComponentBits();
	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(i == positionIndex, bits[i]) << "i = " << i << ".";
	}

	positionAndVelocity.remove<ashley::test::PositionComponent>();
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 0);
	ASSERT_FALSE(positionAndVelocity.hasComponent<ashley::test::PositionComponent>());
	ASSERT_FALSE(positionAndVelocity.hasComponent<ashley::test::VelocityComponent>());

	bits = positionAndVelocity.getComponentBits();
	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(0, bits[i]) << "i = " << i << ".";
	}
}

TEST_F(EntityTest, AddExistingComponent) {
	auto e = std::unique_ptr<ashley::Entity>(new ashley::Entity());

	auto pos = std::unique_ptr<ashley::test::PositionComponent>(new ashley::test::PositionComponent(5, 5));

	e->add(std::move(pos));

	ASSERT_TRUE(e->hasComponent<ashley::test::PositionComponent>());
	ASSERT_FALSE(e->hasComponent<ashley::test::VelocityComponent>());

	auto posComp = e->getComponent<ashley::test::PositionComponent>();

	ASSERT_EQ(5, posComp->x);
	ASSERT_EQ(5, posComp->y);
}

// Ensure the removeAll() function works
TEST_F(EntityTest, AddAndRemoveAllComponents) {
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);

	auto bits = positionAndVelocity.getComponentBits();

	auto pIndex = ashley::ComponentType::getIndexFor(std::type_index(typeid(ashley::test::PositionComponent)));

	auto vIndex = ashley::ComponentType::getIndexFor(std::type_index(typeid(ashley::test::VelocityComponent)));

	for (unsigned int i = 0; i < bits.count(); i++) {
		ASSERT_EQ(i == pIndex || i == vIndex, bits[i]) << "Invalid bits";
	}

	positionAndVelocity.removeAll();

	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 0);

	bits = positionAndVelocity.getComponentBits();

	for (unsigned int i = 0; i < bits.count(); i++) {
		ASSERT_EQ(0, bits[i]) << "Invalid bits";
	}
}

// Ensure the hasComponent<>() and getComponent<>() functions work correctly.
TEST_F(EntityTest, HasAndGetComponent) {
	EXPECT_TRUE(positionAndVelocity.hasComponent<ashley::test::PositionComponent>()) << "hasComponent failed.";
	EXPECT_TRUE(positionAndVelocity.hasComponent<ashley::test::VelocityComponent>()) << "hasComponent failed.";
	EXPECT_TRUE(onlyPosition.hasComponent<ashley::test::PositionComponent>()) << "hasComponent failed.";
	EXPECT_TRUE(onlyVelocity.hasComponent<ashley::test::VelocityComponent>()) << "hasComponent failed.";

	EXPECT_FALSE(onlyPosition.hasComponent<ashley::test::VelocityComponent>()) << "hasComponent failed.";
	EXPECT_FALSE(onlyVelocity.hasComponent<ashley::test::PositionComponent>()) << "hasComponent failed.";

	EXPECT_FALSE((onlyPosition.getComponent<ashley::test::PositionComponent>()) == nullptr) << "getComponent failed.";
	EXPECT_FALSE(onlyVelocity.getComponent<ashley::test::VelocityComponent>() == nullptr) << "getComponent failed.";
	EXPECT_FALSE(positionAndVelocity.getComponent<ashley::test::PositionComponent>() == nullptr)
						<< "getComponent failed.";
	EXPECT_FALSE(positionAndVelocity.getComponent<ashley::test::VelocityComponent>() == nullptr)
						<< "getComponent failed.";

	EXPECT_TRUE(onlyPosition.getComponent<ashley::test::VelocityComponent>() == nullptr);
	EXPECT_TRUE(onlyVelocity.getComponent<ashley::test::PositionComponent>() == nullptr);
}

// Ensure that adding the same type of component a second time overwrites correctly.
TEST_F(EntityTest, AddSameComponent) {
	auto posComp = positionAndVelocity.getComponent<ashley::test::PositionComponent>();
	auto velComp = positionAndVelocity.getComponent<ashley::test::VelocityComponent>();

	ASSERT_FALSE(posComp == nullptr);
	ASSERT_FALSE(velComp == nullptr);

	EXPECT_TRUE(posComp->x == initialXPos) << "Invalid x position.";
	EXPECT_TRUE(posComp->y == initialYPos) << "Invalid y position.";

	EXPECT_TRUE(velComp->x == initialXVel) << "Invalid x velocity.";
	EXPECT_TRUE(velComp->y == initialYVel) << "Invalid y velocity.";

	positionAndVelocity.add<ashley::test::PositionComponent>(initialXPos * 2, initialYPos * 2);

	posComp = positionAndVelocity.getComponent<ashley::test::PositionComponent>();

	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);
	EXPECT_FALSE(posComp->x == initialXPos) << "x position unchanged.";
	EXPECT_FALSE(posComp->y == initialYPos) << "y position unchanged.";

	EXPECT_TRUE(posComp->x == (initialXPos * 2)) << "Invalid x position.";
	EXPECT_TRUE(posComp->y == (initialYPos * 2)) << "Invalid y position.";

	positionAndVelocity.add<ashley::test::VelocityComponent>(initialXVel * 2, initialYVel * 2);
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);

	velComp = positionAndVelocity.getComponent<ashley::test::VelocityComponent>();

	EXPECT_FALSE(velComp->x == initialXVel) << "Unchanged x velocity.";
	EXPECT_FALSE(velComp->y == initialYVel) << "Unchanged y velocity.";

	EXPECT_TRUE(velComp->x == (initialXVel * 2)) << "Invalid x velocity.";
	EXPECT_TRUE(velComp->y == (initialYVel * 2)) << "Invalid y velocity.";
}

// Ensure the component listener signals work as intended
TEST_F(EntityTest, ComponentListener) {
	ashley::Entity e;
	auto dynAdd = std::unique_ptr<EntityListenerMock>(new EntityListenerMock());
	auto dynRem = std::unique_ptr<EntityListenerMock>(new EntityListenerMock());

	e.componentAdded.add(dynAdd.get());
	e.componentRemoved.add(dynRem.get());

	ASSERT_EQ(0u, dynAdd->counter);
	ASSERT_EQ(0u, dynRem->counter);

	e.add<ashley::test::PositionComponent>(5, 2);

	ASSERT_EQ(1u, dynAdd->counter);
	ASSERT_EQ(0u, dynRem->counter);

	e.remove<ashley::test::PositionComponent>();

	ASSERT_EQ(1u, dynAdd->counter);
	ASSERT_EQ(1u, dynRem->counter);

	e.add<ashley::test::VelocityComponent>(5, 222);

	ASSERT_EQ(2u, dynAdd->counter);
	ASSERT_EQ(1u, dynRem->counter);

	e.remove<ashley::test::VelocityComponent>();

	ASSERT_EQ(2u, dynAdd->counter);
	ASSERT_EQ(2u, dynRem->counter);
}
