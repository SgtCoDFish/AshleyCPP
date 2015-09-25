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

#include "Ashley/core/Entity.hpp"
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

	Family *family1 = Family::getFor( { typeid(ComponentA) });
	Family *family2 = Family::getFor( { typeid(ComponentB) });
	Family *family3 = Family::getFor( { typeid(ComponentC) });
	Family *family4 = Family::getFor( { typeid(ComponentA), typeid(ComponentB) });
	Family *family5 = Family::getFor( { typeid(ComponentA), typeid(ComponentC) });
	Family *family6 = Family::getFor( { typeid(ComponentB), typeid(ComponentC) });
	Family *family7 = Family::getFor( { typeid(ComponentA), typeid(ComponentB), typeid(ComponentC) });
	Family *family8 = Family::getFor(ashley::ComponentType::getBitsFor<ComponentA, ComponentB>(),
	        ashley::ComponentType::getBitsFor<ComponentC, ComponentD>(),
	        ashley::ComponentType::getBitsFor<ComponentE, ComponentF>());

	FamilyTest() :
			        a(),
			        b(),
			        c(),
			        d(),
			        e(),
			        f() {
	}

	virtual ~FamilyTest() {
	}

};
}

// Check that all the test fixture families are generated correctly.
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

// Ensure that equal invocations produce equal families, including with different orders of components.
TEST_F(FamilyTest, SameFamily) {
	auto familyA = Family::getFor( { typeid(ComponentA) });
	auto familyB = Family::getFor( { typeid(ComponentB) });

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

	auto familyBA = Family::getFor( { typeid(ComponentB), typeid(ComponentA) });
	auto familyCA = Family::getFor( { typeid(ComponentC), typeid(ComponentA) });
	auto familyCB = Family::getFor( { typeid(ComponentB), typeid(ComponentC) });

	ASSERT_TRUE(familyBA == family4);
	ASSERT_TRUE(familyCA == family5);
	ASSERT_TRUE(familyCB == family6);

	auto familyCBA = Family::getFor( { typeid(ComponentC), typeid(ComponentB), typeid(ComponentA) });
	ASSERT_TRUE(familyCBA == family7);
}

// Ensure that different invocations produce different families, including with different orders of components.
TEST_F(FamilyTest, DifferentFamily) {
	auto family12 = Family::getFor(ashley::ComponentType::getBitsFor( { typeid(ComponentC), typeid(ComponentD) }),
	        ashley::ComponentType::getBitsFor( { typeid(ComponentE), typeid(ComponentF) }),
	        ashley::ComponentType::getBitsFor( { typeid(ComponentA), typeid(ComponentB) }));

	ASSERT_FALSE(family1 == family2);
	ASSERT_FALSE(family1 == family3);
	ASSERT_FALSE(family1 == family4);
	ASSERT_FALSE(family1 == family5);
	ASSERT_FALSE(family1 == family6);
	ASSERT_FALSE(family1 == family7);
	ASSERT_FALSE(family1 == family8);
	ASSERT_FALSE(family1 == family12);

	ASSERT_FALSE(family7 == family12);
	ASSERT_FALSE(family8 == family12);
}

// Ensure that entity matching works correctly
TEST_F(FamilyTest, EntityMatch) {
	ashley::Entity e;
	e.add<ComponentA>().add<ComponentB>();

	ASSERT_TRUE(family1->matches(e));
	ASSERT_TRUE(family2->matches(e));
	ASSERT_TRUE(family4->matches(e));

	e.add<ComponentC>();

	ASSERT_TRUE(family1->matches(e));
	ASSERT_TRUE(family2->matches(e));
	ASSERT_TRUE(family3->matches(e));
	ASSERT_TRUE(family4->matches(e));
	ASSERT_TRUE(family5->matches(e));
	ASSERT_TRUE(family6->matches(e));
	ASSERT_TRUE(family7->matches(e));
	ASSERT_TRUE(family8->matches(e));
}

// Ensure that families don't match incorrect entities
TEST_F(FamilyTest, EntityMismatch) {
	ashley::Entity e;
	e.add<ComponentA>().add<ComponentB>();

	ASSERT_FALSE(family3->matches(e));
	ASSERT_FALSE(family5->matches(e));
	ASSERT_FALSE(family6->matches(e));
	ASSERT_FALSE(family7->matches(e));
	ASSERT_FALSE(family8->matches(e));

	e.remove<ComponentB>();

	ASSERT_FALSE(family2->matches(e));
	ASSERT_FALSE(family3->matches(e));
	ASSERT_FALSE(family4->matches(e));
	ASSERT_FALSE(family5->matches(e));
	ASSERT_FALSE(family6->matches(e));
	ASSERT_FALSE(family7->matches(e));
	ASSERT_FALSE(family8->matches(e));
}

// Ensure that a matched entity gives a mismatch after it changes
TEST_F(FamilyTest, EntityMatchThenMismatch) {
	ashley::Entity e;
	e.add<ComponentA>().add<ComponentB>();

	ASSERT_TRUE(family4->matches(e));

	e.remove<ComponentA>();

	ASSERT_FALSE(family4->matches(e));
}

// Ensure a mismatched entity is matched after it changes
TEST_F(FamilyTest, EntityMismatchThenMatch) {
	ashley::Entity e;
	e.add<ComponentA>().add<ComponentC>();

	ASSERT_FALSE(family4->matches(e));

	e.add<ComponentB>();

	ASSERT_TRUE(family4->matches(e));
}

// Ensure that the 3-argument filter getFor works correctly
TEST_F(FamilyTest, FamilyFiltering) {
	/*
	 * family12 has:
	 * All: C, D
	 * One: A, B
	 * None: E, F
	 * Same as java family2 in this test
	 */
	auto family12 = Family::getFor(ashley::ComponentType::getBitsFor( { typeid(ComponentC), typeid(ComponentD) }),
	        ashley::ComponentType::getBitsFor( { typeid(ComponentA), typeid(ComponentB) }),
	        ashley::ComponentType::getBitsFor( { typeid(ComponentE), typeid(ComponentF) }));

	/* family8 has:
	 * All: A, B
	 * One: C, D
	 * None: E, F
	 * Same as java family1 in this test
	 */

	ashley::Entity e;

	ASSERT_FALSE(family8->matches(e));
	ASSERT_FALSE(family12->matches(e));

	e.add<ComponentA>().add<ComponentB>();

	ASSERT_FALSE(family8->matches(e));
	ASSERT_FALSE(family12->matches(e));

	e.add<ComponentC>();

	ASSERT_TRUE(family8->matches(e));
	ASSERT_FALSE(family12->matches(e));

	e.add<ComponentD>();

	ASSERT_TRUE(family8->matches(e));
	ASSERT_TRUE(family12->matches(e));

	e.add<ComponentE>();

	ASSERT_FALSE(family8->matches(e));
	ASSERT_FALSE(family12->matches(e));

	e.remove<ComponentE>();

	ASSERT_TRUE(family8->matches(e));
	ASSERT_TRUE(family12->matches(e));

	e.remove<ComponentA>();

	ASSERT_FALSE(family8->matches(e));
	ASSERT_TRUE(family12->matches(e));
}
