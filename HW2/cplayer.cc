#include "cplayer.h"

#include <cstdlib>
#include <algorithm>

namespace chk
{
double CBoard::squareCoeff[32] = {
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
      2, 4, 4, 2,
      2, 4, 4, 2,
      1, 1, 1, 1,
      1, 1, 1, 1,
      2, 2, 2, 2
    };

H_Type Node::Min = -1000000;//std::numeric_limits< H_Type >::min();
H_Type Node::Max =  1000000;//std::numeric_limits< H_Type >::max();

H_Type Node::AlphaBeta
(
  Node   & origin,
  int      depth,
  H_Type   alpha,
  H_Type   beta,
  Player   player
)
{
  if( depth == 0 ) // we hit depth limit
  {
    return computeHeuristic( origin.boardAtNode );
  }
  else
  {
    std::vector< Node > nodeChildren;

    expand( nodeChildren, origin, player );
    std::vector< Node >::iterator it_ch;

    if( player == MaxPlayer )
    {
      for( it_ch = nodeChildren.begin() ; it_ch != nodeChildren.end() ; it_ch++)
      {
        alpha = std::max( alpha,
                     AlphaBeta( (*it_ch), depth-1, alpha, beta, MinPlayer ));

        if( beta <= alpha )
        {
          break;
        }
      }
    }
    else
    {
      for( it_ch = nodeChildren.begin() ; it_ch != nodeChildren.end() ; it_ch++)
      {
        beta = std::min( beta,
                    AlphaBeta( (*it_ch), depth-1, alpha, beta, MaxPlayer ));

        if( beta <= alpha )
        {
          break;
        }
      }
    }
  }

  return 0;
}

H_Type Node::computeHeuristic
(
  const CBoard & pBoard
)
{
  // piece and king count for me (own) and the other player (oth)
  float ownPC;
  float ownKC;
  float othPC;
  float othKC;

  pBoard.GetPiecesCount( ownPC, ownKC, othPC, othKC );

  return ( OWN_P_VAL * ownPC + OTH_P_VAL * othPC +
           OWN_K_VAL * ownKC + OTH_K_VAL * othKC );
}

void Node::expand
(
  std::vector< Node > & expandedNodes,
  Node                & origin,
  Player                player
)
{
  expandedNodes.clear();

  // find every possible moves from this state, generate corresponding
  // boards and create nodes

  std::vector< CMove > nextMoves;
  origin.boardAtNode.FindPossibleMoves( nextMoves, ( player == MaxPlayer ? CELL_OWN : CELL_OTHER ) ); // TODO

  std::vector< CMove >::const_iterator it_moves;

  for( it_moves = nextMoves.begin() ; it_moves != nextMoves.end() ; it_moves++)
  {
    CBoard newBoard( origin.boardAtNode, ( *it_moves ) );

    expandedNodes.push_back( Node( ( *it_moves ), newBoard ) );
  }

}

CPlayer::CPlayer()
{
}

bool CPlayer::Idle(const CBoard &pBoard)
{
    return false;
}

void CPlayer::Initialize(bool pFirst,const CTime &pDue)
{
    srand(CTime::GetCurrent().Get());
}

CMove CPlayer::Play(const CBoard &pBoard,const CTime &pDue)
{
    //Use the commented version if your system supports ANSI color (linux does)
    pBoard.Print();
    // pBoard.PrintNoColor();

    std::vector<CMove> lMoves;
    std::vector< CMove >::iterator it_move;

    H_Type bestValue = Node::Min;
    H_Type tmpValue  = 0;

    CMove bestMove;

    pBoard.FindPossibleMoves(lMoves,CELL_OWN);
    /*
     * Here you should write your clever algorithms to get the best next move.
     * This skeleton returns a random movement instead.
     */

    for( it_move = lMoves.begin() ; it_move != lMoves.end() ; it_move++ )
    {
      // ugly ?
      CBoard originBoard = CBoard( pBoard, ( *it_move ) );

      Node origin( ( *it_move ), originBoard );

      // we do alphaBeta for each possible move. Since we have Max's
      // possible moves, it results in Min nodes, hence the MinPlayer
      // parameter.

      tmpValue = Node::AlphaBeta( origin,
                                  Node::DL,
                                  Node::Min,
                                  Node::Max,
                                  MinPlayer );

      // Note : if we have several movements having same value we pick up
      // the first one for now.
      if( tmpValue > bestValue )
      {
        bestValue = tmpValue;
        bestMove  = *it_move;
      }
    }

    return bestMove;
    //return lMoves[rand()%lMoves.size()];
}

/*namespace chk*/ }

