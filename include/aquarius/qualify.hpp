#pragma once
#include "qualify/basic_stream.hpp"


namespace aquarius
{
	using iostream = qualify::basic_stream<std::byte, std::allocator<std::byte>>;
}