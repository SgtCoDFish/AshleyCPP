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

	void receive(Signal<Dummy> * const signal, Dummy * const object) override {
		++count;
	}
};

class SignalTest : public ::testing::Test {
protected:
	Dummy dummy;
	Signal<Dummy> signal;
	std::unique_ptr<Listener<Dummy>> listener;

	SignalTest() :
			dummy(), signal(), listener{new ListenerMock()} {
	}
	virtual ~SignalTest() {
	}
};
}

// Check that basic signal/listener functionality works.
TEST_F(SignalTest, AddListenerAndDispatch) {
	signal.add(listener.get());
	auto dynList = (ListenerMock *)(listener.get());

	for (int i = 0; i < 10; i++) {
		ASSERT_EQ(i, dynList->count);
		signal.dispatch(&dummy);
		ASSERT_EQ(i + 1, dynList->count);
	}
}

// Check that signal/listener functionality works for multiple listeners.
TEST_F(SignalTest, AddListenersAndDispatch) {
	const int numListeners = 10;
	ListenerMock mockArr[numListeners];

	std::vector<ashley::Listener<Dummy>*> listeners;

	while (listeners.size() < numListeners) {
		auto index = listeners.size();
		mockArr[index] = ListenerMock();
		ashley::Listener<Dummy> *mPtr = &(mockArr[index]);
		listeners.push_back(mPtr);
		signal.add(mPtr);
	}

	const int numDispatches = numListeners;

	for (int i = 0; i < numDispatches; i++) {
		for (auto &p : listeners) {
			ASSERT_EQ(i, ((ListenerMock *)(p))->count);
		}

		signal.dispatch(&dummy);

		for (auto &p : listeners) {
			ASSERT_EQ(i + 1, ((ListenerMock *)(p))->count);
		}
	}
}

//// Check that removing a listener works correctly.
//TEST_F(SignalTest, AddListenerDispatchAndRemove) {
//	std::shared_ptr<ListenerMock> lBPtr = std::make_shared<ListenerMock>();
//	auto base = std::dynamic_pointer_cast<Listener<Dummy>>(lBPtr);
//	auto dynList = (ListenerMock *)(listener.get());
//	signal.add(listener);
//	signal.add(base);
//
//	const int numDispatches = 5;
//
//	for (int i = 0; i < numDispatches; i++) {
//		ASSERT_EQ(i, dynList->count);
//		ASSERT_EQ(i, lBPtr->count);
//
//		signal.dispatch(dummy);
//
//		ASSERT_EQ(i + 1, dynList->count);
//		ASSERT_EQ(i + 1, lBPtr->count);
//	}
//
//	signal.remove(base);
//
//	for (int i = 0; i < numDispatches; i++) {
//		ASSERT_EQ(i + numDispatches, dynList->count);
//		ASSERT_EQ(numDispatches, lBPtr->count);
//
//		signal.dispatch(dummy);
//
//		ASSERT_EQ(i + 1 + numDispatches, dynList->count);
//		ASSERT_EQ(numDispatches, lBPtr->count)<< "Listener not removed correctly.";
//	}
//}
