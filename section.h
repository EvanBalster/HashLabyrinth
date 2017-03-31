#ifndef HAZE_SECTION_H
#define HAZE_SECTION_H

#include "seed.h"
#include "hash.h"

namespace haze
{


	template<unsigned PortalCount, bool UseSectionSeed
		class PortalSeed = MazeSeedO, class SectionSeed = MazeSeed>
	struct Section_
	{

		// This array is a ring.  Order matters, but not which item comes first!
		PortalSeed portalSeeds[PortalCount];
	};
}

#endif
