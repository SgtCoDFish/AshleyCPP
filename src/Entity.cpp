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

#include <cassert>
#include <cstdint>

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <typeindex>
#include <memory>

#include "Ashley/signals/Signal.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"

uint64_t ashley::Entity::nextIndex = 0;

ashley::Entity::Entity() :
		index(nextIndex++) {
}

ashley::Entity::~Entity() {
	components.clear();
}

ashley::Entity& ashley::Entity::add(ashley::Component &component) {
	auto typeIndex = component.identify();
	auto typeID = ComponentType::getIndexFor(typeIndex);

	components[typeIndex] = &component;

	componentBits[typeID] = true;

	componentAdded.dispatch(this);
	return *this;
}

void ashley::Entity::removeAll() {
	components.clear();
	componentBits.reset();
	familyBits.reset();
}

std::unordered_map<std::type_index, ashley::Component *> ashley::Entity::getComponents() const {
	return components;
}

const std::vector<ashley::Component *> ashley::Entity::getComponentsVector() const {
	std::vector<ashley::Component *> retVal;
	std::for_each(components.begin(), components.end(),
			[&](std::pair<std::type_index, Component *> vals) {retVal.push_back(vals.second);});
	return retVal;
}

const std::bitset<ASHLEY_MAX_COMPONENT_COUNT> &ashley::Entity::getComponentBits() const {
	return componentBits;
}
