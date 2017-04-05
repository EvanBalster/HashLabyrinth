#ifndef HAZE_SECTION_H
#define HAZE_SECTION_H

//#include "seed.h"
#include <cstdint>

namespace haze
{
	/*
		A hash algorithm!
	*/
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
	
	
	template<typename HashFunc_t >
	struct SeedMath_Basic_
	{
		typedef typename HashFunc_t::Hash Seed;
		
		// Return the hash of this portal's seed value.
		//   This should not generally be overridden.
		static Seed Hash(const Seed seed)
		{
			return HashFunc_t::Get(seed);
		}
		
		// Get the opposite side of this portal.
		//   This may be overridden, but to maintain consistency, guarantee this condition:
		//   a == a.otherSide().otherSide()
		static Seed Reverse(const Seed seed)
		{
			return seed;
		}
		
		// Get the complement of a portal after traveling through a different portal.
		//   This may be overridden, but to maintain consistency, guarantee this condition:
		//   a == a.neighbor(b.seedHash(), n).neighbor(b.otherSide().seedHash(), n)
		static Seed Traverse(const Seed base, const Seed throughHash, unsigned offset)
		{
			return base ^ throughHash;
		}
		
		// Seed value
		Seed seed;
	};
	
	
	/*
		A seed ring.
	*/
	template<unsigned N_Size, typename T_Rules = SeedMath_Basic_<FastHash32> >
	struct SeedRing_
	{
		static const unsigned Size = N_Size;
		
		typedef typename T_Rules::Seed Seed;
		
		/*
			Transform a seed ring by traversing one of its seeds.
		*/
		void traverse(unsigned portalIndex)
		{
			Seed hash = T_Rules::Hash(seeds[portalIndex]);
			
			seeds[portalIndex] = T_Rules::Reverse(seeds[portalIndex]);
			
			for (unsigned off = 1; off < Size; ++off)
			{
				unsigned i = (portalIndex+off) % Size;
				seeds[i] = T_Rules::Traverse(seeds[i], hash, off);
			}
		}
		
		/*
			Comparare two seed rings.
		*/
		static int Compare(const Seed *a, unsigned ai, const Seed *b, unsigned bi)
		{
			for (unsigned i = 0; i < Size; ++i)
			{
				auto sa = a[(ai+i) % Size], sb = b[(bi+i) % Size];
				if (sa < sb) return -1;
				if (sb < sa) return +1;
			}
			return 0;
		}
		
		/*
			Determine a "first" portal index independent of the rotation of the portals.
				Useful for determining equality and hashing.
		*/
		unsigned startIndex() const
		{
			unsigned result = 0;
			for (unsigned i = 1; i < Size; ++i)
				if (Compare(seeds, result, seeds, i)==-1) result = i;
			return result;
		}
		
		/*
			Determine if two seed rings are equal, ignoring rotation.
		*/
		bool operator==(const SeedRing_ &other) const
		{
			return Compare(seeds, startIndex(), other.seeds, other.startIndex())==0;
		}
		
		bool operator!=(const SeedRing_ &other) const    {return !operator==(other);}
		
		/*
			Determine a rotation-independent hash for the seed ring.
		*/
		Seed hash() const
		{
			unsigned start = startIndex();
			
			Seed hash = 0;
			for (unsigned i = 0; i < Size; ++i)
			{
				hash = T_Rules::Hash(hash ^ seeds[(start+i)%Size]);
			}
			
			return hash;
		}
		
		Seed       &operator[](const unsigned index)          {return seeds[index];}
		const Seed &operator[](const unsigned index) const    {return seeds[index];}
		

		// This array is a ring; order matters, but first and last do not!
		Seed seeds[Size];
	};
	
	
}

namespace std
{
	template<unsigned N_Size, typename T_Rules>
	struct hash<typename haze::SeedRing_<N_Size, T_Rules> >
	{
		typedef haze::SeedRing_<N_Size, T_Rules> Key;
		
		size_t operator()(const Key &k) const    {return k.ringHash();}
	};
}

#endif
