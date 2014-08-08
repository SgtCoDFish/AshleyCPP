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

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <cstdint>

#include <vector>
#include <bitset>
#include <typeindex>

#include "Ashley/AshleyCore.hpp"

namespace ashley {

class Component;

/**
 * <strong>Java Description</strong>
 * Simple containers of {@link Component}s that give them "data". The component's data
 * is then processed by {@link EntitySystem}s.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Entity {
public:
	uint64_t flags = 0;

	//TODO: Implement signals here

	Entity();

	~Entity();

	Entity& add(Component &component);
	Entity& remove(std::type_index type);

	inline uint64_t getIndex() const {
		return index;
	}

private:
	static uint_fast64_t nextIndex;

	uint64_t index;

	std::vector<ashley::Component> components;

	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> componentBits;
	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> familyBits;
};

}

#endif /* ENTITY_HPP_ */
