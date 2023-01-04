#pragma once

#include <type_traits>

#include "Shared/SimpleBinStream.hpp"

namespace simple {
	template<typename Type>
	class mem_istream;
	template<typename Type>
	class mem_ostream;
}

using PacketIStream = simple::mem_istream<std::true_type>;
using PacketOStream = simple::mem_ostream<std::true_type>;