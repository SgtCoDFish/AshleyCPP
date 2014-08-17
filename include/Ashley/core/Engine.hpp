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

#include <typeinfo>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <memory>

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
	 * <p>Adds an {@link Entity} to this {@link Engine}.</p>
	 *
	 * <p>Note that once added, ownership is transferred to the Engine. For access to the {@link Entity} after adding,
	 * use {@link Engine#addEntityAndGet} or use the overload which takes an std::shared_ptr.</p>
	 */
	void addEntity(ashley::Entity &entity);

	/**
	 * <p>Adds an std::shared_ptr to an {@link Entity} to this {@link Engine}.</p>
	 */
	void addEntity(std::shared_ptr<ashley::Entity> ptr);

	/**
	 * <p>Adds an {@link Entity} to this Engine and returns an std::shared_ptr to it.</p>
	 */
	std::shared_ptr<ashley::Entity> addEntityAndGet(ashley::Entity &entity);

	/**
	 * <p>Removes an {@link Entity} from this {@link Engine}.</p>
	 *
	 * <p>Note that if no external pointers have been maintained, the {@link Entity} will be destroyed immediately.</p>
	 */
	void removeEntity(ashley::Entity &entity);

	/**
	 * Removes an {@link Entity} from this {@link Engine} and returns it in a shared_ptr.
	 */
	std::shared_ptr<ashley::Entity> removeEntityAndGet(ashley::Entity &entity);

	/**
	 * Removes all entities registered with this Engine.
	 */
	void removeAllEntities();

	/**
	 * Adds the {@link EntitySystem} to this Engine.
	 *
	 * <p>Note that once added, ownership is transferred to the Engine. For access to the {@link EntitySystem} after
	 * adding, use {@link Engine#addSystemAndGet}.</p>
	 */
	void addSystem(ashley::EntitySystem &system);

	/**
	 * Adds the {@link EntitySystem} to this Engine via an std::shared_ptr.
	 *
	 * <p>Note that once added, ownership is transferred to the Engine. For access to the {@link EntitySystem} after
	 * adding, use {@link Engine#addSystemAndGet}.</p>
	 */
	void addSystem(std::shared_ptr<ashley::EntitySystem> system);

	/**
	 * <p>Adds an {@link EntitySystem} to this Engine and returns an std::shared_ptr to it.</p>
	 */
	std::shared_ptr<ashley::EntitySystem> addSystemAndGet(ashley::EntitySystem &system);

	/**
	 * <p>Removes the {@link EntitySystem} from this Engine.</p>
	 * <p>Note that if no external pointers have been maintained, the {@link EntitySystem} will be destroyed
	 * immediately. To preserve it, use {@link Engine#removeSystemAndGet}.</p>
	 */
	void removeSystem(ashley::EntitySystem &system);

	/**
	 * Removes an {@link EntitySystem} from this {@link Engine} and returns it in a shared_ptr.
	 */
	std::shared_ptr<ashley::EntitySystem> removeSystemAndGet(ashley::EntitySystem &system);

	/**
	 * <p>Quick {@link EntitySystem} retrieval.</p>
	 */
	std::weak_ptr<ashley::EntitySystem> getSystem(std::type_index &systemType) const;

	/**
	 * <p>Returns const vector of {@link Entity}s for the specified {@link Family}.</p>
	 */
	const std::vector<std::shared_ptr<ashley::Entity>> getEntitiesFor(ashley::Family &family) const;

	/**
	 * Adds an {@link EntityListener}.
	 */
	void addEntityListener(ashley::EntityListener &listener);

	/**
	 * Removes an {@link EntityListener}.
	 */
	void removeEntityListener(ashley::EntityListener &listener);

	/**
	 * Updates all the systems in this Engine.
	 * @param deltaTime The time passed since the last frame.
	 */
	void update(float deltaTime);

private:
	std::vector<std::shared_ptr<ashley::Entity>> entities;

	std::vector<std::shared_ptr<ashley::EntitySystem>> systems;
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<ashley::EntitySystem>>>systemsByClass;

	std::unordered_map<ashley::Family, std::vector<std::shared_ptr<ashley::Entity>>> families;
	std::unordered_map<ashley::Family, const std::vector<std::shared_ptr<ashley::Entity>>> immutableFamilies;

	std::vector<ashley::EntityListener *> listeners;
	std::vector<ashley::EntityListener *> removalPendingListeners;

	bool notifying;

	ashley::Listener<ashley::Entity> *componentAddedListener;
	ashley::Listener<ashley::Entity> *componentRemovedListener;

	void componentAdded(const ashley::Entity &entity);
	void componentRemoved(const ashley::Entity &entity);
	void removePendingListeners();

//	class AddedListener : public ashley::Listener<Entity> {
//		virtual void receieve(const ashley::Signal<ashley::Entity> &signal, const ashley::Entity &object) override {
//			componentAdded(object);
//		}
//	};
//
//	class RemovedListener : public ashley::Listener<Entity> {
//		virtual void receieve(const ashley::Signal<ashley::Entity> &signal, const ashley::Entity &object) override {
//			componentRemoved(object);
//		}
//	};
};
}

#endif /* ENGINE_HPP_ */
