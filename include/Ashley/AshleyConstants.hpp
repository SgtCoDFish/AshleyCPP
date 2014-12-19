/*
 * AshleyConstants.hpp
 *
 * Created on: 9 Aug 2014
 * Author: Ashley Davis (SgtCoDFish)
 */

#ifndef ASHLEYCONSTANTS_HPP_
#define ASHLEYCONSTANTS_HPP_

#include <bitset>
#include <memory>

#ifndef ASHLEY_MAX_COMPONENT_COUNT
#define ASHLEY_MAX_COMPONENT_COUNT 64
#endif

namespace ashley {

using BitsType = std::bitset<ASHLEY_MAX_COMPONENT_COUNT>;

template<typename T> using ashley_ptr_type = std::shared_ptr<T>;
template<typename T> using ashley_weak_ptr_type = std::weak_ptr<T>;

}

#endif /* ASHLEYCONSTANTS_HPP_ */
