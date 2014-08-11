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

#include <iostream>
#include <vector>
#include <bitset>
#include <typeinfo>
#include <typeindex>

#include "Ashley/core/Entity.hpp"
#include "Ashley/core/ComponentType.hpp"

#include "AshleyTestCommon.hpp"

#include "gtest/gtest.h"
namespace {
class EntityTest : public ::testing::Test {
protected:
	EntityTest() {
	}
	virtual ~EntityTest() {
	}

	virtual void SetUp() {
//		std::cout << "SetUp()\n";
		ashley::test::PositionComponent p(5, 6);
		ashley::test::VelocityComponent v(10, 2);

		onlyPosition.add(p);
		onlyVelocity.add(v);
		positionAndVelocity.add(p).add(v);
	}

	virtual void TearDown() {
//		std::cout << "TearDown()\n";
		emptyEntity.removeAll();
		onlyPosition.removeAll();
		onlyVelocity.removeAll();
		positionAndVelocity.removeAll();
	}

	ashley::Entity emptyEntity;
	ashley::Entity onlyPosition;
	ashley::Entity onlyVelocity;
	ashley::Entity positionAndVelocity;
};
}

// Ensure that all entities obtain different IDs.
TEST_F(EntityTest, UniqueIndex) {
//	std::cout << "In UniqueIndex\n";
	const int numEntities = 1000;

	std::vector<ashley::Entity> entities(numEntities);
	std::bitset<numEntities * 4> ids;

	for (int i = 0; i < numEntities; i++) {
		ashley::Entity e;
		ASSERT_FALSE(ids[(e.getIndex())] == 1)<< "Non-unique entity ID generated: " << e.getIndex() << ".";
		ids[e.getIndex()] = 1;
		entities.push_back(e);
	}
}

// Ensure that an empty entity is treated correctly by various functions.
TEST_F(EntityTest, NoComponents) {
//	std::cout << "In NoComponents\n";
	ashley::test::assertValidComponentAndBitSize(emptyEntity, 0);

	//TODO: Add more functions here testing interaction with ComponentMapper, see java original
}

// Ensure that adding and removing components works correctly.
TEST_F(EntityTest, AddAndRemoveComponents) {
//	std::cout << "In AddAndRemoveComponents\n";
	ashley::test::assertValidComponentAndBitSize(onlyPosition, 1);
	ashley::test::assertValidComponentAndBitSize(onlyVelocity, 1);
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);

	auto bits = onlyPosition.getComponentBits();
	auto positionIndex = ashley::ComponentType::getIndexFor(
			std::type_index(typeid(ashley::test::PositionComponent)));

	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(i == (positionIndex), bits[i]);
	}

	//TODO: ComponentMapper functions

	onlyPosition.remove<ashley::test::PositionComponent>();

	ashley::test::assertValidComponentAndBitSize(onlyPosition, 0);
	ASSERT_FALSE(onlyPosition.hasComponent<ashley::test::PositionComponent>());

	bits = onlyPosition.getComponentBits();

	for (unsigned int i = 0; i < bits.size(); i++) {
		ASSERT_EQ(0, bits[i])<< "i = " << i << ".";
	}

	// TODO: ComponentMapper functions
}

TEST_F(EntityTest, AddAndRemoveAllComponents) {
	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 2);

	auto bits = positionAndVelocity.getComponentBits();

	auto pIndex = ashley::ComponentType::getIndexFor(
			std::type_index(typeid(ashley::test::PositionComponent)));

	auto vIndex = ashley::ComponentType::getIndexFor(
			std::type_index(typeid(ashley::test::VelocityComponent)));

	for (unsigned int i = 0; i < bits.count(); i++) {
		ASSERT_EQ(i == pIndex || i == vIndex, bits[i])<< "Invalid bits";
	}

	positionAndVelocity.removeAll();

	ashley::test::assertValidComponentAndBitSize(positionAndVelocity, 0);

	bits = positionAndVelocity.getComponentBits();

	for (unsigned int i = 0; i < bits.count(); i++) {
		ASSERT_EQ(0, bits[i])<< "Invalid bits";
	}
}

TEST_F(EntityTest, HasAndGetComponent) {
	EXPECT_TRUE(positionAndVelocity.hasComponent<ashley::test::PositionComponent>())<< "hasComponent failed.";
	EXPECT_TRUE(positionAndVelocity.hasComponent<ashley::test::VelocityComponent>())<< "hasComponent failed.";
	EXPECT_TRUE(positionAndVelocity.getComponent<ashley::test::PositionComponent>() != nullptr)<< "getComponent failed.";
	EXPECT_TRUE(positionAndVelocity.getComponent<ashley::test::VelocityComponent>() != nullptr)<< "getComponent failed.";

	EXPECT_TRUE(onlyPosition.hasComponent<ashley::test::PositionComponent>());
	EXPECT_TRUE(onlyPosition.getComponent<ashley::test::PositionComponent>() != nullptr)<< "getComponent failed.";

	EXPECT_TRUE(onlyVelocity.hasComponent<ashley::test::VelocityComponent>());
	EXPECT_TRUE(onlyVelocity.getComponent<ashley::test::VelocityComponent>() != nullptr)<< "getComponent failed.";

}
