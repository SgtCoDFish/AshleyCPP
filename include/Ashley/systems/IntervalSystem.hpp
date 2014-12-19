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

#ifndef INTERVALSYSTEM_HPP_
#define INTERVALSYSTEM_HPP_

#include <cstdint>

#include "Ashley/core/EntitySystem.hpp"

namespace ashley {

/**
 * <p>An {@link EntitySystem} which runs its logic every time
 * a given period of time (an <em>interval</em>) passes.</p>
 *
 * <p><em>Java author:</em> David Saltares</p>
 * @author Ashley Davis (SgtCoDFish)
 */
class IntervalSystem : public ashley::EntitySystem {
private:
	float interval;
	float accumulator;

public:
	virtual ~IntervalSystem() = default;
	IntervalSystem(const IntervalSystem &other) = default;
	IntervalSystem(IntervalSystem &&other) = default;
	IntervalSystem& operator=(const IntervalSystem &other) = default;
	IntervalSystem& operator=(IntervalSystem &&other) = default;

	/**
	 * <p>Creates an {@link IntervalSystem} with a default priority and the given interval.
	 * @param interval The amount of time, in seconds, that the system waits before it
	 * executes {@link IntervalSystem#updateInterval}.
	 */
	IntervalSystem(float interval);

	/**
	 * <p>Creates an {@link IntervalSystem} with the given priority and the given interval.
	 * @param interval The amount of time, in seconds, that the system waits before it
	 * executes {@link IntervalSystem#updateInterval}.
	 * @param priority The system's priority; lower priorities execute first.
	 */
	IntervalSystem(float interval, uint64_t priority);

	void update(float deltaTime) override;
protected:
	/**
	 * <p>Should be overridden with the processing logic for the system.</p>
	 */
	virtual void updateInterval() = 0;
};

}

#endif /* INTERVALSYSTEM_HPP_ */
