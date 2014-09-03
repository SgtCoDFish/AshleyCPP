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
#include <utility>

#include "Ashley/signals/Signal.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"

uint64_t ashley::Entity::nextIndex = 0;

ashley::Entity::Entity() :
		index(nextIndex++) {
}

ashley::Entity &ashley::Entity::add(std::shared_ptr<Component> component) {
	auto type = component->identify();
	auto typeID = ashley::ComponentType::getIndexFor(type);

	if (componentBits[typeID]) {
		componentMap[type] = nullptr;
	}

	componentBits[typeID] = true;

	// add new reference to component
	componentMap[type] = std::shared_ptr<Component>(component);

	componentAdded.dispatch(*this);
	return *this;
}

void ashley::Entity::removeAll() {
	componentBits.reset();
	familyBits.reset();

	for (auto &p : componentMap) {
		p.second.reset();
	}

	componentMap.clear();
}

const std::vector<std::shared_ptr<ashley::Component>> ashley::Entity::getComponents() const {
	std::vector<std::shared_ptr<ashley::Component>> retVal;

	for (auto &p : componentMap) {
		retVal.emplace_back(std::shared_ptr<ashley::Component>(p.second));
	}

	return retVal;
}

const ashley::BitsType &ashley::Entity::getComponentBits() const {
	return componentBits;
}
