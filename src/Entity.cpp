/*******************************************************************************
 * Copyright 2014, 2015 See AUTHORS file.
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

#include "Ashley/core/Entity.hpp"
#include "Ashley/signals/Signal.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"

uint64_t ashley::Entity::nextIndex = 0;

ashley::Entity::Entity() :
		        index(nextIndex++) {
}

std::unique_ptr<ashley::Component> ashley::Entity::remove(const std::type_index typeIndex) {
	try {
		return removeImpl(typeIndex);
	} catch (std::out_of_range &oor) {
		return std::unique_ptr<Component> { nullptr };
	}
}

void ashley::Entity::removeAll() {
	componentBits.reset();
	familyBits.reset();

	componentMap.clear();
}

std::vector<ashley::Component *> ashley::Entity::getComponents() const {
	std::vector<ashley::Component *> retVal;

	for (auto &p : componentMap) {
		retVal.emplace_back(p.second.get());
	}

	return retVal;
}

const ashley::BitsType &ashley::Entity::getComponentBits() const {
	return componentBits;
}

void ashley::Entity::addInternal(std::unique_ptr<Component> &&component, std::type_index type) {
	auto typeID = ashley::ComponentType::getIndexFor(type);

	if (componentBits[typeID]) {
		componentMap[type] = nullptr;
	}

	componentBits[typeID] = true;
	componentMap[type] = std::move(component);

	componentAdded.dispatch(this);
}

std::unique_ptr<ashley::Component> ashley::Entity::removeImpl(std::type_index typeIndex) {
	if (operationHandler != nullptr) {
		operationHandler->remove(this, typeIndex);
	} else {
		return removeInternal(typeIndex);
	}

	return std::unique_ptr<Component> { nullptr };
}

std::unique_ptr<ashley::Component> ashley::Entity::removeInternal(std::type_index typeIndex) {
	const auto id = ashley::ComponentType::getIndexFor(typeIndex);

	std::unique_ptr<ashley::Component> ret { nullptr };

	if (componentBits[id] == true) {
		componentBits[id] = false;
		ret = std::move(componentMap.at(typeIndex));
		componentMap.erase(typeIndex);

		componentRemoved.dispatch(this);
	}

	// should be moved automatically
	return ret;
}
