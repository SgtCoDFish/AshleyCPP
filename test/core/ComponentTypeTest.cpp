#include <typeindex>
#include <typeinfo>

#include "Ashley/core/ComponentType.hpp"
#include "AshleyTestCommon.hpp"

#include "gtest/gtest.h"

// Ensures that we don't waste any memory by having a vtable on Components; that is, tests that a Component has a size of 1 (the minimum)
TEST(ComponentTypeTest, MinSizeComponents) {
	ASSERT_EQ(sizeof(ashley::Component), 1u);
}

// Ensures a valid type is returned when a call is made to the various getFor() functions, and that this type is the same in the various functions.
// Note that this covers both the validComponentType and sameComponentType tests in the Java version
TEST(ComponentTypeTest, ValidComponentTypes) {
	auto type1 = ashley::ComponentType::getFor(typeid(ashley::test::PositionComponent)); // std::type_info
	auto type2 = ashley::ComponentType::getFor(std::type_index(typeid(ashley::test::PositionComponent))); // std::type_index
	auto type3 = ashley::ComponentType::getFor<ashley::test::PositionComponent>(); // pass by template type

	ASSERT_EQ(type1, type2);
	ASSERT_EQ(type2, type3);
}

// Ensures a valid and consistent index is returned when a call is made to the various getIndexFor() functions
TEST(ComponentTypeTest, ValidComponentIndexTypes) {
	auto id1 = ashley::ComponentType::getIndexFor(typeid(ashley::test::PositionComponent));
	auto id2 = ashley::ComponentType::getIndexFor(std::type_index(typeid(ashley::test::PositionComponent)));
	auto id3 = ashley::ComponentType::getIndexFor<ashley::test::PositionComponent>();

	ASSERT_EQ(id1, id2);
	ASSERT_EQ(id2, id3);
}

// Ensure that two different component types generate different indices.
TEST(ComponentTypeTest, DifferentComponentType) {
	auto type1 = ashley::ComponentType::getFor<ashley::test::PositionComponent>();
	auto type2 = ashley::ComponentType::getFor<ashley::test::VelocityComponent>();

	ASSERT_NE(type1.getIndex(), type2.getIndex());
	ASSERT_NE(type1, type2);
}

TEST(ComponentTypeTest, GetBitsFor) {
	auto type1 = ashley::ComponentType::getBitsFor<ashley::test::PositionComponent, ashley::test::VelocityComponent>();
	auto type2 = ashley::ComponentType::getBitsFor( { typeid(ashley::test::PositionComponent),
	        typeid(ashley::test::VelocityComponent) });

	auto type3 = ashley::ComponentType::getBitsFor( { typeid(ashley::test::PositionComponent) });
	auto type4 = ashley::ComponentType::getBitsFor( { std::type_index(typeid(ashley::test::PositionComponent)) });

	ASSERT_EQ(type1, type2);
	ASSERT_EQ(type3, type4);
}
