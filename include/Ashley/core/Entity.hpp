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
#include <memory>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/signals/Signal.hpp"

namespace ashley {

class Component;
class ComponentType;

/**
 * Simple containers of {@link Component}s that hold data. The component's data
 * is then processed by {@link EntitySystem}s.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Entity {
public:
	/** A flag that can be used to bit mask this entity. Up to the user to manage. */
	uint64_t flags = 0;

	/** Will dispatch an event when a component is added. */
	ashley::Signal<ashley::Entity> componentAdded;

	/** Will dispatch an event when a component is removed. */
	ashley::Signal<ashley::Entity> componentRemoved;

	/**
	 * Creates an empty Entity.
	 */
	Entity();

	~Entity();

	/**
	 * Adds a {@link Component} to this Entity. If a {@link Component} of the same type already exists, it'll be replaced.
	 * @return The Entity for easy chaining
	 */
	Entity& add(Component &component);

	/**
	 * Removes the {@link Component} of the specified type. Since there is only ever one component of one type, we
	 * don't need an instance, just the type.
	 * @return A pointer to the removed {@link Component}, or nullptr if the Entity did no contain such a component.
	 */
	ashley::Component *remove(const std::type_index type);

	/**
	 * Removes all the {@link Component}'s from the Entity.
	 */
	void removeAll();

	/**
	 * @return const iteratove over all this Entity's {@link Component}s.
	 */
	std::vector<ashley::Component>::const_iterator getComponents() const;

	/**
	 * @return The Entity's unique index.
	 */
	inline uint64_t getIndex() const {
		return index;
	}

	/**
	 * @return Whether or not the Entity already has a {@link Component} for the specified type.
	 */
	bool hasComponent(ashley::ComponentType type) const;

	/**
	 * @return A const reference to this Entity's component bits, describing all the {@link Component}s it contains.
	 */
	const std::bitset<ASHLEY_MAX_COMPONENT_COUNT>& getComponentBits() const;
private:
	static uint64_t nextIndex;

	uint64_t index;

	std::vector<ashley::Component> components;

	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> componentBits;
	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> familyBits;
};

}

#endif /* ENTITY_HPP_ */
