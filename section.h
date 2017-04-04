#ifndef HAZE_SECTION_H
#define HAZE_SECTION_H

//#include "seed.h"
#include "hash.h"

namespace haze
{
	template<typename HashFunc_t = FastHash32>
	struct PortalBase_
	{
		typedef HashFunc_t                HashFunc;
		typedef typename HashFunc_t::Hash Hash;
		typedef Hash                      Seed;
		
		// Return the hash of this portal's seed value.
		//   This should not generally be overridden.
		Hash seedHash() const
		{
			return HashFunc::Get(seed);
		}
		
		// Get the opposite side of this portal.
		//   This may be overridden, but to maintain consistency, guarantee this condition:
		//   a == a.otherSide().otherSide()
		PortalBase_ otherSide() const
		{
			return *this;
		}
		
		// Get the complement of a portal after traveling through a different portal.
		//   This may be overridden, but to maintain consistency, guarantee this condition:
		//   a == a.neighbor(b.seedHash(), n).neighbor(b.otherSide().seedHash(), n)
		PortalBase_ complement(Hash throughSeedHash, unsigned offset) const
		{
			PortalBase_ result = {seed ^ throughSeedHash};
			return result;
		}
		
		Seed seed;
	};
	
	
	typedef PortalBase_<FastHash32> PortalBase32;
	
	
	/*
		A section
			N_Rotate:  How much to rotate a section when moving through a portal.
	*/
	template<typename T_Portal, unsigned N_Portals>
	struct Section_
	{
		static const unsigned PortalCount = N_Portals;
		
		typedef T_Portal Portal;
		
		/*
			Default constructor; might not initialize anything!
		*/
		Section_()
		{
		}

		/*
			Find the section adjacent to a given section, through the given portal.
		*/
		Section_(const Section_ &from, unsigned portalIndex)
		{
			portals[portalIndex] = from.portals[portalIndex].otherSide();
			
			auto seedHash = from.portals[portalIndex].seedHash();
			for (unsigned off = 1; off < PortalCount; ++off)
			{
				unsigned i = (portalIndex+off) % PortalCount;
				portals[i] = from.portals[i].complement(seedHash, off);
			}
		}
		
		/*
			Comparare two seed rings
		*/
		static int RingCompare(const Portal *a, unsigned ai, const Portal *b, unsigned bi)
		{
			for (unsigned i = 0; i < PortalCount; ++i)
			{
				auto sa = a[(ai+i)%PortalCount].seed, sb = b[(bi+i)%PortalCount].seed;
				if (sa < sb) return -1;
				if (sb < sa) return +1;
			}
			return 0;
		}
		
		/*
			Determine a "first" portal index independent of the rotation of the portals.
				Useful for determining equality and hashing.
		*/
		unsigned ringStart() const
		{
			unsigned result = 0;
			for (unsigned i = 1; i < PortalCount; ++i)
				if (RingCompare(portals, result, portals, i)==-1) result = i;
			return result;
		}
		
		/*
			Determine if two sections' seed rings are equal, ignoring rotation.
		*/
		bool ringEquals(const Section_ &other) const
		{
			return RingCompare(portals, ringStart(), other.portals, other.ringStart())==0;
		}
		
		/*
			Determine a hash for the section.
				This is independent from the rotation of the portals.
		*/
		typename Portal::Hash ringHash() const
		{
			unsigned start = ringStart();
			
			typename Portal::Hash hash = 0;
			for (unsigned i = 0; i < PortalCount; ++i)
			{
				hash ^= Portal::HashFunc::Get(portals[(start+i)%PortalCount].seed);
			}
			
			return hash;
		}
		

		// This array is a ring; order matters, but first and last do not!
		Portal portals[PortalCount];
	};
	
	
}

namespace std
{
	template<typename T_Portal, unsigned N_Portals>
	struct hash<typename haze::Section_<T_Portal, N_Portals> >
	{
		typedef haze::Section_<T_Portal, N_Portals> Key;
		
		size_t operator()(const Key &k) const    {return k.ringHash();}
	};
}

#endif
