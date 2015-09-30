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

#ifndef ACPP_SYSTEMS_INTERVALITERATINGSYSTEM_HPP_
#define ACPP_SYSTEMS_INTERVALITERATINGSYSTEM_HPP_

#include <vector>

#include "Ashley/systems/IntervalSystem.hpp"

namespace ashley {
class Entity;
class Engine;
class Family;

/**
 * <p>An {@link EntitySystem} which processing a {@link Family} of {@link Entity}s after the given
 * interval, in seconds, has elapsed. The logic for processing should go into an
 * overridden {@link IntervalIteratingSystem#processEntity} method. </p>
 *
 * <p><em>Java author: </em>David Saltares</em></p>
 * @author Ashley Davis (SgtCoDFish)
 */
class IntervalIteratingSystem : public ashley::IntervalSystem {
private:
	Family *family = nullptr;
	std::vector<Entity *> *entities = nullptr;

public:

	virtual ~IntervalIteratingSystem() = default;
	IntervalIteratingSystem(const IntervalIteratingSystem &other) = default;
	IntervalIteratingSystem(IntervalIteratingSystem &&other) = default;
	IntervalIteratingSystem& operator=(const IntervalIteratingSystem &other) = default;
	IntervalIteratingSystem& operator=(IntervalIteratingSystem &&other) = default;

	/**
	 * <p>Creates an {@link IntervalIteratingSystem} which iterates over the {@link Entity}s
	 * matching <em>family</em>
	 * @param family the {@link Family} to match with.
	 * @param interval the interval, in seconds, after which {@link IntervalIteratingSystem#processEntity}
	 * will be called for each matched {@link Entity}.
	 */
	IntervalIteratingSystem(Family *family, float interval);

	/**
	 * <p>Creates an {@link IntervalIteratingSystem} which iterates over the {@link Entity}s
	 * matching <em>family</em>
	 * @param family the {@link Family} to match with.
	 * @param interval the interval, in seconds, after which {@link IntervalIteratingSystem#processEntity}
	 * will be called for each matched {@link Entity}.
	 * @param priority the system's priority; lower priorities execute first.
	 */
	IntervalIteratingSystem(Family *family, float interval, uint64_t priority);

	void addedToEngine(Engine &engine) override;
	void removedFromEngine(Engine &engine) override;

	Family *getFamily() const {
		return family;
	}

protected:
	void updateInterval() override;

	virtual void processEntity(Entity * const &entity) = 0;
};

}

#endif /* INTERVALITERATINGSYSTEM_HPP_ */
