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

#include <algorithm>

#include <typeindex>

#include "Ashley/core/Entity.hpp"
#include "Ashley/core/Component.hpp"

uint_fast64_t ashley::Entity::nextIndex = 0;

ashley::Entity::Entity() {
	index = nextIndex++;
}

ashley::Entity::~Entity() {
	components.clear();
}

ashley::Entity& ashley::Entity::add(ashley::Component &component) {
	auto typeIndex = std::type_index(typeid(component));
	auto componentType = ComponentType::getFor(typeIndex);
	auto typeID = componentType.getIndex();

	if (componentBits.test(typeID)) {
		std::replace_if(components.begin(), components.end(),
				[&](ashley::Component &c) {return std::type_index(typeid(c)) == typeIndex;},
				component);
	}

	componentBits.set(typeID, true);

	return *this;
}

ashley::Entity& ashley::Entity::remove(std::type_index index) {
	auto componentType = ComponentType::getFor(index);
	//auto typeID = componentType.getIndex();

	return *this;
}
