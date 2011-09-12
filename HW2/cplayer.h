#ifndef _CHECKERS_CPLAYER_H_
#define _CHECKERS_CPLAYER_H_

#include "constants.h"
#include "ctime.h"
#include "cmove.h"
#include "cboard.h"
#include <vector>

namespace chk
{

class CPlayer
{
public:
    ///constructor

    ///Shouldn't do much. Any expensive initialization should be in
    ///Initialize
    CPlayer();

    ///called when waiting for the other player to move

    ///\param pBoard the current state of the board
    ///\return false if we don't want this function to be called again
    ///until next move, true otherwise
    bool Idle(const CBoard &pBoard);

    ///perform initialization of the player

    ///\param pFirst true if we will move first, false otherwise
    ///\param pDue time before which we must have returned. To check,
    ///for example, to check if we have less than 100 ms to return, we can check if
    ///CTime::GetCurrent()+100000>pDue or pDue-CTime::GetCurrent()<100000.
    ///All times are in microseconds.
    void Initialize(bool pFirst,const CTime &pDue);

    ///perform a move

    ///\param pBoard the current state of the board
    ///\param pDue time before which we must have returned
    ///\return the move we make
    CMove Play(const CBoard &pBoard,const CTime &pDue);
};

/*namespace chk*/ }

#endif
