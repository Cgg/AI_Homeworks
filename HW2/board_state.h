#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include <vector>
#include <limits>

#include "constants.h"

namespace chk
{

class CMove;
class CBoard;

class BoardState
{
  public:

    typedef int H_Type;

    static H_Type Min;
    static H_Type Max;

    static const int DL = 4; // depth limit for alphaBeta

    struct Node
    {
      Node( CMove & move, CBoard & board ) :
        moveToGetHere( move ), boardAtNode( board ) {}

      CMove  & moveToGetHere;
      CBoard & boardAtNode;
    };

    enum Player
    {
      MaxPlayer,
      MinPlayer
    };

  private:

    static const H_Type OWN_P_VAL = 1;
    static const H_Type OTH_P_VAL = -1;
    static const H_Type OWN_K_VAL = 2;
    static const H_Type OTH_K_VAL = -2;

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
};

}

#endif // BOARD_STATE_H
