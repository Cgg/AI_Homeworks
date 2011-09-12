#include "board_state.h"

using namespace std;

BoardState::BoardState
(
  ECell          maxPlayer,
  CMove  const & move,
  CBoard const & state,
  int            depth
) :
  maxPlayer( maxPlayer ),
  board( state ),
  moveToGetHere( move ),
  depth( depth )
{
  value = computeHeuristic( board );
}

BoardState::~BoardState
(
  void
)
{
  ChildrenHolder::iterator it_ch;

  for( it_ch = children.begin() ; it_ch != children.end() ; it_ch++ )
  {
    delete ( *it_ch );
  }
}

BoardState * GetNextState
(
  CMove const & nextMove
)
{
  ChildrenHolder::iterator it_ch;

  for( it_ch = children.begin() ; it_ch != children.end() ; it_ch++ )
  {
    // if (*it_ch) is the nextMove then take out of children and return it
  }
}

void BoardState::expand
(
  void
)
{
  // generate all children and stuff them into corresponding class member.
  //
  // find every possible moves from this state, generate corresponding
  // boards and create children

  std::vector< CMove > nextMoves;
  board.FindPossibleMoves( nextMoves, player );

  std::vector< CMove >::const_iterator it_moves;

  for( it_moves = nextMoves.begin() ; it_moves != nextMoves.end() ; it_moves++)
  {
    children.push( new BoardState() );
  }
}

H_Type BoardState::alphaBeta
(
  H_Type alpha,
  H_Type beta,
  int    depthLimit
)
{
  // implement alpha-beta here

  if( depth == depthLimit )  // we are a leaf
  {
    return value;
  }
  else                       // we are in the middle of the tree
  {
    expand();

    if( ( depth % 2 ) == 0 ) // node MAX (my turn to play)
    {
    }
    else                     // node MIN (his turn)
    {
    }
  }
}

HType BoardState::computeHeuristic( const CBoard & pBoard ) const
{
  // piece and king count for me (own) and the other player (oth)
  int ownPC;
  int ownKC;
  int othPC;
  int othKC;

  pBoard( ownPC, ownKC, othPC, othKC );

  return H_Piece_Coeff * ( ownPC - othPC ) +
         H_King_Coeff * ( ownKC - othKC );
}

