#include "cplayer.h"
#include <cstdlib>
#include <iostream>

#include "macro.h"

namespace ducks
{

CPlayer::CPlayer()
{
}

CAction CPlayer::Shoot(const CState &pState,const CTime &pDue)
{
  /*
   * Here you should write your clever algorithms to get the best action.
   * This skeleton never shoots.
   */

  //this line doesn't shoot any bird
  return cDontShoot;

  //this line would predict that bird 0 is totally stopped and shoot at it
  //return CAction(0,ACTION_STOP,ACTION_STOP,BIRD_STOPPED);
}

void CPlayer::Guess(std::vector<CDuck> &pDucks,const CTime &pDue)
{
  /*
  * Here you should write your clever algorithms to guess the species of each alive bird.
  * This skeleton guesses that all of them are white... they were the most likely after all!
  */

  int duckSeqLenght;

  LOG( L_GUESS, "Here we are" );

  for(int i=0;i<pDucks.size();i++)
  {
    if(pDucks[i].IsAlive())
    {
       duckSeqLenght = pDucks[ i ].GetSeqLength();

       for( int sIdx = 0 ; sIdx < duckSeqLenght ; sIdx++ )
       {
         pDucks[ i ].GetAction( i ).Print();
       }
    }
  }
}

void CPlayer::Hit(int pDuck,ESpecies pSpecies)
{
  std::cout << "HIT DUCK!!!\n";
}

/*namespace ducks*/ }
