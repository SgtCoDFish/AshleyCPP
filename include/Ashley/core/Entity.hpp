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

#include <bitset>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/core/Component.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/internal/ComponentOperations.hpp"
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
	 * <p>Adds a component via a unqiue ptr to an existing component via moving.</p>
	 *
	 * <p>In short, if you create a component stored in a std::unique_ptr, you want to call: <br>
	 * <tt>entity->add(std::move(component));</tt><br>
	 * and remember not to use the component after moving it; use getComponent.
	 * </p>
	 * @param component the component to add.
	 * @return this {@link Entity} for chaining.
	 */
	Entity &add(std::unique_ptr<Component> &&component);

	/**
	 * <p>Constructs a new object of type C (subclassing ashley::Component) using <em>args...</em> for construction.</p>
	 *
	 * <p>If a {@link Component} of the same type already exists, it'll be replaced and destroyed without being retrievable
	 * from this class again.</p>
	 * @return This {@link Entity} for easy chaining
	 */
	template<typename C, typename ...Args> Entity &add(Args&&... args) {
		auto mapPtr = std::unique_ptr<Component>(new C(args...));

		if (operationHandler != nullptr) {
			operationHandler->add(this, mapPtr);
		} else {
			addInternal(mapPtr);
		}

		return *this;
	}

	/**
	 * <p>Removes a {@link Component} by its type_index.</p>
	 * @param typeIndex the type index of the component to remove
	 * @return the removed {@link Component}'s unique_ptr if the component was removed straight away; or nullptr if not
	 * found or if the removal has been delayed (e.g. when we're already in update() and need to wait to the end)
	 */
	std::unique_ptr<Component> remove(const std::type_index typeIndex);

	/**
	 * <p>Removes the given {@link Component}, if it's found attached to this {@link Entity}.
	 * @param component the {@link Component} to remove.
	 * @return the removed {@link Component}'s unique_ptr if the component was removed straight away; or nullptr if not
	 * found or if the removal has been delayed (e.g. when we're already in update() and need to wait to the end)
	 */
	std::unique_ptr<Component> remove(Component * const component);

	/**
	 * <p>Removes the {@link Component} of the specified type. Since there is only ever one component of one type, we
	 * don't need an instance, just the type.</p>
	 * @return A the removed {@link Component}, or a null shared_ptr if the Entity did not contain such a component.
	 */
	template<typename C> std::unique_ptr<Component> remove() {
		const auto typeIndex = std::type_index(typeid(C));
		const auto typeID = ashley::ComponentType::getIndexFor<C>();

		if (componentBits[typeID] == true) {
			return remove(typeIndex);
		} else {
			return std::unique_ptr<Component>(nullptr);
		}
	}

	/**
	 * <p>Removes all the {@link Component}'s from the Entity.</p>
	 * <p>Note that they'll all be destroyed and unretrievable by doing this.</p>
	 */
	void removeAll();

	/**
	 * <p>Retrieves a list of naked, safe-to-use, pointers to the {@link Component}s attached to this {@link Entity}.</p>
	 * <p>Note that this function creates a new vector and populates it with pointers referencing the original
	 * unique_ptrs that the object stores internally. This is slow, and this function probably shouldn't be used often.</p>
	 *
	 * @return vector of all this Entity's {@link Component} pointers.
	 */
	std::vector<Component *> getComponents() const;

	/**
	 * @return The Entity's unique index.
	 */
	inline uint64_t getIndex() const {
		return index;
	}

	/**
	 * @return a naked pointer to the specified {@link Component} in this object, or nullptr if the {@link Entity} has
	 * no such component.
	 */
	template<typename C> C* getComponent() {
		auto type = std::type_index(typeid(C));
		auto id = ashley::ComponentType::getIndexFor(type);

		return (componentBits[id] ? (C *) (componentMap[type].get()) : nullptr);
	}

	/**
	 * @return Whether or not the {@link Entity} already has a {@link Component} of the specified type.
	 */
	template<typename C> bool hasComponent() const {
		return componentBits[ashley::ComponentType::getIndexFor(std::type_index(typeid(C)))];
	}

	/**
	 * @return A const reference to this Entity's component bits, describing all the {@link Component}s it contains.
	 */
	const ashley::BitsType& getComponentBits() const;

	/**
	 * @return The number of components attached to this {@link Entity}.
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

	std::unordered_map<std::type_index, std::unique_ptr<Component>> componentMap;

	ashley::BitsType componentBits;
	ashley::BitsType familyBits;

	ComponentOperationHandler *operationHandler = nullptr;

	void addInternal(std::unique_ptr<Component> &component);

	std::unique_ptr<Component> removeImpl(std::type_index typeIndex);

	/**
	 * Actually processes the removal of a {@link Component} from this {@link Entity}.
	 * @param componentType the component to remove
	 * @return the component removed or nullptr if not removed
	 */
	std::unique_ptr<Component> removeInternal(std::type_index typeIndex);

	friend class ComponentOperationHandler;
};

}

#endif /* ENTITY_HPP_ */
