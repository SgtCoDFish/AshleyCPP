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

#include <iostream>
#include <vector>
#include <bitset>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <utility>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/signals/Signal.hpp"
#include "Ashley/internal/ComponentOperations.hpp"

namespace ashley {
/**
 * Simple containers of {@link Component}s that hold data. The component's data
 * is then processed by {@link EntitySystem}s.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Entity {
	friend class Engine;

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
	Entity(const Entity &other) = default;
	Entity(Entity &&other) = default;
	Entity& operator=(const Entity &other) = default;
	Entity& operator=(Entity &&other) = default;

	/**
	 * <p>Adds a component via a shared ptr to an existing component.</p>
	 * @param component the component to add.
	 * @return this {@link Entity} for chaining.
	 */
	Entity &add(std::shared_ptr<Component> component);

	/**
	 * <p>Constructs a new object of type C (subclassing ashley::Component) using args for construction.</p>
	 *
	 * <p>If a {@link Component} of the same type already exists, it'll be replaced without being retrievable from this class again.</p>
	 * @return This Entity for easy chaining
	 */
	template<typename C, typename ...Args> Entity &add(Args&&... args) {
//		auto type = std::type_index(typeid(C));

		auto mapPtr = std::make_shared<C>(args...);
		std::shared_ptr<ashley::Component> comPtr = std::dynamic_pointer_cast<ashley::Component>(mapPtr);

		if (operationHandler != nullptr) {
			operationHandler->add(this, comPtr);
		} else {
			addInternal(comPtr);
		}

		return *this;
	}

	/**
	 * <p>Removes a {@link Component} by its type_index.</p>
	 * @param typeIndex the type index of the component to remove
	 * @return a shared_ptr to the removed component or nullptr if it was not found.
	 */
	std::shared_ptr<ashley::Component> remove(std::type_index typeIndex);

	/**
	 * <p>Removes the given {@link Component}, if it's found attached to this {@link Entity}.
	 * @param component the {@link Component} to remove.
	 * @return the removed {@link Component} or nullptr if not found.
	 */
	std::shared_ptr<ashley::Component> remove(std::shared_ptr<ashley::Component> &component);

	/**
	 * <p>Removes the {@link Component} of the specified type. Since there is only ever one component of one type, we
	 * don't need an instance, just the type.</p>
	 * @return A shared_ptr to the removed {@link Component}, or a null shared_ptr if the Entity did not contain such a component.
	 */
	template<typename C> std::shared_ptr<ashley::Component> remove() {
		auto typeIndex = std::type_index(typeid(C));
		std::cout << "In remove: type is " << typeIndex.name() << ".\n";
		std::cout.flush();
		auto typeID = ashley::ComponentType::getIndexFor<C>();

		if (componentBits[typeID] == true) {
			return remove(typeIndex);
		} else {
			return std::shared_ptr<ashley::Component>(nullptr);
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

	/**
	 * @return a bitset used for checking the membership of this {@link Entity} in various {@link Family}s.
	 * Used internally; change at your peril.
	 */
	inline ashley::BitsType &getFamilyBits() {
		return familyBits;
	}

	bool operator==(const ashley::Entity &other) const {
		return this->index == other.index;
	}

	bool operator!=(const ashley::Entity &other) const {
		return this->index != other.index;
	}

	bool operator<(const ashley::Entity &other) const {
		return this->index < other.index;
	}

	bool operator<=(const ashley::Entity &other) const {
		return this->index <= other.index;
	}

	bool operator>(const ashley::Entity &other) const {
		return this->index > other.index;
	}

	bool operator>=(const ashley::Entity &other) const {
		return this->index >= other.index;
	}
private:
	static uint64_t nextIndex;

	uint64_t index;

	std::unordered_map<std::type_index, std::shared_ptr<ashley::Component>> componentMap;

	ashley::BitsType componentBits;
	ashley::BitsType familyBits;

	ComponentOperationHandler *operationHandler = nullptr;

	Entity &addInternal(std::shared_ptr<ashley::Component> component);

	std::shared_ptr<ashley::Component> removeImpl(std::shared_ptr<Component> &component);

	/**
	 * Actually processes the removal of a {@link Component} from this {@link Entity}.
	 * @param componentType the component to remove
	 * @return the component removed or nullptr if not removed
	 */
	std::shared_ptr<ashley::Component> removeInternal(std::shared_ptr<Component> &component);

	friend class ComponentOperationHandler;
};

}

#endif /* ENTITY_HPP_ */
