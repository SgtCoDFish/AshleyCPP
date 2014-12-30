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

#ifndef ITERATINGSYSTEM_HPP_
#define ITERATINGSYSTEM_HPP_

#include <vector>

#include "Ashley/core/EntitySystem.hpp"

namespace ashley {
class Entity;
class Engine;
class Family;

/**
 * <p>A simple EntitySystem that iterates over each entity and calls processEntity() for each entity every time
 * the EntitySystem is updated. This is really just a convenience class as most systems iterate over a list
 * of entities.</p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class IteratingSystem : public ashley::EntitySystem {
public:
	/**
	 * Instantiates a system that will iterate over the entities described by the Family, with the default priority.
	 * @param family The family of entities iterated over in this System
	 */
	explicit IteratingSystem(Family *family) :
			IteratingSystem(family, ashley::EntitySystem::DEFAULT_PRIORITY) {
	}

	/**
	 * Instantiates a system that will iterate over the entities described by the Family, with a
	 * specific priority.
	 * @param family The family of entities iterated over in this System
	 * @param priority The priority to execute this system with (lower means higher priority)
	 */
	IteratingSystem(Family *family, uint64_t priority) :
			EntitySystem(priority), family(family), entities() {
	}

	virtual ~IteratingSystem() = default;
	IteratingSystem(const IteratingSystem &other) = default;
	IteratingSystem(IteratingSystem &&other) = default;
	IteratingSystem& operator=(const IteratingSystem &other) = default;
	IteratingSystem& operator=(IteratingSystem &&other) = default;

	virtual void addedToEngine(ashley::Engine &engine) override;
	virtual void removedFromEngine(ashley::Engine &engine) override;

	virtual void update(float deltaTime) override;

	/**
	 * This method is called on every entity on every update call of the EntitySystem. Override this to implement
	 * your system's specific processing.
	 * @param entity The current Entity being processed
	 * @param deltaTime The delta in time between the last and current frame
	 */
	virtual void processEntity(Entity * const &entity, float deltaTime) = 0;

	/**
	 * @return true if there is at least 1 {@link Entity} matching the family for this system.
	 */
	virtual bool checkProcessing() override;

protected:
	/**
	 * The family used by this IteratingSystem to retrieve its {@link Entity}s.
	 */
	Family *family = nullptr;

	/**
	 * An array of pointers to matching {@link Entity}s which is automatically updated every tick.
	 */
	std::vector<Entity *> *entities = nullptr;
};
}

#endif /* ITERATINGSYSTEM_HPP_ */
