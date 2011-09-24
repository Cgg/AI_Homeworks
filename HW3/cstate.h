#ifndef _DUCKS_CSTATE_H_
#define _DUCKS_CSTATE_H_

#include "cduck.h"
#include <vector>

namespace ducks {

///represents the game state
struct CState
{
    ///returns the number of ducks
    int GetNumDucks()	const	{	return mDucks.size();	}
    ///returns a reference to the i-th duck
    const CDuck &GetDuck(int i)	const {	return mDucks[i];		}

    ///returns the index of your player among all players
    int WhoAmI() const				{	return mWhoIAm;			}

    ///returns the number of players
    int GetNumPlayers()	const		{	return mScores.size();	}

    ///returns your current score
    int MyScore() const				{	return mScores[mWhoIAm];	}
    ///returns the score of the i-th player
    int GetScore(int i) const			{	return mScores[i];			}
    
    ///returns the number of turns elapsed since last time Shoot was called.
    
    ///this is the amount of new data available for each duck
    int GetNumNewTurns() const		{	return mNumNewTurns;	}
    
private:
    CState() {}
    CState(const CState&);

    std::vector<CDuck> mDucks;
    std::vector<int> mScores;
    int mWhoIAm;
    
    int mNumNewTurns;
    
    friend class CClient;
};

/*namespace ducks*/ }

#endif
