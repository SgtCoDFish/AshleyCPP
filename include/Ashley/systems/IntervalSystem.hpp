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

#ifndef ACPP_SYSTEMS_INTERVALSYSTEM_HPP_
#define ACPP_SYSTEMS_INTERVALSYSTEM_HPP_

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
public:
	/**
	 * <p>Creates an {@link IntervalSystem} with the given priority and the given interval.
	 * @param interval The amount of time, in seconds, that the system waits before it
	 * executes {@link IntervalSystem#updateInterval}.
	 * @param priority The system's priority; lower priorities execute first.
	 */
	IntervalSystem(float interval, int64_t priority);

	virtual ~IntervalSystem() = default;
	IntervalSystem(const IntervalSystem &other) = default;
	IntervalSystem(IntervalSystem &&other) = default;
	IntervalSystem& operator=(const IntervalSystem &other) = default;
	IntervalSystem& operator=(IntervalSystem &&other) = default;

	void update(float deltaTime) override;

	float getInterval() const {
		return interval;
	}

	void setInterval(float interval) {
		this->interval = interval;
	}

protected:
	/**
	 * <p>Should be overridden with the processing logic for the system.</p>
	 */
	virtual void updateInterval() = 0;

private:
	float interval;
	float accumulator;
};

}

#endif /* INTERVALSYSTEM_HPP_ */
