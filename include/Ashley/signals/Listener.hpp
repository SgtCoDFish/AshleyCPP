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

#ifndef ACPP_SIGNALS_LISTENER_HPP_
#define ACPP_SIGNALS_LISTENER_HPP_

#include "Ashley/AshleyConstants.hpp"

namespace ashley {

template<typename T> class Signal;

/**
 * <p>A simple Listener interface used to listen to a {@link Signal}.</p>
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
template<typename T>
class Listener {
public:
	Listener() {
	}

	virtual ~Listener() = default;
	Listener(const Listener &other) = default;
	Listener(Listener &&other) = default;
	Listener& operator=(const Listener &other) = default;
	Listener& operator=(Listener &&other) = default;

	/**
	 * <p>Called by a {@link Signal} for each of its attached {@link Listener}s when some event occurs.</p>
	 * @param signal the calling signal.
	 * @param object an object related to the call.
	 */
	virtual void receive(ashley::Signal<T> * const signal, T *object) = 0;
};
}

#endif /* LISTENER_HPP_ */
