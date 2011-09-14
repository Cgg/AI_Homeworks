#include "cplayer.h"

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <memory>

namespace chk
{
double CBoard::squareManCoeff[32] = {
      1, 1, 1, 1,
      2, 1, 1, 1,
      2, 1, 1, 1,
      2, 2, 2, 1,
      2, 2, 2, 1,
      2, 1, 1, 1,
      2, 1, 1, 4,
      4, 4, 4, 4
    };

double CBoard::squareKingCoeff[32] = {
      6, 6, 6, 6,
      6, 1, 1, 1,
      1, 1, 1, 1,
      1, 4, 4, 1,
      1, 4, 4, 1,
      1, 1, 1, 1,
      1, 1, 1, -2,
      -2, -2, -2, -2
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
  std::cout << "depth" << depth << std::endl;

  if( depth == 0 ) // we hit depth limit
  {
    std::cout << "bottom leaf, value = " << origin.value << std::endl;

    return origin.value;
  }
  else
  {
    std::vector< Node > * nodeChildren = expand( origin, player );

   assert( !nodeChildren->empty() ); // no children means no possible moves means terminal node

    std::vector< Node >::iterator it_ch;

    if( player == MaxPlayer )
    {
      for( it_ch = nodeChildren->begin() ; it_ch != nodeChildren->end() ; it_ch++)
      {
        H_Type tmp = std::max( alpha,
                     AlphaBeta( (*it_ch), depth-1, alpha, beta, MinPlayer ));

        std::cout << "alpha = " << tmp << std::endl;

        alpha = tmp;

        if( beta <= alpha )
        {
          std::cout << ">>>B p" << std::endl;

          break;
        }
        return alpha;
      }
    }
    else
    {
      for( it_ch = nodeChildren->begin() ; it_ch != nodeChildren->end() ; it_ch++)
      {
        H_Type tmp = std::min( beta,
                        AlphaBeta( (*it_ch), depth-1, alpha, beta, MaxPlayer ) );

        std::cout << "beta = " << tmp << std::endl;
        beta = tmp;

        if( beta <= alpha )
        {
          std::cout << ">>>A p" << std::endl;

          break;
        }

        return beta;
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
  double ownPC = 0;
  double ownKC = 0;
  double othPC = 0;
  double othKC = 0;

  pBoard.GetPiecesCount( ownPC, ownKC, othPC, othKC );

  //std::cout << "my pieces = " << ownPC << std::endl;

  return ( OWN_P_VAL * ownPC + OTH_P_VAL * othPC +
           OWN_K_VAL * ownKC + OTH_K_VAL * othKC );
}

std::vector< Node > * Node::expand
(
  Node                & origin,
  Player                player
)
{
  std::vector< CMove > nextMoves;

  origin.boardAtNode->FindPossibleMoves( nextMoves, ( player == MaxPlayer ? CELL_OWN : CELL_OTHER ) );

  std::vector< Node > * expandedNodes = new std::vector< Node >;

  expandedNodes->reserve( nextMoves.size() );

  // find every possible moves from this state, generate corresponding
  // boards and create nodes

  std::vector< CMove >::const_iterator it_moves;

  for( it_moves = nextMoves.begin() ; it_moves != nextMoves.end() ; it_moves++)
  {
    expandedNodes->push_back( Node( new CMove( *it_moves ), new CBoard( *(origin.boardAtNode), ( *it_moves ) ) ) );
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
      Node origin( new CMove( *it_move ), new CBoard( pBoard, ( *it_move ) ) );

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

