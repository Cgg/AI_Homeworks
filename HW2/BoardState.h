#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include <vector>
#include <limit>

#include "constants.h"

namespace chk
{

class BoardState
{
  private:
    // put here constants and typedefs
    typedef H_Type int ;
    typedef ChildrenHolder std::vector< BoardState * >;

    static H_Type OWN_P_VAL = 1;
    static H_Type OTH_P_VAL = -1;
    static H_Type OWN_K_VAL = 2;
    static H_Type OTH_K_VAL = -2;

  public:

    BoardState
    (
      ECell          maxPlayer,
      CMove  const & move,
      CBoard const & state,
      int            depth
    );

    ~BoardState
    (
      void
    );

    BoardState * GetNextState
    (
      CMove const & nextMove
    );

  protected:

    void expand
    (
      void
    );

    H_Type alphaBeta
    (
      H_Type alpha,
      H_Type beta,
      int    depthLimit
    );

    H_Type computeHeuristic
    (
      CBoard const & board
    );

  private:

    ECell    player;
    CBoard & board;
    CMove  & moveToGetHere;
    H_Type   value;
    int      depth;

    ChildrenHolder children;
};

}

#endif // BOARD_STATE_H
