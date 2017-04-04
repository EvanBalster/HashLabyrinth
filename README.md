# Hash Labyrinth: Simple math for vast mazes.

The algorithms here can be used to generate vast and senseless labyrinths.  In these places, four left turns won't bring you back where you came from...  But you can always retrace your steps.


# The Math

1. A **seed ring** is a rotatable array of **N** integers.
  * We'll describe them like `[:12, 34, 56, 78:]`
  
2. We can traverse a **seed** to generate a new **seed ring**.
  * The seed we traversed through is unchanged.
  * Other seeds in the ring are XOR'd by a hash of the seed we traversed.
  * Traversing from `[:12, 34, 56, 78:]` through `34` takes us to `[:12 XOR H, 34, 56 XOR H, 78 XOR H:]` where `H = Hash(34)`.
  
3. Two **seed rings** are equivalent if they differ only by rotation.
  * `[:12, 34, 56, 78:] == [:56, 78, 12, 34:]`
  * `[:12, 34, 56, 78:] != [:78, 56, 34, 12:]`


# The Maze

In a hash labyrinth, each **seed ring** defines a **section**.  Each **seed** in the ring can potentially define a **portal** connecting one section to another.

It is always mathematically possible to traverse a seed, but in labyrinths they may be treated as windows, walls or other impassable features.  By defining only some seeds as passable doorways or portals we can get a more interesting topology — assuming we wish to create something that looks like a labyrinth.

We can have other state defining sections, as long as any transformations are reversible.  One very useful example is a value for which side of a portal we're on; this allows portals to have non-symmetrical shapes.

If we want a seed for the section as a whole, we can generate an orientation-independent hash of the seed ring.  We can do this by rotating the ring into the orientation which collates lowest, and then hashing the resulting array.
