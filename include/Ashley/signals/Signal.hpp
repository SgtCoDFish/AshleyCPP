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

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include <vector>
#include <algorithm>
#include <memory>

#include <iostream>

#include "Ashley/signals/Listener.hpp"

namespace ashley {
/**
 * <p>A Signal is a basic event class then can dispatch an event to multiple listeners. It uses
 * templates to allow any type of object to be passed around on dispatch.</p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
template<typename T>
class Signal {
public:
	Signal() {
	}

	~Signal() {
		listeners.clear();
	}

	void add(Listener<T> *listener) {
		listeners.push_back(listener);
	}

	void remove(Listener<T> *listener) {
		std::remove_if(listeners.begin(), listeners.end(),
				[&](Listener<T> *found) {return listener == found;});
	}

	void dispatch(const T &object) const {
		for (auto &p : listeners) {
			p->receive(*this, object);
		}
	}

private:
	std::vector<Listener<T> *> listeners;
};
}

#endif /* SIGNAL_HPP_ */
