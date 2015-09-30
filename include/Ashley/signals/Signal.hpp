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

#ifndef ACPP_SIGNALS_SIGNAL_HPP_
#define ACPP_SIGNALS_SIGNAL_HPP_

#include <vector>
#include <algorithm>
#include <memory>

#include "Ashley/AshleyConstants.hpp"
#include "Ashley/signals/Listener.hpp"

namespace ashley {

/**
 * <p>A Signal is a basic event class then can dispatch an event to multiple listeners. It uses
 * templates to allow any type of object to be passed around on dispatch.</p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
template<typename T> class Signal {
public:
	Signal() {
	}

	~Signal() = default;

	Signal(const Signal &other) = default;
	Signal(Signal &&other) = default;
	Signal& operator=(const Signal &other) = default;
	Signal& operator=(Signal &&other) = default;

	/**
	 * <p>Add a {@link Listener} to this {@link Signal}</p>
	 * @param listener The {@link Listener} to be added, passed by value to create an automatic copy which is used.
	 */
	void add(Listener<T> *listener) {
		listeners.emplace_back(listener);
	}

	/**
	 * <p>Remove a {@link Listener} from this {@link Signal}.</p>
	 * @param listener The {@link Listener} to be removed.
	 */
	void remove(Listener<T> * listener) {
		auto it = std::find_if(listeners.begin(), listeners.end(),
		        [&](ashley::Listener<T> *&found) {return listener == found;});

		if (it != listeners.end()) {
			listeners.erase(it);
		}
	}

	void removeAll() {
		listeners.clear();
	}

	/**
	 * <p>Calls receive on all {@link Listener}s attached to this {@link Signal}.
	 * @param object An object which is passed to each listener.
	 */
	void dispatch(T *object) {
		for (auto &p : listeners) {
			p->receive(this, object);
		}
	}

private:
	std::vector<Listener<T> *> listeners;
};
}

#endif /* SIGNAL_HPP_ */
