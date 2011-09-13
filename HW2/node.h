#ifndef _NODE_H
#define _NODE_H

#include <vector>
#include <limits>

#include "cboard.h"
#include "cmove.h"

namespace chk {

typedef int H_Type;

enum Player
{
  MaxPlayer,
  MinPlayer
};

class Node
{
  // static data
  public:

    static const int DL = 4; // depth limit for alphaBeta

    static H_Type Min;
    static H_Type Max;

  protected:

    static const H_Type OWN_P_VAL = 1;
    static const H_Type OTH_P_VAL = -1;
    static const H_Type OWN_K_VAL = 2;
    static const H_Type OTH_K_VAL = -2;

  // static methods
  public:

    static H_Type AlphaBeta
    (
      Node   & origin,
      int      depth,
      H_Type   alpha,
      H_Type   beta,
      Player   player
    );

  protected:

    static void expand
    (
      std::vector< Node > & expandedNodes, // return of the function
      Node                & origin, // node from where we extend
      Player                player  // who shall play (min or max)
    );

    static H_Type computeHeuristic
    (
      CBoard const & board
    );

  // non static methods and data
  public:

    Node( CMove & move, CBoard & board )
      : moveToGetHere( move ), boardAtNode( board )
    {
    }

    CMove  & moveToGetHere;
    CBoard & boardAtNode;
};

}

#endif // _NODE_H

