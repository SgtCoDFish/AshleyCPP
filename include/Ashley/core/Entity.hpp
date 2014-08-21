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
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <utility>

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
	ashley::Signal<Entity> componentAdded;

	/** Will dispatch an event when a component is removed. */
	ashley::Signal<Entity> componentRemoved;

	/**
	 * Creates an empty Entity.
	 */
	Entity();

	~Entity() = default;

	/**
	 * <p>Constructs a new object of type C (subclassing ashley::Component) using args for construction.</p>
	 *
	 * <p>If a {@link Component} of the same type already exists, it'll be replaced without being retrievable from this class again.</p>
	 * @return This Entity for easy chaining
	 */
	template<typename C, typename ...Args> Entity &add(Args&&... args) {
		auto type = std::type_index(typeid(C));
		auto typeID = ashley::ComponentType::getIndexFor(type);

		auto mapPtr = std::make_shared<C>(args...);

		if (componentBits[typeID]) {
			componentMap[type] = nullptr;
		}

		componentBits[typeID] = true;
		componentMap[type] = mapPtr;

		componentAdded.dispatch(*this);
		return *this;
	}

	/**
	 * <p>Removes the {@link Component} of the specified type. Since there is only ever one component of one type, we
	 * don't need an instance, just the type.</p>
	 * @return A shared_ptr to the removed {@link Component}, or a null shared_ptr if the Entity did not contain such a component.
	 */
	template<typename C> std::shared_ptr<C> remove() {
		auto typeIndex = std::type_index(typeid(C));
		auto id = ashley::ComponentType::getIndexFor(typeIndex);

		if (componentBits[id] == true) {
			componentBits[id] = false;
			auto retVal = std::dynamic_pointer_cast<C>(
					std::shared_ptr<ashley::Component>(componentMap[typeIndex]));
			componentMap[typeIndex] = nullptr;
			componentMap.erase(typeIndex);

			componentRemoved.dispatch(*this);
			return retVal;
		} else {
			return std::shared_ptr<C>();
		}
	}

	/**
	 * <p>Removes all the {@link Component}'s from the Entity.</p>
	 */
	void removeAll();

	/**
	 * @return const version of the map of all this Entity's {@link Component} pointers.
	 */
	const std::vector<std::shared_ptr<ashley::Component>> getComponents() const;

	/**
	 * @return The Entity's unique index.
	 */
	inline uint64_t getIndex() const {
		return index;
	}

	/**
	 * @return a shared_ptr to the specified component type in this object, or a null shared_ptr if the Entity has no such component.
	 */
	template<typename C> std::shared_ptr<C> getComponent() {
		auto type = std::type_index(typeid(C));
		auto id = ashley::ComponentType::getIndexFor(type);

		return (componentBits[id] ?
				std::dynamic_pointer_cast<C>(componentMap[type]) : std::shared_ptr<C>());
	}

	/**
	 * @return a weak_ptr to the specified component type in this object, or a null weak_ptr if the Entity has no such component.
	 */
	template<typename C> std::weak_ptr<C> getComponentWeak() {
		auto type = std::type_index(typeid(C));
		auto id = ashley::ComponentType::getIndexFor(type);

		return (componentBits[id] ?
				std::weak_ptr<C>(std::dynamic_pointer_cast<C>(componentMap[type])) :
				std::weak_ptr<C>());
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
	const ashley::BitsType& getComponentBits() const;

	/**
	 * @return The number of components attached to this Entity.
	 */
	inline unsigned int countComponents() const {
		return componentMap.size();
	}

	inline ashley::BitsType &getFamilyBits() {
		return familyBits;
	}

	const bool operator==(const ashley::Entity &other) const {
		return this->index == other.index;
	}

	const bool operator!=(const ashley::Entity &other) const {
		return this->index != other.index;
	}

	const bool operator<(const ashley::Entity &other) const {
		return this->index < other.index;
	}

	const bool operator<=(const ashley::Entity &other) const {
		return this->index <= other.index;
	}

	const bool operator>(const ashley::Entity &other) const {
		return this->index > other.index;
	}

	const bool operator>=(const ashley::Entity &other) const {
		return this->index >= other.index;
	}
private:
	static uint64_t nextIndex;

	uint64_t index;

	std::unordered_map<std::type_index, std::shared_ptr<ashley::Component>> componentMap;

	ashley::BitsType componentBits;
	ashley::BitsType familyBits;
};

}

#endif /* ENTITY_HPP_ */
