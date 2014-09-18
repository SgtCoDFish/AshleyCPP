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

#include <iostream>
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

ashley::Entity::~Entity() {
	removeAll();
}

ashley::Entity &ashley::Entity::add(std::shared_ptr<Component> component) {
	if (operationHandler != nullptr) {
		operationHandler->add(this, component);
	} else {
		addInternal(component);
	}

	return *this;
}

std::shared_ptr<ashley::Component> ashley::Entity::remove(std::type_index typeIndex) {
	std::shared_ptr<ashley::Component> com = nullptr;

	std::cout << "In remove(ti): type is " << typeIndex.name() << ".\n";
	std::cout.flush();

	try {
		com = componentMap.at(typeIndex);
		return removeImpl(com);
	} catch(std::out_of_range &oor) {
		return std::shared_ptr<ashley::Component>(nullptr);
	}

}

std::shared_ptr<ashley::Component> ashley::Entity::remove(std::shared_ptr<ashley::Component> &component) {
	auto index = component->identify();
	std::cout << "In remove(shr): type is " << index.name() << ".\n";
	std::cout.flush();
	return remove(index);
}

void ashley::Entity::removeAll() {
	componentBits.reset();
	familyBits.reset();

	for (auto &p : componentMap) {
		p.second = nullptr;
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

ashley::Entity &ashley::Entity::addInternal(std::shared_ptr<ashley::Component> component) {
	auto type = component->identify();
	std::cout << "Adding type: " << type.name() << ".\n";
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

std::shared_ptr<ashley::Component> ashley::Entity::removeImpl(std::shared_ptr<Component> &component) {
	if(operationHandler != nullptr) {
		operationHandler->remove(this, component);
	} else{
		return removeInternal(component);
	}

	return component;
}

std::shared_ptr<ashley::Component> ashley::Entity::removeInternal(std::shared_ptr<Component> &component) {
	auto typeIndex = component->identify();
	auto id = ashley::ComponentType::getIndexFor(typeIndex);

	std::cout << "In remove(intern): type is " << typeIndex.name() << ".\n";
	std::cout.flush();

	if (componentBits[id] == true) {
		auto retVal = std::shared_ptr<ashley::Component>(componentMap[typeIndex]);

		componentBits[id] = false;
		componentMap.at(typeIndex) = nullptr;
		componentMap.erase(typeIndex);

		componentRemoved.dispatch(*this);
		return retVal;
	} else {
		return std::shared_ptr<ashley::Component>();
	}
}
