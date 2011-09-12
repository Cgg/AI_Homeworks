#include "cplayer.h"
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
    pBoard.FindPossibleMoves(lMoves,CELL_OWN);
    /*
     * Here you should write your clever algorithms to get the best next move.
     * This skeleton returns a random movement instead.
     */
    return lMoves[rand()%lMoves.size()];
}

HType CPlayer::GetHeuristic( const CBoard & pBoard ) const
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

/*namespace chk*/ }
