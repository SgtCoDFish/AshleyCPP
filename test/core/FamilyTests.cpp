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

	FamilyTest() : a(), b(), c(), d(), e(), f() {}
	virtual ~FamilyTest() {}


};
}

TEST_F(FamilyTest, ValidFamily) {

}

TEST_F(FamilyTest, SameFamily) {

}

TEST_F(FamilyTest, DifferentFamily) {

}

