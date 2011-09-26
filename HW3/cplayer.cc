#include "cplayer.h"
#include <cstdlib>
#include <iostream>

#include "macro.h"

#define DEBUG

namespace ducks
{

uint8_t HashEvidence( CAction const & action )
{
  uint8_t result = 0;

  result = action.GetHAction() +
           ( action.GetVAction() << 2 ) +
           ( action.GetMovement() << 4 );

  return result;
}

CAction UnhashEvidence( uint8_t hash, int birdNumber )
{
  EAction   actionH = ( EAction )( hash & 0x03 );
  EAction   actionV = ( EAction )( ( hash & 0x0C ) >> 2 );
  EMovement move    = ( EMovement )( ( hash & 0xF0 ) >> 4) ;

  return CAction( birdNumber, actionH, actionV, move );
}

CPlayer::CPlayer()
{
  // For now let's assume that each behavior has the same probability to
  // happen
  // This probabilities should be adjusted later.
  for( int i = 0 ; i < B_N_BEHAVIORS * B_N_BEHAVIORS ; i++ )
    TransitionMatrix[ i ] = 1 / B_N_BEHAVIORS;
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

  int     duckSeqLenght;
  int     duckNumber;
  uint8_t hash;

#ifdef DEBUG
  std::cout << "Entering Guess" << std::endl;
#endif

  for(int i=0;i<pDucks.size();i++)
  {
#ifdef DEBUG
    std::cout << "Guess Duck " << i << std::endl;
#endif

    if(pDucks[i].IsAlive())
    {
#ifdef DEBUG
      std::cout << "\tDuck is alive" << std::endl;
#endif

       duckSeqLenght = pDucks[ i ].GetSeqLength();

       duckNumber = pDucks[ i ].GetAction( 0 ).GetBirdNumber();

       for( int sIdx = 0 ; sIdx < duckSeqLenght ; sIdx++ )
       {
         pDucks[ i ].GetAction( sIdx ).Print();

         hash = HashEvidence( pDucks[ i ].GetAction( sIdx ) );

         if( UnhashEvidence( hash, duckNumber ) == pDucks[ i ].GetAction( sIdx ) )
           std::cout << "Successfull hashing/unhashing" << std::endl;
         else
           std::cout << "FAIL at hashing/unhashing" << std::endl;
       }
    }
  }
}

void CPlayer::Hit(int pDuck,ESpecies pSpecies)
{
  std::cout << "HIT DUCK!!!\n";
}

/*namespace ducks*/ }
