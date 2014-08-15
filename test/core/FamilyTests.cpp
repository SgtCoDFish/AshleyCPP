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

#include <typeinfo>
#include <typeindex>
#include <memory>

#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/core/Family.hpp"

#include "gtest/gtest.h"

using ashley::Family;

namespace {
class ComponentA : public ashley::Component {

};

class ComponentB : public ashley::Component {

};

class ComponentC : public ashley::Component {

};

class ComponentD : public ashley::Component {

};

class ComponentE : public ashley::Component {

};

class ComponentF : public ashley::Component {

};

class FamilyTest : public ::testing::Test {
protected:
	ComponentA a;
	ComponentB b;
	ComponentC c;
	ComponentD d;
	ComponentE e;
	ComponentF f;

	std::shared_ptr<Family> family1 = Family::getFor(typeid(ComponentA));
	std::shared_ptr<Family> family2 = Family::getFor(typeid(ComponentB));
	std::shared_ptr<Family> family3 = Family::getFor(typeid(ComponentC));
	std::shared_ptr<Family> family4 = Family::getFor({typeid(ComponentA), typeid(ComponentB)});
	std::shared_ptr<Family> family5 = Family::getFor({typeid(ComponentA), typeid(ComponentC)});
	std::shared_ptr<Family> family6 = Family::getFor({typeid(ComponentB), typeid(ComponentC)});
	std::shared_ptr<Family> family7 = Family::getFor({typeid(ComponentA), typeid(ComponentB), typeid(ComponentC)});
	std::shared_ptr<Family> family8 = Family::getFor(ashley::ComponentType::getBitsFor<ComponentA, ComponentB>(),
			ashley::ComponentType::getBitsFor<ComponentC, ComponentD>(),
			ashley::ComponentType::getBitsFor<ComponentE, ComponentF>());

	FamilyTest() :
			a(), b(), c(), d(), e(), f() {
	}

	virtual ~FamilyTest() {
	}

};
}

TEST_F(FamilyTest, ValidFamily) {
	ASSERT_FALSE(family1 == nullptr);
	ASSERT_FALSE(family2 == nullptr);
	ASSERT_FALSE(family3 == nullptr);
	ASSERT_FALSE(family4 == nullptr);
	ASSERT_FALSE(family5 == nullptr);
	ASSERT_FALSE(family6 == nullptr);
	ASSERT_FALSE(family7 == nullptr);
	ASSERT_FALSE(family8 == nullptr);
}

TEST_F(FamilyTest, SameFamily) {
	auto familyA = Family::getFor(typeid(ComponentA));
	auto familyB = Family::getFor(typeid(ComponentB));

	auto familyLong = Family::getFor(ashley::ComponentType::getBitsFor<ComponentA, ComponentB>(),
				ashley::ComponentType::getBitsFor<ComponentC, ComponentD>(),
				ashley::ComponentType::getBitsFor<ComponentE, ComponentF>());

	ASSERT_TRUE(familyA == family1);
	ASSERT_FALSE(familyA == family2);

	ASSERT_TRUE(familyB == family2);
	ASSERT_FALSE(familyB == family1);

	ASSERT_TRUE(familyLong == family8);
	ASSERT_FALSE(family8 == familyA);
	ASSERT_FALSE(family8 == familyB);
}

TEST_F(FamilyTest, DifferentFamily) {

}

