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

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <typeindex>
#include <vector>

namespace ashley {
class Family;
class Entity;
class EntitySystem;
class EntityListener;

/**
 * <p>The heart of the Entity framework. It is responsible for keeping track of {@link Entity} and
 * managing {@link EntitySystem} objects. The Engine should be updated every tick via the {@link #update(float)} method.</p>
 *
 * <p>With the Engine you can:
 *
 * <ul>
 * <li>Add/Remove {@link Entity} objects</li>
 * <li>Add/Remove {@link EntitySystem}s</li>
 * <li>Obtain a list of entities for a specific {@link Family}</li>
 * <li>Update the main loop</li>
 * <li>Register/unregister {@link EntityListener} objects</li>
 * </ul>
 * </p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Engine {
public:
	Engine();

	/**
	 * Adds an entity to this Engine.
	 */
	void addEntity(ashley::Entity &entity);

	/**
	 * Removes an entity from this Engine.
	 */
	void removeEntity(ashley::Entity &entity);

	/**
	 * Removes all entities registered with this Engine.
	 */
	void removeAllEntities();

	/**
	 * Adds the {@link EntitySystem} to this Engine.
	 */
	void addSystem(ashley::EntitySystem &system);

	/**
	 * Removes the {@link EntitySystem} from this Engine.
	 */
	void removeSystem(ashley::EntitySystem &system);

	/**
	 * Quick {@link EntitySystem} retrieval.
	 */
	EntitySystem& getSystem(std::type_index &systemType) const;

	/**
	 * Returns const vector of entities for the specified {@link Family}.
	 */
	const std::vector<ashley::Entity *> getEntitiesFor(ashley::Family &family);

	/**
	 * Adds an {@link EntityListener}
	 */
	void addEntityListener(ashley::EntityListener &listener);

	/**
	 * Removes an {@link EntityListener}
	 */
	void removeEntityListener(ashley::EntityListener &listener);

	/**
	 * Updates all the systems in this Engine.
	 * @param deltaTime The time passed since the last frame.
	 */
	void update(float deltaTime);
};
}

#endif /* ENGINE_HPP_ */
