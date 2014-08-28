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

#ifndef COMPONENTMAPPER_HPP_
#define COMPONENTMAPPER_HPP_

#include <typeindex>
#include <typeinfo>
#include <memory>

#include "Ashley/core/Entity.hpp"
#include "Ashley/core/ComponentType.hpp"

namespace ashley {

/**
 * <p>Provides super fast {@link Component} retrieval from {@Link Entity} objects.</p>
 *
 * @param <T> the class type of the {@link Component}.
 *
 * @author David Saltares
 */
template<typename T> class ComponentMapper {
public:
	~ComponentMapper() = default;
	ComponentMapper(const ComponentMapper &other) = default;
	ComponentMapper(ComponentMapper &&other) = default;
	ComponentMapper& operator=(const ComponentMapper &other) = default;
	ComponentMapper& operator=(ComponentMapper &&other) = default;

	/**
	 * @param index Component class to be retrieved by the mapper.
	 * @return New instance that provides fast access to the {@link Component} of the specified class.
	 */
	static const ComponentMapper<T> getFor() {
		return ComponentMapper<T>(typeid(T));
	}

	static const ComponentMapper<T> getFor(std::type_index index) {
		return ComponentMapper<T>(index);
	}

	/**
	 * @return The {@link Component} of the specified class belonging to e.
	 */
	std::shared_ptr<T> get(std::shared_ptr<ashley::Entity> e) {
		return e->getComponent<T>();
	}

	/**
	 * @return Whether or not entity has the component of the specified class.
	 */
	bool has(std::shared_ptr<ashley::Entity> e) const {
		return e->hasComponent<T>();
	}

private:
	const ashley::ComponentType componentType;
	ComponentMapper(std::type_index componentClass) :
			componentType(ashley::ComponentType::getFor(componentClass)) {
	}
};
}

#endif /* COMPONENTMAPPER_HPP_ */
