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

#ifndef COMPONENTTYPE_HPP_
#define COMPONENTTYPE_HPP_

#include <cstdint>

#include <bitset>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

#include "Ashley/AshleyConstants.hpp"

namespace ashley {
class Component;

/**
 * <p>Uniquely identifies a {@link Component} sub-class, assigning them an index which is used internally for fast comparison and retrieval.</p>
 * <p>Functions which accept a type are overloaded to accept both std::type_index (preferred) and std::type_info. This means that a {@link Component}, c, type can be passed as either std::type_index(typeid(c)) or just as typeid(c).</p>
 * <p>Note that the std::type_info form is silently converted to a std::type_index anyway.</p>
 * See {@link Family} and {@link Entity}.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class ComponentType {
public:
	ComponentType();
	~ComponentType();

	inline uint64_t getIndex() const {
		return index;
	}

	/**
	 * @param componentType The {@link Component} class's type.
	 * @return A ComponentType matching the Component's class.
	 */
	static ComponentType& getFor(std::type_index componentType);

	/**
	 * <p>As with the std::type_index version.</p>
	 */
	static ComponentType& getFor(std::type_info &componentType) {
		return ComponentType::getFor(std::type_index(componentType));
	}

	/**
	 * <p>As with std::type_index version but using a templated type instead.</p>
	 */
	template<typename C> static ComponentType &getFor() {
		return ComponentType::getFor(std::type_index(typeid(C)));
	}

	/**
	 * Quick helper method. The same could be done via {@link ComponentType.getFor()}.
	 * @param componentType The {@link Component} class's type.
	 * @return The index for the specified {@link Component} Class
	 */
	static uint64_t getIndexFor(std::type_index componentType);

	/**
	 * <p>As with the std::type_index version but only needs a typeid(Component).</p>
	 */
	static uint64_t getIndexFor(std::type_info &componentType) {
		return ComponentType::getIndexFor(std::type_index(componentType));
	}

	/**
	 * <p>As with the std::type_index version but with a templated type instead of an argument</p>
	 */
	template<typename C> static uint64_t getIndexFor() {
		return ComponentType::getIndexFor(std::type_index(typeid(C)));
	}

	/**
	 * <p>Derives the bits for a list of component types passed as template arguments.</p>
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(BitsType, BitsType, BitsType)}.
	 */
	template<typename C, typename... CRest> ashley::BitsType getBitsFor() {
		return (sizeof...(CRest) > 0 ? (getBitsFor<CRest...>()).set(ashley::ComponentType::getIndexFor<C>()) : ashley::BitsType().set(ashley::ComponentType::getIndexFor<C>()));
	}

	/**
	 * @param components Initializer list of {@link Component} class std::type_indexes.
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(BitsType, BitsType, BitsType)}.
	 */
	static const ashley::BitsType getBitsFor(
			std::initializer_list<std::type_index> components);

	/**
	 * @param components Beginning and ending const iterators over a collection of std::type_indexes.
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(Bits, Bits, Bits)}.
	 */
	template<typename Iter, typename IterType = std::type_index> static const std::bitset<
	ASHLEY_MAX_COMPONENT_COUNT> getBitsFor(Iter first, Iter last);

	bool operator==(const ComponentType &other) const;
	bool operator!=(const ComponentType &other) const;
private:
	static uint64_t typeIndex;
	static std::unordered_map<std::type_index, ComponentType> componentTypes;

	uint64_t index;
};
}

#endif /* COMPONENTTYPE_HPP_ */
