#include "cplayer.h"
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <cmath>

//#define DEBUG
//#define DEBUG_RAND
//#define DEBUG_FW
//#define DEBUG_BW

namespace ducks
{

// Local methods implementation
void PrintMatrix( std::vector< double > const & theMatrix, int nRow, int nCol )
{
  for( int i = 0 ; i < nRow ; i++ )
  {
    for( int j = 0 ; j < nCol ; j++ )
      std::cout << theMatrix[ ( i * nCol ) + j ] << '\t' ;

    std::cout << std::endl;
  }
}

void CheckSum( std::vector< double > const &probaMatrix, int nRow, int nCol )
{
  double rowSum;

  for( int i = 0 ; i < nRow ; i++ )
  {
    rowSum = 0;

    for( int j = 0 ; j < nCol ; j++ )
    {
      rowSum += probaMatrix[ j + ( nCol * i ) ];
    }

    std::cout << "sum of row " << i << " is " << rowSum << std::endl;
  }
}

std::vector< double > GenerateUniformNoisyProba( int nProba )
{
  std::vector< double > probas;

  double curProb;

  double base = 1 / ( double )nProba;

  int epsilon = floor( ( base / 10.0 ) * 10000 );
  int halfEps = epsilon / 2;

  double sum = 1.0;

  for( int i = 0 ; i < nProba - 1 ; i++ )
  {
    curProb = base + ( ( rand() % epsilon  - halfEps ) / 10000.0 );

    if( sum - curProb >= 0 )
    {
      probas.push_back( curProb );
      sum -= curProb;
    }
    else if( sum > 0 )
    {
      probas.push_back( sum );
      sum = 0;
    }
  }

  probas.push_back( sum );
  sum = 0;

#ifdef DEBUG_RAND
  std::cout << "Vector of probabilities : " << std::endl;
  for( int i = 0 ; i < nProba ; i++ )
    std::cout << probas[ i ] << std::endl;

  std::cout << "Left-over : " << sum << std::endl;
#endif

  return probas;
}

// class HMM implementation
int HMM::N_OBS = -1;
std::map< uint8_t, int > HMM::evidencesHashes;


void HMM::PopulateEvidencesHashes()
{
  int i = 0;

  // the "all stop" case. / Can it really happen ? TODO
  evidencesHashes.insert( std::pair< uint8_t, int >(
    HashEvidence( ACTION_STOP, ACTION_STOP, BIRD_STOPPED ), i++ ) );

  // the "going horizontal" cases
  for( int iH = 0 ; iH < ACTION_STOP ; iH++ )
  {
    evidencesHashes.insert( std::pair< uint8_t, int >(
      HashEvidence( (EAction)iH, ACTION_STOP, MOVE_EAST ), i++ ) );
    evidencesHashes.insert( std::pair< uint8_t, int >(
      HashEvidence( (EAction)iH, ACTION_STOP, MOVE_WEST ), i++ ) );
  }

  // the "going vertical" cases
  for( int iV = 0 ; iV < ACTION_STOP ; iV++ )
  {
    evidencesHashes.insert( std::pair< uint8_t, int >(
      HashEvidence( ACTION_STOP, (EAction)iV, MOVE_UP ), i++ ) );
    evidencesHashes.insert( std::pair< uint8_t, int >(
      HashEvidence( ACTION_STOP, (EAction)iV, MOVE_DOWN ), i++ ) );
  }

  // general case
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
  std::cout << "Hashes for possible evidences : " << std::endl;
  std::map< uint8_t, int >::const_iterator itEv;
  for( itEv = evidencesHashes.begin() ; itEv != evidencesHashes.end() ; itEv++)
    std::cout << (int)itEv->first << '\t' << itEv->second <<std::endl;
#endif
}

uint8_t HMM::HashEvidence( CAction const & action )
{
  return HashEvidence( action.GetHAction(),
                       action.GetVAction(),
                       action.GetMovement() );
}

uint8_t HMM::HashEvidence
(
  EAction actionH,
  EAction actionV,
  EMovement move
)
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

void HMM::Learn( CDuck const & duck )
{
#ifdef DEBUG
  std::cout << "HMM::Learn" << std::endl;
#endif

  double oldLikelyhood = -1e10; // something very negative;
  double newLikelyhood = oldLikelyhood;

  int duckSeqLength = duck.GetSeqLength();
  int duckNumber    = duck.GetAction( 0 ).GetBirdNumber();
  int evidenceIdx;

  // hashes for the given sequence of evidences
  std::vector< uint8_t > hashedEvidences( duckSeqLength, 0 );

  // scaling factors
  std::vector< double > scalFactors( duckSeqLength, 1 );

  // each column is an xxx vector for a given t, from 0 to T-1
  std::vector< std::vector< double > > alphas;
  std::vector< std::vector< double > > betas;
  std::vector< std::vector< double > > diGammas;
  std::vector< std::vector< double > > gammas;

  // initialize alphas and betas arrays
  for( int i = 0 ; i < duckSeqLength ; i++ )
    alphas.push_back( std::vector< double >( B_N_BEHAVIORS, 0 ) );

  for( int i = 0 ; i < duckSeqLength ; i++ )
    betas.push_back( std::vector< double >( B_N_BEHAVIORS, 0 ) );

  // get all hashes for the observations sequence
  for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    hashedEvidences[ iSeq ] = HashEvidence( duck.GetAction( iSeq ) );

  do
  {
    Forward( alphas, scalFactors, duckSeqLength-1, hashedEvidences );
    Backward( betas, scalFactors, 0, duckSeqLength-1, hashedEvidences );

    ComputeGammas( diGammas, gammas, alphas, betas, hashedEvidences );

    UpdateModel( diGammas, gammas, hashedEvidences );

    oldLikelyhood = newLikelyhood;

    double newLikelyhood = ComputeNewLikelyhood( scalFactors );

#ifdef DEBUG
    std::cout << "Scaling factors from 0 to T-1" << std::endl;
    for( int i = 0 ; i < duckSeqLength ; i++ )
      std::cout << scalFactors[ i ] << std::endl;

    std::cout << "Alphas from 0 to T-1 : " << std::endl;
    for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    {
      for( int ai = 0 ; ai < B_N_BEHAVIORS ; ai++ )
        std::cout << alphas[ iSeq ][ ai ] << '\t';

      std::cout << std::endl;
    }
    std::cout << "Betas from 0 to T-1 : " << std::endl;
    for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    {
      for( int bi = 0 ; bi < B_N_BEHAVIORS ; bi++ )
        std::cout << betas[ iSeq ][ bi ] << '\t';

      std::cout << std::endl;
    }
    std::cout << "New PI" << std::endl;
    PrintMatrix( PI, 1, B_N_BEHAVIORS );
    CheckSum( PI, 1, B_N_BEHAVIORS );

    std::cout << std::endl << "New Transitions" << std::endl;
    PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
    CheckSum( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );

    std::cout << std::endl << "New Evidences" << std::endl;
    PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
    CheckSum( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
#endif

    std::cout << "Old LH is "  << oldLikelyhood
              << " new LH is " << newLikelyhood << std::endl;
    std::cout << (newLikelyhood > oldLikelyhood) << std::endl;

  } while( newLikelyhood > oldLikelyhood );
}

CAction HMM::Predict( CDuck const & duck ) const
{
  // do clever stuff to predict next move of the duck
}

void HMM::InitTheMatrixes()
{
  // In which we init/populate the various matrixes needed throughout the
  // hunting.

  // First the initial state matrix and the transition matrix
  PI.reserve( B_N_BEHAVIORS );

  TransitionMatrix.reserve( B_N_BEHAVIORS * B_N_BEHAVIORS );

  std::vector< double > initPi = GenerateUniformNoisyProba( B_N_BEHAVIORS );
  std::vector< double > initTr;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    PI[ i ] = initPi[ i ];

    initTr = GenerateUniformNoisyProba( B_N_BEHAVIORS );

    for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
    {
      TransitionMatrix[ j + ( i * B_N_BEHAVIORS ) ] = initTr[ j ];
    }
  }

  // now we fill the evidences matrix
  assert( N_OBS > 0 );

  EvidenceMatrix.reserve( B_N_BEHAVIORS * N_OBS );

  // at the beginning we assume for every state that we could equally
  // observe every evidence in it. (Of course this is not true but that's
  // what learning is for)
  std::vector< double > initObs;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    for( int j = 0 ; j < N_OBS ; j++ )
    {
      initObs = GenerateUniformNoisyProba( N_OBS );

      EvidenceMatrix[ j + ( N_OBS * i ) ] = initObs[ j ];
    }
  }

#ifdef DEBUG
  std::cout << "PI" << std::endl;
  PrintMatrix( PI, 1, B_N_BEHAVIORS );
  CheckSum( PI, 1, B_N_BEHAVIORS );
  std::cout << std::endl << "Transitions" << std::endl;
  PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
  CheckSum( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
  std::cout << std::endl << "Evidences" << std::endl;
  PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
  CheckSum( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
#endif
}

std::vector< double >  HMM::Forward
(
  std::vector< std::vector< double > > & alphas,
  std::vector< double >                & scalFactors,
  int t,
  std::vector< uint8_t > const & observations
)
{
#ifdef DEBUG_FW
  std::cout << "Entering Forward routine with t = " << t << std::endl;
#endif

  double scalingFactor = 0;

  std::vector< double > alphaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = evidencesHashes[ observations[ t ] ];

  if( t == 0 )
  {
#ifdef DEBUG_FW
    std::cout << "FW, at t = " << t << " hits end of recursion" << std::endl;
#endif

    // end of recursion
    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      alphaT[ i ] = PI[ i ] * EvidenceMatrix[ evidenceIdx + ( i * N_OBS ) ];
      scalingFactor += alphaT[ i ];
    }
  }
  else
  {
    // my, that's a yummy recursion !
    std::vector< double > alphaPrev = Forward( alphas, scalFactors,
                                         t - 1, observations );

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
        alphaT[ i ] += alphaPrev[ j ] * TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ];

      alphaT[ i ] *= EvidenceMatrix[ evidenceIdx + ( i * N_OBS ) ];

      scalingFactor += alphaT[ i ];
    }
  }

  // do the actual scaling
  scalingFactor = 1 / scalingFactor;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    alphaT[ i ] *= scalingFactor;

#ifdef DEBUG_FW
  std::cout << "Alpha at t = " << t << std::endl;
  PrintMatrix( alphaT, 1, B_N_BEHAVIORS );
#endif

  // populate the scaling factor array
  scalFactors[ t ] = scalingFactor;

  // populate the alphas matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    alphas[ t ][ i ] = alphaT[ i ];
  }

  return alphaT;
}

std::vector< double > HMM::Backward
(
  std::vector< std::vector< double > > & betas,
  std::vector< double > const & scalFactors,
  int t,
  int lastT,
  std::vector< uint8_t > const & observations
)
{
#ifdef DEBUG_BW
  std::cout << "Entering Backward routine with t = " << t << std::endl;
#endif

  std::vector< double > betaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = evidencesHashes[ observations[ t ] ];

  if( t == lastT )
  {
#ifdef DEBUG_BW
    std::cout << "BW, at t = " << t << " hits end of recursion" << std::endl;
#endif
    // end of recursion
    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      betaT[ i ] = 1;
    }
  }
  else
  {
    // recursion... me liek it
    std::vector< double > betaNext = Backward( betas, scalFactors, t+1, 
                                        lastT, observations );

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
      {
        betaT[ i ] = TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] * EvidenceMatrix[ evidenceIdx + ( j * N_OBS ) ] * betaNext[ j ];
      }
    }
  }

  // do the scaling
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    betaT[ i ] *= scalFactors[ t ];

#ifdef DEBUG_BW
  std::cout << "Beta at t = " << t << std::endl;
  PrintMatrix( betaT, 1, B_N_BEHAVIORS );
#endif

  // populate the betas matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    betas[ t ][ i ] = betaT[ i ];
  }

  return betaT;
}

void HMM::ComputeGammas
(
  std::vector< std::vector< double > >       & diGammas,
  std::vector< std::vector< double > >       & gammas,
  std::vector< std::vector< double > > const & alphas,
  std::vector< std::vector< double > > const & betas,
  std::vector< uint8_t >               const & observations
)
  const
{
  double denominator;
  double curGammaI;
  int    evidenceIdx;

  int duckSeqLength = observations.size();

  for( int t = 0 ; t < duckSeqLength - 1 ; t++ ) // carefull, there are T-1 elements in gammas and diGammas
  {
    evidenceIdx = evidencesHashes[ observations[ t + 1 ] ];

    diGammas.push_back( std::vector< double >( B_N_BEHAVIORS * B_N_BEHAVIORS ) );
    gammas.push_back( std::vector< double >( B_N_BEHAVIORS ) );

    denominator = 0;

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
      {
       denominator += alphas[ t ][ i ] *
                       TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] *
                       EvidenceMatrix[ evidenceIdx + ( N_OBS * j ) ] *
                       betas[ t + 1 ][ j ];
      }
    }

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      curGammaI = 0;

      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
      {
        diGammas[ t ][ j + ( B_N_BEHAVIORS * i ) ] =
          ( alphas[ t ][ i ] *
            TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] *
            EvidenceMatrix[ evidenceIdx + ( N_OBS * j ) ] *
            betas[ t + 1 ][ j ] )
          /
          denominator;   // ugly, ugly, uglyyyy

        curGammaI += diGammas[ t ][ j + ( B_N_BEHAVIORS * i ) ];
      }

      gammas[ t ][ i ] = curGammaI;
    }
  }
}

void HMM::UpdateModel
(
  std::vector< std::vector< double > > const & diGammas,
  std::vector< std::vector< double > > const & gammas,
  std::vector< uint8_t  >              const & observations
)
{
#ifdef DEBUG
  std::cout << "HMM::UpdateModel()" << std::endl;
#endif

  // update PI
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    PI[ i ] = gammas[ 0 ][ i ];

  // update the transition matrix

  int evidenceIdx;

  double numerator;
  double denominator;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
    {
      numerator = denominator = 0;

      for( int t = 0 ; t < gammas.size() ; t++ )
      {
        numerator   += diGammas[ t ][ j + ( B_N_BEHAVIORS * i ) ];
        denominator += gammas[ t ][ i ];
      }

      TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] = numerator / denominator;
    }
  }

  // update the evidence matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    for( int j = 0 ; j < N_OBS ; j++ )
    {
      numerator = denominator = 0;

      for( int t = 0 ; t < gammas.size() ; t++ )
      {
        evidenceIdx = evidencesHashes[ observations[ t ] ];

        if( evidenceIdx == j )
          numerator += gammas[ t ][ i ];

        denominator += gammas[ t ][ i ];
      }

      EvidenceMatrix[ j + ( N_OBS * i ) ] = numerator / denominator;
    }
  }
}

double HMM::ComputeNewLikelyhood
(
  std::vector< double > const & scalFactors
)
  const
{
  double logProb = 0;

  for( int t = 0 ; t < scalFactors.size() ; t++ )
  {
    logProb += log( scalFactors[ t ] );
  }

  return -logProb;
}


// CPlayer implementation

CPlayer::CPlayer()
{
  srand( time( NULL ) );

  GenerateUniformNoisyProba( 4 );

  HMM::PopulateEvidencesHashes();
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
      std::cout << "\tCreating an HMM and learning duck " << i << std::endl;
#endif
      HMM model;

      model.Learn( pDucks[ i ] );
    }
  }
}

void CPlayer::Hit(int pDuck,ESpecies pSpecies)
{
  std::cout << "HIT DUCK!!!\n";
}

/*namespace ducks*/ }
