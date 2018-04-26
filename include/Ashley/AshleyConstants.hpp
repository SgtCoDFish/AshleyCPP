#ifndef ACPP_ASHLEYCONSTANTS_HPP_
#define ACPP_ASHLEYCONSTANTS_HPP_

#include <bitset>

#ifndef ASHLEY_MAX_COMPONENT_COUNT
#define ASHLEY_MAX_COMPONENT_COUNT 64
#endif

namespace ashley {

using BitsType = std::bitset<ASHLEY_MAX_COMPONENT_COUNT>;

}

#endif
