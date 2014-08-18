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

#include "Ashley/core/Component.hpp"
#include "AshleyTestCommon.hpp"
#include "Ashley/systems/IteratingSystem.hpp"

#include "gtest/gtest.h"

using ashley::test::PositionComponent;
using ashley::test::VelocityComponent;

namespace {
class IteratingSystemTest : public ::testing::Test {
protected:

};

class IteratingSystemMock : public ashley::IteratingSystem {
public:
	IteratingSystemMock(ashley::Family &family) : IteratingSystem(family), numUpdates(0) {}
	virtual ~IteratingSystemMock() {}

	int numUpdates;


	void processEntity(ashley::Entity &entity, float deltaTime) override {
		++numUpdates;
	}
};

class ComponantA : public ashley::Component {

};

class ComponantB : public ashley::Component {

};

class ComponantC : public ashley::Component {

};
}

TEST_F(IteratingSystemTest, ShouldIterateEntitiesWithCorrectFamily) {

}
