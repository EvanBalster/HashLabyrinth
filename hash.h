#ifndef HAZE_HASH_H
#define HAZE_HASH_H

#include "seed.h"

namespace haze
{
	uint32_t FastHash32(uint32_t v)
	{
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x =  (x >> 16) ^ x;
		return x;
	}
}

#endif
