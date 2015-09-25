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

#include "Ashley/core/Entity.hpp"
#include "Ashley/core/Component.hpp"
#include "gtest/gtest.h"

#ifndef ASHLEYTESTCOMMON_HPP_
#define ASHLEYTESTCOMMON_HPP_

namespace ashley {
namespace test {
/**
 * Test component representing a position; will be used to implement a MovementSystem test and also to test component and entity related functions.
 */
class PositionComponent : public ashley::Component {
public:
	int64_t x;
	int64_t y;

	explicit PositionComponent() :
			        PositionComponent(0, 0) {
	}

	explicit PositionComponent(int64_t x, int64_t y) :
			        x(x),
			        y(y) {
	}
};

/**
 * Test component representing a velocity; will be used to implement a MovementSystem test and also to test component and entity related functions.
 */
class VelocityComponent : public ashley::Component {
public:
	int64_t x;
	int64_t y;

	explicit VelocityComponent() :
			        VelocityComponent(0, 0) {
	}

	explicit VelocityComponent(int64_t x, int64_t y) :
			        x(x),
			        y(y) {
	}
};

/**
 * Uses googletest asserts to check that a {@link Entity}'s getComponents() array and getComponentBits() bitset have the given size.
 */
inline void assertValidComponentAndBitSize(ashley::Entity &e, size_t size) {
	auto bitsSize = e.getComponentBits().count();
	auto componentsSize = e.getComponents().size();

	EXPECT_EQ(bitsSize, size) << "Component bits has size " << bitsSize << ", expected " << size << ".";
	EXPECT_EQ(componentsSize, size) << "Component array has size " << componentsSize << ", expected " << size << ".";
}

}

}

#endif /* ASHLEYTESTCOMMON_HPP_ */
