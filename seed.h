#ifndef HAZE_SEED_H
#define HAZE_SEED_H

#include <cstdint>
#include <type_traits>


namespace haze
{
	/*
		Maze seed template.
			IntBase should always be unsigned.
			May have a binary orientation occupying the lowest bit.
	*/
	template<typename IntBase, bool Oriented = false>
	struct MazeSeed_
	{
		static_assert(std::is_unsigned<IntBase>::value, "MazeSeed_ must be based on an unsigned value.");

		static const IntBase
			ONE       = 1u,
			ORI_SHIFT = 8u*sizeof(IntBase)-1,
			ORI_BIT   = ONE<<ORI_SHIFT;


		void set(IntBase i, bool orientation)
		{
			if (Oriented) value = (i & (~ORI_BIT)) | (orientation ? ORI_BIT : 0);
		}

		IntBase get() const
		{
			if (Oriented) return value & (~ORI_BIT);
			else          return value;
		}


		bool orientation() const
		{
			return (Oriented ? (value>>ORI_SHIFT)==ONE : false);
		}

		void reverse()
		{
			if (Oriented) value ^= ORI_BIT;
		}

		void setOrientation(bool orientation)
		{
			if (Oriented) set(get(), orientation);
		}

		void set(IntBase i)
		{
			if (Oriented) set(i, orientation())
			else          value = i;
		}

		operator IntBase() const          {return get();}
		MazeSeed_ operator=(IntBase i)    {set(i); return *this;}

		IntBase value;
	};

	typedef MazeSeed_<uint8_t,  true > MazeSeed8o;
	typedef MazeSeed_<uint8_t,  false> MazeSeed8;
	typedef MazeSeed_<uint16_t, true > MazeSeed16o;
	typedef MazeSeed_<uint16_t, false> MazeSeed16;
	typedef MazeSeed_<uint32_t, true > MazeSeed32o;
	typedef MazeSeed_<uint32_t, false> MazeSeed32;
	typedef MazeSeed_<uint64_t, true > MazeSeed64o;
	typedef MazeSeed_<uint64_t, false> MazeSeed64;

	typedef MazeSeed32o MazeSeedO;
	typedef MazeSeed8   MazeSeed;
}

#endif
