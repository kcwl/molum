#pragma once
#include "molum/basic_stream.hpp"


namespace molum
{
	using iostream = molum::basic_stream<std::byte, std::allocator<std::byte>>;
}