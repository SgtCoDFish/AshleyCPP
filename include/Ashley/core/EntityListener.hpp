/*******************************************************************************
 * Copyright 2014, 2015 See AUTHORS file.
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

#ifndef ACPP_CORE_ENTITYLISTENER_HPP_
#define ACPP_CORE_ENTITYLISTENER_HPP_

#include "Ashley/AshleyConstants.hpp"

namespace ashley {
class Entity;

/**
 * <p>Gets notified of {@link Entity} related events.</p>
 *
 * <em>Java author: David Saltares</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class EntityListener {
public:
	EntityListener() {
	}

	virtual ~EntityListener() = default;
	EntityListener(const EntityListener &other) = default;
	EntityListener(EntityListener &&other) = default;
	EntityListener& operator=(const EntityListener &other) = default;
	EntityListener& operator=(EntityListener &&other) = default;

	virtual void entityAdded(ashley::Entity &entity) = 0;
	virtual void entityRemoved(ashley::Entity &entity) = 0;
};
}

#endif /* ENTITYLISTENER_HPP_ */
