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

#ifndef INTERVALITERATINGSYSTEM_HPP_
#define INTERVALITERATINGSYSTEM_HPP_

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
	std::shared_ptr<ashley::Family> family;
	std::vector<std::shared_ptr<ashley::Entity>> *entities = nullptr;

public:

	/**
	 * <p>Creates an {@link IntervalIteratingSystem} which iterates over the {@link Entity}s
	 * matching <em>family</em>
	 * @param family the {@link Family} to match with.
	 * @param interval the interval, in seconds, after which {@link IntervalIteratingSystem#processEntity}
	 * will be called for each matched {@link Entity}.
	 */
	IntervalIteratingSystem(std::shared_ptr<ashley::Family> family, float interval);

	/**
	 * <p>Creates an {@link IntervalIteratingSystem} which iterates over the {@link Entity}s
	 * matching <em>family</em>
	 * @param family the {@link Family} to match with.
	 * @param interval the interval, in seconds, after which {@link IntervalIteratingSystem#processEntity}
	 * will be called for each matched {@link Entity}.
	 * @param priority the system's priority; lower priorities execute first.
	 */
	IntervalIteratingSystem(std::shared_ptr<ashley::Family> family, float interval, uint64_t priority);

	void addedToEngine(Engine &engine) override;
	void removedFromEngine(Engine &engine) override;

protected:
	void updateInterval() override;

	virtual void processEntity(std::shared_ptr<ashley::Entity> &entity) = 0;
};

}

#endif /* INTERVALITERATINGSYSTEM_HPP_ */
