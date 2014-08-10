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
#include <cassert>

#include <algorithm>
#include <typeindex>
#include <memory>

#include "Ashley/signals/Signal.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"

uint64_t ashley::Entity::nextIndex = 0;

ashley::Entity::Entity() :
		componentAdded(), componentRemoved(), index(nextIndex++) {
}

ashley::Entity::~Entity() {
	components.clear();
}

ashley::Entity& ashley::Entity::add(ashley::Component &component) {
	auto typeIndex = component.identify();
	auto componentType = ComponentType::getFor(typeIndex);
	auto typeID = componentType.getIndex();

	if (componentBits[typeID]) {
		std::replace_if(components.begin(), components.end(),
				[&](ashley::Entity::ComponentsStoreType c) {return std::type_index(typeid(c)) == typeIndex;},
				&component);
	} else {
		components.push_back(&component);
	}

	componentBits[typeID] = true;

	componentAdded.dispatch(this);
	return *this;
}

ashley::Entity::ComponentsStoreType ashley::Entity::remove(const std::type_index &index) {
	ashley::Entity::ComponentsStoreType retVal = nullptr;

	auto componentType = ComponentType::getFor(index);
	auto typeID = componentType.getIndex();

	if (componentBits[typeID]) {
		auto loc = components.begin();

		for (auto it = components.begin(); it != components.end(); it++) {
			ashley::Entity::ComponentsStoreType vv = (*it);
			if (index == vv->identify()) {
				retVal = vv;
				loc = it;
				break;
			}
		}

		if (retVal != nullptr) {
			components.erase(loc);
			componentRemoved.dispatch(this);
			componentBits[typeID] = false;
		}
	}

	return retVal;
}

void ashley::Entity::removeAll() {
	while (components.size() > 0) {
		remove(std::type_index(typeid(components[0])));
	}

	assert(components.size() == 0 && "Entity component list size != 0 after removeAll().");
}

const ashley::Entity::ComponentsCollectionType ashley::Entity::getComponents() const {
	return components;
}

ashley::Entity::ComponentsStoreType ashley::Entity::getComponent(ashley::ComponentType &type) const {
	auto it =
			std::find_if(components.begin(), components.end(),
					[&](ashley::Entity::ComponentsStoreType found) {return ashley::ComponentType::getFor(found->identify()) == type;});

	return (it == components.end() ? nullptr : *it);
}

bool ashley::Entity::hasComponent(ashley::ComponentType &type) const {
	return componentBits[type.getIndex()];
}

const ashley::Entity::BitsStoreType &ashley::Entity::getComponentBits() const {
	return componentBits;
}
