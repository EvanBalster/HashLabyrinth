#ifndef HAZE_HASH_H
#define HAZE_HASH_H

#include <cstdint>

namespace haze
{
	struct FastHash32
	{
		typedef uint32_t Hash;
		
		// Scalar hash.
		static Hash Get(uint32_t x)
		{
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x =  (x >> 16) ^ x;
			return x;
		}
		
		Hash operator()(uint32_t x) const    {return Get(x);}
	};
	
	
}

#endif
