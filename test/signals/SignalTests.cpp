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

#include <vector>
#include <memory>

#include "AshleyTestCommon.hpp"
#include "Ashley/signals/Listener.hpp"
#include "Ashley/signals/Signal.hpp"

#include "gtest/gtest.h"

using ashley::Listener;
using ashley::Signal;

namespace {
class Dummy {
};

class ListenerMock : public Listener<Dummy> {
public:
	ListenerMock() :
			count(0) {
	}
	virtual ~ListenerMock() {
	}

	int count = 0;

	void receive(const Signal<Dummy> &signal, Dummy &object) override {
		++count;
	}
};

class SignalTest : public ::testing::Test {
protected:
	Dummy dummy;
	Signal<Dummy> signal;
	std::shared_ptr<ListenerMock> listener;

	SignalTest() :
			dummy(), signal() {
		listener = std::make_shared<ListenerMock>();
	}
	virtual ~SignalTest() {
	}
};
}

// Check that basic signal/listener functionality works.
TEST_F(SignalTest, AddListenerAndDispatch) {
	signal.add(listener);

	for (int i = 0; i < 10; i++) {
		ASSERT_EQ(i, listener->count);
		signal.dispatch(dummy);
		ASSERT_EQ(i + 1, listener->count);
	}
}

// Check that signal/listener functionality works for multiple listeners.
TEST_F(SignalTest, AddListenersAndDispatch) {
	const int numListeners = 10;
	ListenerMock mockArr[numListeners];

	std::vector<std::shared_ptr<ashley::Listener<Dummy>>> listeners;

	while (listeners.size() < numListeners) {
		auto index = listeners.size();
		mockArr[index] = ListenerMock();
		std::shared_ptr<ashley::Listener<Dummy>> mPtr = std::make_shared<ListenerMock>(mockArr[index]);
		listeners.push_back(mPtr);
		signal.add(mPtr);
	}

	const int numDispatches = numListeners;

	for (int i = 0; i < numDispatches; i++) {
		for (auto p : listeners) {
			ASSERT_EQ(i, std::dynamic_pointer_cast<ListenerMock>(p)->count);
		}

		signal.dispatch(dummy);

		for (auto p : listeners) {
			ASSERT_EQ(i + 1, std::dynamic_pointer_cast<ListenerMock>(p)->count);
		}
	}
}

// Check that removing a listener works correctly.
TEST_F(SignalTest, AddListenerDispatchAndRemove) {
	std::shared_ptr<ListenerMock> lBPtr = std::make_shared<ListenerMock>();

	signal.add(listener);
	signal.add(lBPtr);

	const int numDispatches = 5;

	for (int i = 0; i < numDispatches; i++) {
		ASSERT_EQ(i, listener->count);
		ASSERT_EQ(i, lBPtr->count);

		signal.dispatch(dummy);

		ASSERT_EQ(i + 1, listener->count);
		ASSERT_EQ(i + 1, lBPtr->count);
	}

	signal.remove(lBPtr);

	for (int i = 0; i < numDispatches; i++) {
		ASSERT_EQ(i + numDispatches, listener->count);
		ASSERT_EQ(numDispatches, lBPtr->count);

		signal.dispatch(dummy);

		ASSERT_EQ(i + 1 + numDispatches, listener->count);
		ASSERT_EQ(numDispatches, lBPtr->count)<< "Listener not removed correctly.";
	}
}
