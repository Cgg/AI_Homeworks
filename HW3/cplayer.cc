#include "cplayer.h"
#include <cstdlib>
#include <iostream>

#include "macro.h"

#define DEBUG

namespace ducks
{

void PrintMatrix( std::vector< double > & theMatrix, int nRow, int nCol )
{
  for( int i = 0 ; i < nRow ; i++ )
  {
    for( int j = 0 ; j < nCol ; j++ )
      std::cout << theMatrix[ ( i * nCol ) + j ] << '\t' ;

    std::cout << std::endl;
  }
}


uint8_t HMM::HashEvidence( CAction const & action )
{
  return HashEvidence( action.GetHAction(), action.GetVAction(), action.GetMovement() );
}

uint8_t HMM::HashEvidence( EAction actionH, EAction actionV, EMovement move )
{
  return actionH +
         ( actionV << 2 ) +
         ( move << 4 );
}

CAction HMM::UnhashEvidence( uint8_t hash, int birdNumber )
{
  EAction   actionH = ( EAction )( hash & 0x03 );
  EAction   actionV = ( EAction )( ( hash & 0x0C ) >> 2 );
  EMovement move    = ( EMovement )( ( hash & 0xF0 ) >> 4) ;

  return CAction( birdNumber, actionH, actionV, move );
}

void HMM::InitTheMatrix()
{
  // In which we init/populate the various matrixes needed throughout the
  // hunting.

  // First the initial state matrix and the transition matrix
  PI.reserve( B_N_BEHAVIORS );

  TransitionMatrix.reserve( B_N_BEHAVIORS * B_N_BEHAVIORS );

  double pi_init = 1 / ( double )B_N_BEHAVIORS;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    PI[ i ] = pi_init;

    for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
    {
      TransitionMatrix[ i + ( j * B_N_BEHAVIORS ) ] = pi_init;
    }
  }

  // Since initializing the evidence matrix is tricky this is done in a
  // separate function.
  PopulateEvidenceMatrix();

#ifdef DEBUG
  std::cout << "PI" << std::endl;
  PrintMatrix( PI, 1, B_N_BEHAVIORS );
  std::cout << std::endl << "Transitions" << std::endl;
  PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
  std::cout << std::endl << "Evidences" << std::endl;
  PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
#endif
}

void HMM::PopulateEvidenceMatrix()
{
  // first populate the map containing hashes for all possible evidences

  // general case
  int i = 0;

  // the "all stop" case. / Can it really happen ? TODO
  evidencesHashes.insert( std::pair< uint8_t, int >( HashEvidence( ACTION_STOP, ACTION_STOP, BIRD_STOPPED ), i++ ) );

  // the "going horizontal" cases
  for( int iH = 0 ; iH < ACTION_STOP ; iH++ )
  {
    evidencesHashes.insert( std::pair< uint8_t, int >( HashEvidence( (EAction)iH, ACTION_STOP, MOVE_EAST ), i++ ) );
    evidencesHashes.insert( std::pair< uint8_t, int >( HashEvidence( (EAction)iH, ACTION_STOP, MOVE_WEST ), i++ ) );
  }

  // the "going vertical" cases
  for( int iV = 0 ; iV < ACTION_STOP ; iV++ )
  {
    evidencesHashes.insert( std::pair< uint8_t, int >( HashEvidence( ACTION_STOP, (EAction)iV, MOVE_UP ), i++ ) );
    evidencesHashes.insert( std::pair< uint8_t, int >( HashEvidence( ACTION_STOP, (EAction)iV, MOVE_DOWN ), i++ ) );
  }

  for( int iH = 0 ; iH < ACTION_STOP ; iH++ )
  {
    for( int iV = 0 ; iV < ACTION_STOP ; iV++ )
    {
      // ugly, ugly, ugly
      evidencesHashes.insert( std::pair< uint8_t, int >(
            HashEvidence( (EAction)iH, (EAction)iV, (EMovement)(MOVE_UP | MOVE_EAST) ), i++ ) );
      evidencesHashes.insert( std::pair< uint8_t, int >(
            HashEvidence( (EAction)iH, (EAction)iV, (EMovement)(MOVE_UP | MOVE_WEST) ), i++ ) );
      evidencesHashes.insert( std::pair< uint8_t, int >(
            HashEvidence( (EAction)iH, (EAction)iV, (EMovement)(MOVE_DOWN | MOVE_EAST) ), i++ ) );
      evidencesHashes.insert( std::pair< uint8_t, int >(
            HashEvidence( (EAction)iH, (EAction)iV, (EMovement)(MOVE_DOWN | MOVE_DOWN) ), i++ ) );
    }
  }

  // Now we can populate the actual evidence matrix, giving for each state
  // the probability of observing an evidence, given we are in the said
  // state.

  N_OBS = evidencesHashes.size();

#ifdef DEBUG
  std::cout << "Number of possible evidences : " << N_OBS << std::endl;
#endif

  EvidenceMatrix.reserve( B_N_BEHAVIORS * N_OBS );

  // at the beginning we assume for every state that we could equally
  // observe every evidence in it. (Of course this is not true but that's
  // what learning is for)
  double obs_init = 1 / (double)N_OBS;

  for( int i = 0 ; i < B_N_BEHAVIORS * N_OBS ; i++ )
    EvidenceMatrix[ i ] = obs_init;
}

std::vector< double >  HMM::Forward( int t, std::vector< uint8_t > const & observations )
{
  std::vector< double > alphaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = evidencesHashes[ observations[ t ] ];

  if( t == 0 )
  {
    // end of recursion
    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      alphaT[ i ] = PI[ i ] * EvidenceMatrix[ evidenceIdx + ( i * N_OBS ) ];
    }
  }
  else
  {
    // my, that's a yummy recursion !
    std::vector< double > alphaPrev = Forward( t - 1, observations );

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
        alphaT[ i ] += alphaPrev[ j ] * TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ];

      alphaT[ i ] *= EvidenceMatrix[ evidenceIdx + ( i * N_OBS ) ];
    }
  }

#ifdef DEBUG
  std::cout << "Alpha at t = " << t << std::endl;
  PrintMatrix( alphaT, B_N_BEHAVIORS, 1 );
#endif

  // populate the alphas matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    alphas[ t ][ i ] = alphaT[ i ];
  }

  return alphaT;
}

std::vector< double > HMM::Backward( int t, int lastT, std::vector< uint8_t > const & observations )
{
  std::vector< double > betaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = evidencesHashes[ observations[ t ] ];

  if( t == lastT - 1 )
  {
    // end of recursion
    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      betaT[ i ] = 1;
    }
  }
  else
  {
    // recursion... me liek it
    std::vector< double > betaNext = Backward( t+1, lastT, observations );

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; i < B_N_BEHAVIORS ; j++ )
      {
        betaT[ i ] = TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] * EvidenceMatrix[ evidenceIdx + ( j * N_OBS ) ] * betaNext[ j ];
      }
    }
  }

#ifdef DEBUG
  std::cout << "Beta at t = " << t << std::endl;
  PrintMatrix( betaT, B_N_BEHAVIORS, 1 );
#endif

  // populate the betas matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    betas[ t ][ i ] = betaT[ i ];
  }

  return betaT;
}

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

  int     duckSeqLenght;
  int     duckNumber;
  uint8_t hash;

#ifdef DEBUG
  std::cout << "Entering Guess" << std::endl;
#endif

  HMM model;

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
    }
  }
}

void CPlayer::Hit(int pDuck,ESpecies pSpecies)
{
  std::cout << "HIT DUCK!!!\n";
}

/*namespace ducks*/ }
