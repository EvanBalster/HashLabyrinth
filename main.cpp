//
//  main.cpp
//  HashLabyrinth
//
//  Created by Evan Balster on 4/1/17.
//  Copyright © 2017 Evan Balster. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <cmath>

#include "seed_ring.h"

namespace haze_test
{
	using namespace haze;
	
	struct Portal
	{
		uint32_t seed;
		
		Portal() : seed(0) {}
		
		Portal(uint32_t _seed) : seed(_seed)
		{
		}
	
		bool isWall() const
		{
			// Odds 3/8 of a wall
			return (seed&7) < 5;  // || (seed&7) == 7;
		}
	};
	
	typedef SeedRing_<4> SeedRing;

	struct Section
	{
		Section()
		{
		}
		
		Section(const Section &from, unsigned portalIndex) :
			seedRing(from.seedRing)
		{
			seedRing.traverse(portalIndex);
			
			limitRange();
		}
		
		void limitRange()
		{
			for (auto &i : seedRing.seeds) i &= 0x1FF;
		}
		
		Portal operator[](unsigned index) const
		{
			return Portal(seedRing[index]);
		}
	
		bool operator==(const Section &other) const
		{
			return seedRing == other.seedRing;
		}
		
		bool operator!=(const Section &other) const
		{
			return seedRing != other.seedRing;
		}
		
		SeedRing seedRing;
	};
}

namespace std
{
	ostream &operator<<(ostream &out, const haze_test::Section &sec)
	{
		out
			<< sec[0].seed << ","
			<< sec[1].seed << ","
			<< sec[2].seed << ","
			<< sec[3].seed;
		return out;
	}
	
	template<>
	struct hash<haze_test::Section>
	{
		size_t operator()(const haze_test::Section &k) const    {return k.seedRing.hash();}
	};
}

namespace haze_test
{
	/*struct SectionNote
	{
		unsigned depth = 0;
		const SectionNote *back = NULL, spare[3] = NULL;
	};*/
	
	//typedef std::vector<Section>                Sections;
	typedef std::unordered_map<Section, unsigned> KnownSections;
	typedef std::unordered_set<Section> SectionSet;

	struct Explorer
	{
		SectionSet known;
		
		SectionSet unexplored;
		
		unsigned
			nRetraced = 0,
			nSections = 0,
			nWalls    = 0,
			nDoors    = 1,
			nDeadEnds = 0,
			nHallways = 0,
			nThreeWays= 0,
			nFourWays = 0,
			nMaxDepth = 0;
		
		Explorer()
		{
		}
		
		
		bool explore(const Section &section, bool countRetrace = false)
		{
			if (known.find(section) != known.end()) return false;
			
			
			unexplored.erase(section);
			/*if (unexplored.erase(section)
			{
				std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
				std::cout << "No longer unknown: " << section;
			}*/
		
			//std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
			//std::cout << "Explored " << section;
		
			newSection(section);
			known.emplace(section);
			
			if (known.size() % 100000 == 0)
			{
				std::cout << '.' << std::flush;
			}
			
			// Note unexplored neighbors
			for (unsigned i = 0; i < SeedRing::Size; ++i)
				if (!section[i].isWall())
			{
				
				Section neighbor(section, i);
				
				if (known.find(neighbor) == known.end())
				{
					unexplored.emplace(neighbor);
					
					//std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
					//std::cout << "Noticed " << neighbor;
					
					// Sanity check
					Section mirror(neighbor, i);
					if (mirror != section)
					{
						std::cout << std::endl
							<< "Failed symmetry test on section " << section
							<< "\n    ...Neighbor is " << neighbor
							<< ", mirror is " << mirror
							<< std::endl;
						return false;
					}
				}
			}
			
			return true;
		}
		
		void meander(Section section, unsigned count)
		{
			//unsigned lastPortal = 0;
			
			for (unsigned i = 0; i < count; ++i)
			{
				if (!explore(section)) ++nRetraced;
				section = Section(section, rand()&3);
			}
		}
		
		bool exploreNextLayer()
		{
			if (unexplored.size() == 0) return false;
			
			SectionSet toExplore;
			toExplore.swap(unexplored);
			
			++nMaxDepth;
			
			for (auto &section : toExplore) explore(section);
			
			return true;
		}
		
		void exploreDepthFirst(const Section &section, unsigned maxDepth,
			unsigned curDepth = 0)
		{
			auto found = known.find(section);
			
			nMaxDepth = std::max(curDepth, nMaxDepth);
			
			if (found == known.end())
			{
				unexplored.erase(section);
				/*if (unexplored.erase(section)
				{
					std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
					std::cout << "No longer unknown: " << section;
				}*/
			
				//std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
				//std::cout << "Explored " << section;
			
				newSection(section);
				known.emplace(section);
				
				if (known.size() % 100000 == 0)
				{
					std::cout << '.' << std::flush;
				}
			}
			else
			{
				// We've been here before
				return;
			}
			
			// Note unexplored neighbors
			for (unsigned i = 0; i < SeedRing::Size; ++i)
			{
				if (!section[i].isWall())
				{
					Section neighbor(section, i);
					
					if (known.find(neighbor) == known.end())
					{
						unexplored.emplace(neighbor);
						
						//std::cout << std::endl; for (unsigned i=curDepth; i--;) std::cout << ' ';
						//std::cout << "Noticed " << neighbor;
						
						// Sanity check
						{
							Section mirror(neighbor, i);
							if (mirror != section)
							{
								std::cout << std::endl
									<< "Failed symmetry test on section " << section
									<< std::endl;
								return;
							}
						}
					}
				}
			}
			
			// Don't explore neighbors if we're at max depth
			if (curDepth >= maxDepth) return;
			
			// Explore neighbors now!
			for (unsigned i = 0; i < SeedRing::Size; ++i)
			{
				if (!section[i].isWall())
				{
					Section neighbor(section, i);
					
					exploreDepthFirst(neighbor, maxDepth, curDepth + 1);
				}
			}
		}
		
	private:
		void newSection(const Section &section)
		{
			++nSections;
			unsigned nDoorsOrig = nDoors;
			for (unsigned i = 0; i < SeedRing::Size; ++i)
			{
				if (section[i].isWall()) ++nWalls; else ++nDoors;
			}
			switch (nDoors - nDoorsOrig)
			{
			case 1: ++nDeadEnds;  break;
			case 2: ++nHallways;  break;
			case 3: ++nThreeWays; break;
			case 4: ++nFourWays;  break;
			}
			--nDoors;
		}
	};
}

using namespace haze_test;


int main(int argc, const char * argv[])
{
	Section origin;
	
	srand(unsigned(time(0)));
	
	for (unsigned i = 0; i < 10; ++i)
	{
		for (auto &seed : origin.seedRing.seeds)
		{
			seed = SeedRing::Seed(rand());
			origin.limitRange();
		}

		Explorer explorer;
		
		std::cout << "Explorer #" << (i+1) << " sets out from " << origin << "..." << std::flush;
		
		explorer.unexplored.emplace(origin);
		while (explorer.known.size() < 1000000)
		{
			if (!explorer.exploreNextLayer()) break;
		}
		
		//explorer.meander(origin, 1000000);
		
		std::cout << "\n    ...done.\n"
			<< "  Explored:   " << std::setw(7) << explorer.nSections << "  "
			<< "  Dead Ends:  " << std::setw(7) << explorer.nDeadEnds << "  "
			<< "  Doors:      " << std::setw(7) << explorer.nDoors << "\n"
		
			<< "  Unexplored: " << std::setw(7) << explorer.unexplored.size() << "  "
			<< "  Hallways:   " << std::setw(7) << explorer.nHallways << "  "
			<< "  Walls:      " << std::setw(7) << explorer.nWalls << "\n"
		
			<< "  Retraced:   " << std::setw(7) << explorer.nRetraced << "  "
			<< "  Three-Ways: " << std::setw(7) << explorer.nThreeWays << "\n"
		
			<< "  Max Depth:  " << std::setw(7) << explorer.nMaxDepth << "  "
			<< "  Four-Ways:  " << std::setw(7) << explorer.nFourWays << "\n"
			<< std::flush;
		
		std::cout << std::endl;
	}
	
	
    return 0;
}
