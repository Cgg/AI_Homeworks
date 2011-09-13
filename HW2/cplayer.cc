#include "cplayer.h"
#include "board_state.h"

#include <cstdlib>

namespace chk
{

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

    BoardState::H_Type bestValue = BoardState::Min;
    BoardState::H_Type tmpValue  = 0;

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

      BoardState::Node origin( ( *it_move ),
                               originBoard );

      // we do alphaBeta for each possible move. Since we have Max's
      // possible moves, it results in Min nodes, hence the MinPlayer
      // parameter.

      tmpValue = BoardState::AlphaBeta( origin,
                                        BoardState::DL,
                                        BoardState::Min,
                                        BoardState::Max,
                                        BoardState::MinPlayer );

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

