#include "board_state.h"
#include "cboard.h"
#include "cmove.h"

namespace chk
{

BoardState::Min = std::numeric_limits< H_Type >::min();
BoardState::Max = std::numeric_limits< H_Type >::max();

BoardState::H_Type BoardState::AlphaBeta
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
    return computeHeuristic( node.boardAtNode );
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
        alpha = max( alpha, 
                     alphaBeta( (*it_ch), depth-1, alpha, beta, MinPlayer ));

        if( beta <= alpha )
          break;
      }
    }
    else
    {
      for( it_ch = nodeChildren.begin() ; it_ch != nodeChildren.end() ; it_ch++)
      {
        beta = min( beta, 
                    alphaBeta( (*it_ch), depth-1, alpha, beta, MaxPlayer ));

        if( beta <= alpha )
          break;
      }
    }
  }
}

BoardState::HType BoardState::computeHeuristic
(
  const CBoard & pBoard
)
{
  // piece and king count for me (own) and the other player (oth)
  int ownPC;
  int ownKC;
  int othPC;
  int othKC;

  pBoard.GetPiecesCount( ownPC, ownKC, othPC, othKC );

  return H_Piece_Coeff * ( ownPC - othPC ) +
         H_King_Coeff  * ( ownKC - othKC );
}

void BoardState::expand
(
  std::vector< Node > & expandedNodes,
  Node                & origin,
  Player                player
)
{
  // find every possible moves from this state, generate corresponding
  // boards and create nodes

  std::vector< CMove > nextMoves;
  board.FindPossibleMoves( nextMoves, 0 ); // TODO

  std::vector< CMove >::const_iterator it_moves;

  for( it_moves = nextMoves.begin() ; it_moves != nextMoves.end() ; it_moves++)
  {
    Board newBoard( pBoard, ( *it_moves ) );

    Node tmpNode( ( *it_moves ), Board( pBoard, newBoard ) );

    result.push( tmpNode );
  }

  return result;
}

}
