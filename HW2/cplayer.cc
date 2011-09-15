#include "cplayer.h"

#include <cassert>
#include <cstdlib>
#include <algorithm>

//#define DEBUG // the infamous

namespace chk
{

double CBoard::squareManCoeff[32] = {
      1, 1, 1, 1,
      1, 1, 1, 1.2,
      1, 1, 1, 1.2,
      1.2, 1.5, 1.5, 1.5,
      1.2, 1.5, 1.5, 1.5,
      1, 1, 1, 1.2,
      1, 1, 1, 2,
      1.5, 2, 2, 2.2
    };

double CBoard::squareKingCoeff[32] = {
      2, 2, 2, 2,
      2, 1, 1, 1,
      1, 1, 1, 1,
      1, 1.5, 1.5, 1,
      1, 1.5, 1.5, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
      -1, 0, 0, 0
    };

double Node::AlphaBeta
(
  bool   isMaxNode,
  int    depthLimit = DEPTH_L,
  double alpha      = SMALL,
  double beta       = BIG
)
{
#ifdef DEBUG
  std::cerr << "Node::AlpBet : depth = " << depthLimit << " alpha = " << alpha
            << " beta = " << beta << " player = " << isMaxNode << std::endl;
#endif

  if( depthLimit == 0 )
  {
#ifdef DEBUG
    std::cerr << "Node::AlpBet : bottom leaf, value = " << value << std::endl;
#endif

    return value;
  }
  else
  {
    std::vector< Node > * children = expand( isMaxNode );
    std::vector< Node >::iterator it_node;

#ifdef DEBUG
    //assert( !children.empty() );
#endif

    if( isMaxNode )
    {
      for( it_node = children->begin() ; it_node != children->end() ; it_node++ )
      {
        alpha = std::max( alpha,
                (*it_node).AlphaBeta( !isMaxNode, depthLimit-1, alpha, beta ));

#ifdef DEBUG
        std::cerr << "depth = " << depthLimit << " alpha is now " << alpha << std::endl;
#endif
        if( beta <= alpha )
        {
#ifdef DEBUG
          std::cerr << "Node::AlpBet : beta pruning" << std::endl;
#endif
          break;
        }
      }

      delete children;

      return alpha;
    }
    else
    {
      for( it_node = children->begin() ; it_node != children->end() ; it_node++ )
      {
        beta = std::min( beta,
                (*it_node).AlphaBeta( !isMaxNode, depthLimit-1, alpha, beta ));

#ifdef DEBUG
        std::cerr << "depth = " << depthLimit << " beta is now " << beta << std::endl;
#endif
        if( beta <= alpha )
        {
#ifdef DEBUG
          std::cerr << "depht = " << depthLimit << " beta : " << beta << " is smaller than alpha : " << alpha << std::endl;
          std::cerr << "Node::AlpBet : alpha pruning" << std::endl;
#endif
          break;
        }
      }

      delete children;

      return beta;
    }
    delete children;
  }

  return 0;
}

double Node::computeHeuristic
(
  void
)
{
  // piece and king count for me (own) and the other player (oth)
  double ownPC = 0;
  double ownKC = 0;
  double othPC = 0;
  double othKC = 0;

  boardAtNode.GetPiecesCountWeighted( ownPC, ownKC, othPC, othKC );

#ifdef DEBUG
//  std::cerr << "Node::computeH : My pieces amount = " << ownPC << std::endl;
#endif

  return ( OWN_P_VAL * ownPC + OTH_P_VAL * othPC +
           OWN_K_VAL * ownKC + OTH_K_VAL * othKC );
}

std::vector< Node > * Node::expand
(
  bool isMaxNode
)
{
  std::vector< Node > * children = new std::vector< Node >;

  std::vector<CMove> lMoves;
  std::vector< CMove >::iterator it_move;

  boardAtNode.FindPossibleMoves(lMoves, ( isMaxNode ? CELL_OWN : CELL_OTHER ) );

  children->reserve( lMoves.size() );

  for( it_move = lMoves.begin() ; it_move != lMoves.end() ; it_move++ )
  {
    children->push_back( Node( boardAtNode, (*it_move) ) );
  }

  std::sort( children->begin(), children->end(), comp );

#ifdef DEBUG
  std::cerr << " Node::expand : " << children->size() << "children." << std::endl;
#endif

  return children;
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

    pBoard.FindPossibleMoves(lMoves,CELL_OWN);
    /*
     * Here you should write your clever algorithms to get the best next move.
     * This skeleton returns a random movement instead.
     */

    double tmpValue  = 0;
    double bestValue = SMALL;

    int bestMove = 0;

    // no need to do that if there is only one available move
    if( lMoves.size() > 1 )
    {
      for( int i = 0 ; i < lMoves.size() ; i++ )
      {
        Node origin( pBoard, lMoves[ i ] );

        tmpValue = origin.AlphaBeta( false );

#ifdef DEBUG
        std::cerr << "CPlayer::Play : Value for move " << i << " is " << tmpValue << std::endl;
#endif

        if( tmpValue > bestValue )
        {
#ifdef DEBUG
          std::cerr << "We have a new best move, index is " << i << std::endl;
#endif
          bestValue = tmpValue;
          bestMove  = i;
        }
      }
    }

#ifdef DEBUG
    std::cerr << ">>> Best move is move " << bestMove + 1 << " out of " << lMoves.size() << std::endl;
#endif

    return lMoves[ bestMove ];
    //return lMoves[rand()%lMoves.size()];
}

/*namespace chk*/ }

