#ifndef _DUCKS_CDUCK_H_
#define _DUCKS_CDUCK_H_

#include "constants.h"
#include "caction.h"

#include <vector>
#include <cstdlib>

namespace ducks {

///represents a duck
class CDuck
{
public:
    CDuck()
        :	mSpecies(SPECIES_UNKNOWN)
    {
    }

    ///length of the sequence of past actions of the duck
    int GetSeqLength() const			{	return mSeq.size();		}
    
    ///returns the last action of the duck
    const CAction &GetLastAction() const	{	return GetAction(mSeq.size()-1);	}
    ///returns one action in the sequence of actions of the duck
    const CAction &GetAction(int i) const	{	return mSeq[i];		}
    
    ///returns true if the duck is dead
    bool IsDead() const				{	return GetLastAction().IsDead();	}
    ///returns true if the duck was dead at time step i
    bool WasDead(int i) const			{	return GetAction(i).IsDead();		}
    ///returns true if the duck is alive
    bool IsAlive() const				{	return !IsDead();	}
    ///returns true if the duck was alive at time step i
    bool WasAlive(int i) const			{	return !WasDead(i);		}

    ///returns the species of the duck (this will only be set if you
    ///have shot the duck)
    ESpecies GetSpecies() const		{	return mSpecies;	}

    ///used in the Guess function to guess the species of a duck
    void SetSpecies(ESpecies pSpecies)	{	mSpecies=pSpecies;	}

private:
    void PushBackAction(const CAction &pAction)	{   mSeq.push_back(pAction);	}

    std::vector<CAction> mSeq;
    ESpecies mSpecies;
    
    friend class CClient;
};

/*namespace ducks*/ }

#endif
