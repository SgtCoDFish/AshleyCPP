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

#include "Ashley/core/Family.hpp"

#include <typeinfo>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Ashley/signals/Listener.hpp"

namespace ashley {
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
	 * <p>Removes an {@link Entity} from this {@link Engine} via an shared_ptr.</p>
	 *
	 * <p>Note that if no external pointers are maintained, the {@link Entity} will be destroyed immediately.</p>
	 */
	void removeEntity(std::shared_ptr<ashley::Entity> ptr);

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
	 *
	 * <p>Passing the address of a heap-allocated {@link EntitySystem} will cause a segfault when the system is destroyed.
	 * Use new EntitySystem() instead. For safety, pass an rvalue into this function and then use getSystem to get a
	 * safe shared_ptr to it.</p>
	 */
	void addSystem(ashley::EntitySystem *system);

	/**
	 * Adds the {@link EntitySystem} to this Engine via an std::shared_ptr.
	 *
	 * <p>Note that once added, ownership is transferred to the Engine. For access to the {@link EntitySystem} after
	 * adding, use {@link Engine#addSystemAndGet}.</p>
	 */
	void addSystem(std::shared_ptr<ashley::EntitySystem> system);

	/**
	 * <p>Adds an {@link EntitySystem} to this Engine and returns an std::shared_ptr to it.</p>
	 * <p>Passing the address of a heap-allocated {@link EntitySystem} will cause a segfault when the system is destroyed.
	 * Use new EntitySystem() instead. For safety, pass an rvalue into this function and then use getSystem to get a
	 * safe shared_ptr to it.</p>
	 */
	std::shared_ptr<ashley::EntitySystem> addSystemAndGet(ashley::EntitySystem *system);

	/**
	 * <p>Removes the given {@link EntitySystem} from this {@link Engine}.</p>
	 * <p>Note that the argument passed could be the only remaining reference to the system and if it is the last,
	 * the system might be immediately destroyed.</p>
	 */
	void removeSystem(std::shared_ptr<ashley::EntitySystem> system);

	/**
	 * <p>Removes the system associated with the given type from this {@link Engine}.</p>
	 * <p>Note that this could destroy the system if no shared_ptrs are maintained to it.</p>
	 */
	void removeSystem(std::type_index systemType);

	/**
	 * <p>Quick {@link EntitySystem} retrieval. Will require a type-cast on the returned {@link EntitySystem}.</p>
	 * <p>To avoid a typecast, use the templated, argumentless version.</p>
	 * @return A shared_ptr to the system if it exists in the system or a shared_ptr to nullptr otherwise.
	 */
	std::shared_ptr<ashley::EntitySystem> getSystem(std::type_index systemType) const;

	/**
	 * <p>Quick {@link EntitySystem} retrieval. Doesn't require type-casts thanks to templates.</p>
	 * @return A shared_ptr to the system if it exists in the system or a shared_ptr to nullptr otherwise.
	 */
	template<typename ES> std::shared_ptr<ES> getSystem() {
		// duplicates some code with the type_index version, but faster this way.
		auto ret = systemsByClass.find(typeid(ES));
		return (ret != systemsByClass.end() ?
				std::dynamic_pointer_cast<ES>(
						std::shared_ptr<ashley::EntitySystem>((*ret).second)) :
				std::shared_ptr<ES>());
	}

	/**
	 * <p>Returns const vector of {@link Entity}s for the specified {@link Family}.</p>
	 */
	std::vector<std::shared_ptr<ashley::Entity>> getEntitiesFor(ashley::Family &family);

	/**
	 * Adds an {@link EntityListener}.
	 */
	void addEntityListener(ashley::EntityListener *listener);

	/**
	 * Removes an {@link EntityListener}.
	 */
	void removeEntityListener(ashley::EntityListener *listener);

	/**
	 * Updates all the systems in this Engine.
	 * @param deltaTime The time passed since the last frame.
	 */
	void update(float deltaTime);

	static bool systemPriorityComparator(std::shared_ptr<ashley::EntitySystem> &one,
			std::shared_ptr<ashley::EntitySystem> &other);

private:
	std::vector<std::shared_ptr<ashley::Entity>> entities;

	std::vector<std::shared_ptr<ashley::EntitySystem>> systems;
	std::unordered_map<std::type_index, std::shared_ptr<ashley::EntitySystem>> systemsByClass;

	std::unordered_map<ashley::Family, std::vector<std::shared_ptr<ashley::Entity>>>families;
//	std::unordered_map<ashley::Family, const std::vector<std::shared_ptr<ashley::Entity>>> immutableFamilies;

	std::vector<ashley::EntityListener *> listeners;
	std::vector<ashley::EntityListener *> removalPendingListeners;

	ashley::Listener<ashley::Entity> *componentAddedListener;
	ashley::Listener<ashley::Entity> *componentRemovedListener;

	bool notifying;

	void componentAdded(std::shared_ptr<ashley::Entity> entity);
	void componentRemoved(std::shared_ptr<ashley::Entity> entity);
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
