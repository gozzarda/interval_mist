Interval MIST
=============
Algorithms for Maximum Internal Spanning Tree in Interval Graphs

## Notes

### Proof

- First prove that alg will give a spanning tree for any connected interval graph
- Consider the order greedy alg adds vertices in to be the greedy order
- Aim to prove by induction over the greedy order always MIST of vertices so far
  - Base: First two vertices guaranteed to be two leaf MIST
  - Step: Show that we can add next vertex in greedy order to tree and remain MIST
- Step cases:
  - New vertex is adjacent to leaf, adding it increases internals by 1, so still MIST (may need to prove adding a vertex can't increase MIST by more than 1)
  - New vertex is not adjacent to leaf, will therefore increase leaves by 1, keeping internals the same, must show this would happen for all MISTs
- **Aim to show?** any MIST of a prefix of the greedy order is a subtree of the total MIST?
  - Effectively no vertex could be added that would result in a graph with a MIST with fewer leaves
- **Aim to show?** any MIST containing this new vertex must have same number of internals as our MIST
  - If it must be a leaf, need to show any subtree of a MIST is a MIST for those verts?
  - If it can be internal... something something other path else disconnected?
- **Aim to show?** that a new interval not adjacent to any leaf in a MIST must not be adjacent to a leaf in *any* MIST
- Leaves can't be adjacent to each other (else we would have extended the earlier leaf with the later)
  - In intervals this means they must be disjoint
  - Show true of any MIST, since could graft branch onto this leaf and reduce number of leaves?
- Leaves therefore partition space new vertex could be in into chunks
- Aim to show if new vertex can be an internal in *any* MIST of these vertices then it should have been picked earlier in greedy order (or that MIST must actually be worse)