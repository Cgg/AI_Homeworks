#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include <vector>
#include <limit>

#include "constants.h"

namespace chk
{

class BoardState
{
  public:
    typedef H_Type int ;

    static H_Type Min = numeric_limits< H_Type >::min();
    static H_Type Max = numeric_limits< H_Type >::max();

  private:
    // put here constants and typedefs

    static H_Type OWN_P_VAL = 1;
    static H_Type OTH_P_VAL = -1;
    static H_Type OWN_K_VAL = 2;
    static H_Type OTH_K_VAL = -2;

    enum Player
    {
      MaxPlayer,
      MinPlayer
    };

    struct Node
    {
      CMove  & moveToGetHere;
      CBoard & boardAtNode;
    };

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
