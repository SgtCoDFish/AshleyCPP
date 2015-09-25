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

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/EntitySystem.hpp"
#include "Ashley/core/EntityListener.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/internal/ComponentOperations.hpp"
#include "Ashley/signals/Listener.hpp"
#include "Ashley/signals/Signal.hpp"
#include "Ashley/util/ObjectPools.hpp"

namespace ashley {
class Component;
class EntitySystem;
class Entity;
class Family;

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
 * <p>Engine has copy construction deleted but allows move construction and assignment. As of the first release,
 * allowing copy construction probably wouldn't break anything, but supporting copy construction might be expensive
 * in the future if more memory management is required.</p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Engine {
public:
	Engine();
	~Engine();

	Engine(const Engine &other) = delete;
	Engine(Engine &&other) = default;
	Engine& operator=(const Engine &other) = delete;
	Engine& operator=(Engine &&other) = default;

	/**
	 * <p>Adds an std::unique_ptr to an {@link Entity} to this {@link Engine} via moving.
	 * Note that the added unique_ptr is owned by the engine after a call to this function,
	 * and trying to use the value you moved in is undefined behaviour
	 * (and probably a segfault).</p>
	 *
	 * @return a (safe-to-use) naked pointer to the {@link Entity} in the {@link Engine}.
	 */
	Entity *addEntity(std::unique_ptr<Entity> &&ptr);

	/**
	 * <p>Constructs a new {@link Entity} owned by this {@link Engine} and returns a pointer to it.</p>
	 * @return a pointer to the created {@link Entity}.
	 */
	Entity *addEntity();

	/**
	 * <p>Removes an {@link Entity} from this {@link Engine} via a pointer to the {@link Entity}.</p>
	 *
	 * <p>Note that the {@link Entity} (and therefore all attached {@link Component}s) will be destroyed.</p>
	 */
	void removeEntity(Entity * const ptr);

	/**
	 * Removes all entities registered with this Engine.
	 */
	void removeAllEntities();

	/**
	 * Adds the {@link EntitySystem} to this Engine via a std::unique_ptr with moving.
	 *
	 * <p>Note that once added, ownership is transferred to the Engine, and you need to use {@link Engine#getSystem} to access it or use the returned naked pointer.</p>
	 *
	 * @return a naked (safe-to-use) pointer to the added system)
	 */
	EntitySystem *addSystem(std::unique_ptr<EntitySystem> &&system);

	/**
	 * <p>Creates and adds a new system based on the templated type.</p>
	 * @param args arguments forwarded to the constructor of the system.
	 * @return a naked (safe-to-use) pointer to the created system.
	 */
	template<typename ES, typename ...Args> ES *addSystem(Args&&... args) {
		return (ES *) addSystem(std::unique_ptr<ES>(new ES(args...)));
	}

	/**
	 * <p>Removes the given {@link EntitySystem} from this {@link Engine}.</p>
	 * <p>Note that the system might be (is likely to be) immediately destroyed upon calling this function, and thus
	 * any state contained in the system will be lost.</p>
	 */
	void removeSystem(EntitySystem * const system);

	/**
	 * <p>Removes the system associated with the given type from this {@link Engine}.</p>
	 * <p>Note that the system might be (is likely to be) immediately destroyed upon calling this function, and thus
	 * any state contained in the system will be lost.</p>
	 */
	void removeSystem(std::type_index systemType);

	/**
	 * <p>Quick {@link EntitySystem} retrieval. Will require a type-cast on the returned {@link EntitySystem}.</p>
	 * <p>To avoid a typecast, use the templated, argument-less version.</p>
	 * @return A naked pointer to the system if it exists in the {@link Engine} or nullptr otherwise.
	 */
	EntitySystem * getSystem(std::type_index systemType) const;

	/**
	 * <p>Quick {@link EntitySystem} retrieval. Doesn't require type-casts thanks to template magic.</p>
	 * @return A naked pointer to the system if it exists in the {@link Engine} or nullptr otherwise.
	 */
	template<typename ES> ES *getSystem() {
		// duplicates some code with the type_index version, but faster this way.
		auto ret = systemsByClass.find(typeid(ES));
		return (ES*) (ret != systemsByClass.end() ? (*ret).second : nullptr);
	}

	/**
	 * @return all the systems currently attached to this {@link Engine}. Note that this creates and populates a new
	 * 		   vector and is therefore slow and not advisable for regular use.
	 */
	const std::vector<EntitySystem *> getSystems() const;

	/**
	 * <p>(Quickly) returns vector of {@link Entity} pointers for {@link Entity}s matching the
	 * specified {@link Family}.</p>
	 * @return a (possibly empty) vector of naked {@link Entity} pointers.
	 */
	std::vector<Entity *> *getEntitiesFor(Family * const family);

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

	static bool systemPriorityComparator(const std::unique_ptr<EntitySystem> &one,
	        const std::unique_ptr<EntitySystem> &other);

private:
	std::vector<std::unique_ptr<Entity>> entities;
	std::unordered_map<Family, std::vector<Entity *>> families;

	std::vector<std::unique_ptr<EntitySystem>> systems;
	std::unordered_map<std::type_index, EntitySystem *> systemsByClass;

	std::vector<ashley::EntityListener *> listeners;
	std::vector<ashley::EntityListener *> removalPendingListeners;

	std::vector<Entity *> pendingRemovalEntities;

	bool notifying;
	bool updating;

	ObjectPool<ComponentOperation> operationPool;
	std::vector<ComponentOperation *> operationVector;

	void updateFamilyMembership(ashley::Entity &entity);

	void processComponentOperations();

	void removePendingListeners();

	void removePendingEntities();
	void removeEntityInternal(Entity * const entity);

	friend class AddedListener;
	friend class RemovedListener;
	friend class EngineOperationHandler;

	class AddedListener : public ashley::Listener<Entity> {
	public:
		AddedListener(Engine * const engine) :
				        engine(engine) {
		}

		virtual void receive(ashley::Signal<ashley::Entity> * const signal, ashley::Entity *object) override {
			engine->updateFamilyMembership(*object);
		}

	private:
		Engine *engine = nullptr;
	};

	class RemovedListener : public ashley::Listener<Entity> {
	public:
		RemovedListener(Engine * const engine) :
				        engine(engine) {
		}

		virtual void receive(ashley::Signal<ashley::Entity> * const signal, ashley::Entity *object) override {
			engine->updateFamilyMembership(*object);
		}

	private:
		Engine *engine = nullptr;
	};

	class EngineOperationHandler : public ashley::ComponentOperationHandler {
	public:
		EngineOperationHandler(Engine *engine) :
				        engine(engine) {
		}
		virtual ~EngineOperationHandler() {
		}

		virtual void add(ashley::Entity * const entity, std::unique_ptr<Component> &&component,
		        const std::type_index typeIndex) override;
		virtual void remove(ashley::Entity * const entity, const std::type_index typeIndex) override;

	private:
		Engine *engine = nullptr;
	};

	std::unique_ptr<ashley::Listener<Entity>> componentAddedListener;
	std::unique_ptr<ashley::Listener<Entity>> componentRemovedListener;

	std::unique_ptr<EngineOperationHandler> operationHandler;
};
}

#endif /* ENGINE_HPP_ */
