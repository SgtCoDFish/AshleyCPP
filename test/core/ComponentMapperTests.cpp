#include "AshleyTestCommon.hpp"

#include "Ashley/core/ComponentMapper.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/ComponentType.hpp"

#include "gtest/gtest.h"

namespace {
class ComponentMapperTest : public ::testing::Test {
protected:
	ComponentMapperTest() = default;
	~ComponentMapperTest() override = default;

	ashley::ComponentMapper<ashley::test::PositionComponent> positionMapper = ashley::ComponentMapper<
	        ashley::test::PositionComponent>::getMapper();
	ashley::ComponentMapper<ashley::test::VelocityComponent> velocityMapper = ashley::ComponentMapper<
	        ashley::test::VelocityComponent>::getMapper();
};

class FooComponent : public ashley::Component {
};
}

TEST_F(ComponentMapperTest, ValidGetAndHas) {
	auto fooMapper = ashley::ComponentMapper<FooComponent>::getMapper();

	auto e = std::unique_ptr<ashley::Entity>(new ashley::Entity());
	e->add<ashley::test::PositionComponent>(10, 2).add<ashley::test::VelocityComponent>(5, 6);

	ASSERT_TRUE(positionMapper.has(e.get()));
	ASSERT_TRUE(velocityMapper.has(e.get()));
	ASSERT_FALSE(fooMapper.has(e.get()));

	ASSERT_TRUE(positionMapper.get(e.get()) != nullptr);
	ASSERT_TRUE(positionMapper.get(e.get()) != nullptr);

	ASSERT_TRUE(positionMapper.get(e.get())->x == 10);
	ASSERT_TRUE(positionMapper.get(e.get())->y == 2);

	ASSERT_TRUE(velocityMapper.get(e.get())->x == 5);
	ASSERT_TRUE(velocityMapper.get(e.get())->y == 6);

	ASSERT_TRUE(fooMapper.get(e.get()) == nullptr);

	e->add<FooComponent>();

	ASSERT_FALSE(fooMapper.get(e.get()) == nullptr);
}
