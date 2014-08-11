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

#include <unordered_map>
#include <vector>
#include <bitset>
#include <typeindex>
#include <memory>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/signals/Signal.hpp"

namespace ashley {
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
	ashley::Signal<Entity *> componentAdded;

	/** Will dispatch an event when a component is removed. */
	ashley::Signal<Entity *> componentRemoved;

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
	template<typename C> C *remove() {
		C *retVal = nullptr;

		auto typeIndex = std::type_index(typeid(C));
		auto id = ashley::ComponentType::getIndexFor(typeIndex);

		if (componentBits[id] == true) {
			retVal = dynamic_cast<C*>(components[typeIndex]);
			components.erase(typeIndex);
			componentRemoved.dispatch(this);
		}

		componentBits[id] = false;

		return retVal;
	}

	/**
	 * Removes all the {@link Component}'s from the Entity.
	 */
	void removeAll();

	/**
	 * @return const version of the map of all this Entity's {@link Component} pointers.
	 */
	std::unordered_map<std::type_index, ashley::Component *> getComponents() const;

	/**
	 * @return const vector of all components; this requires converting the stored map implementation into a vector, so could be slow. Don't call every frame!
	 */
	const std::vector<ashley::Component *> getComponentsVector() const;

	/**
	 * @return The Entity's unique index.
	 */
	inline uint64_t getIndex() const {
		return index;
	}

	template<typename C> C *getComponent() const {
		return dynamic_cast<C*>(components.at(std::type_index(typeid(C))));
	}

	/**
	 * @return Whether or not the Entity already has a {@link Component} for the specified type.
	 */
	template<typename C> bool hasComponent() const {
		return componentBits[ashley::ComponentType::getIndexFor(std::type_index(typeid(C)))];
	}

	/**
	 * @return A const reference to this Entity's component bits, describing all the {@link Component}s it contains.
	 */
	const std::bitset<ASHLEY_MAX_COMPONENT_COUNT>& getComponentBits() const;

//	/**
//	 * @return The number of components attached to this Entity.
//	 */
//	inline unsigned int countComponents() const {
//		return components.size();
//	}
private:
	static uint64_t nextIndex;

	uint64_t index;

	std::unordered_map<std::type_index, ashley::Component *> components;

	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> componentBits;
	std::bitset<ASHLEY_MAX_COMPONENT_COUNT> familyBits;
};

}

#endif /* ENTITY_HPP_ */
