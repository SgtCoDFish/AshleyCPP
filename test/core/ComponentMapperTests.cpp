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

#include "AshleyTestCommon.hpp"

#include "Ashley/core/ComponentMapper.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/ComponentType.hpp"

#include "gtest/gtest.h"

namespace {
class ComponentMapperTest : public ::testing::Test {
protected:
	ComponentMapperTest() {
	}
	virtual ~ComponentMapperTest() {
	}

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
