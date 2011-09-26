#ifndef _DUCKS_CACTION_H_
#define _DUCKS_CACTION_H_

#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include "constants.h"

namespace ducks {

///represents an action of a bird, and its movement direction

///this class is used both for representing a past action and for
///shooting at a bird
struct CAction
{
    ///construct a bird action object
    
    /// \param pBirdNumber the bird index
    /// \param pHorz the horizontal action of the bird
    /// \param pVert the horizontal action of the bird
    /// \param pMovement the movement of the bird 
    CAction(int pBirdNumber,EAction pHorz,EAction pVert,int pMovement)
        :	mBirdNumber(pBirdNumber)
        ,	mHorz(pHorz)
        ,	mVert(pVert)
        ,	mMovement((EMovement)pMovement)
    {}

    ///returns true if the bird is dead
    bool IsDead() const 	{	return (mMovement&BIRD_DEAD);	}

    ///the index of the bird this action corresponds too
    int GetBirdNumber() const	{	return mBirdNumber;	}

    ///the horizontal action (one of ACTION_ACCELERATE, ACTION_KEEPSPEED or ACTION_STOP)
    EAction GetHAction() const	{	return mHorz;	}
    ///the vertical action (one of ACTION_ACCELERATE, ACTION_KEEPSPEED or ACTION_STOP)
    EAction GetVAction() const	{	return mVert;	}
    
    ///the movement of the bird
    
    ///can be either BIRD_STOPPED or a combination of one or two of MOVE_WEST, MOVE_EAST, MOVE_UP and MOVE_DOWN
    /// for example, MOVE_WEST|MOVE_UP  or MOVE_EAST or MOVE_EAST|MOVE_DOWN
    /// of course, MOVE_WEST can't be combined with MOVE_EAST and MOVE_UP can't be combined with MOVE_DOWN
    EMovement GetMovement() const {	return mMovement;	}

    ///represents a no-shoot action
    bool IsDontShoot() const	{	return (mBirdNumber==-1);	}
    
    ///prints the content of this action object
    void Print() const
    {
        if(IsDontShoot())
            std::cout << "DONT SHOOT" << std::endl;
        else
        {
            std::cout << mBirdNumber << " ";
        
            if(IsDead())
                std::cout << "DEAD DUCK";
            else
            {
                if(mHorz==ACTION_ACCELERATE)
                    std::cout << "ACCELERATE ";
                else if(mHorz==ACTION_KEEPSPEED)
                    std::cout << "KEEPSPEED ";
                else 
                    std::cout << "STOP ";
                if(mVert==ACTION_ACCELERATE)
                    std::cout << "ACCELERATE";
                else if(mVert==ACTION_KEEPSPEED)
                    std::cout << "KEEPSPEED";
                else 
                    std::cout << "STOP";
            
                if(mMovement==BIRD_STOPPED)
                    std::cout << " STOPPED";
                else
                {
                    if(mMovement&MOVE_UP)
                        std::cout << " UP";
                    if(mMovement&MOVE_DOWN)
                        std::cout << " DOWN";
                    if(mMovement&MOVE_WEST)
                        std::cout << " WEST";
                    if(mMovement&MOVE_EAST)
                        std::cout << " EAST";
                }
            }
            std::cout << std::endl;
        }
    }

    bool operator==(const CAction &pRH) const
    {
        return mBirdNumber==pRH.mBirdNumber&&
               mHorz==pRH.mHorz&&mVert==pRH.mVert&&
               mMovement==pRH.mMovement;
    }
    
private:
    int mBirdNumber;
    EAction mHorz,mVert;
    EMovement mMovement;
};

///special CAction object used to choose not to shoot
static const CAction cDontShoot(-1,ACTION_STOP,ACTION_STOP,BIRD_STOPPED);

/*namespace ducks*/ }

#endif
