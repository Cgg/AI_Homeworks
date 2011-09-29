#include "cplayer.h"
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <math.h>

//#define DEBUG
//#define DEBUG_PRED
//#define DEBUG_GAM
//#define DEBUG_EXT
//#define DEBUG_RAND
//#define DEBUG_FW
//#define DEBUG_BW

namespace ducks
{

// Local methods implementation
void PrintMatrix( std::vector< PROB > const & theMatrix, int nRow, int nCol )
{
  for( int i = 0 ; i < nRow ; i++ )
  {
    for( int j = 0 ; j < nCol ; j++ )
      std::cerr << theMatrix[ ( i * nCol ) + j ] << '\t' ;

    std::cerr << std::endl;
  }
}

void CheckSum( std::vector< PROB > const &probaMatrix, int nRow, int nCol )
{
  PROB rowSum;

  for( int i = 0 ; i < nRow ; i++ )
  {
    rowSum = 0;

    for( int j = 0 ; j < nCol ; j++ )
    {
      rowSum += probaMatrix[ j + ( nCol * i ) ];
    }

    std::cerr << "sum of row " << i << " is " << rowSum << std::endl;
  }
}

std::vector< PROB > GenerateUniformNoisyProba( int nProba )
{
  std::vector< PROB > probas;

  PROB curProb;

  PROB base = 1 / ( PROB )nProba;

  int epsilon = floor( ( base / 10.0 ) * 10000 );
  int halfEps = epsilon / 2;

  PROB sum = 1.0;

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
  std::cerr << "Vector of probabilities : " << std::endl;
  for( int i = 0 ; i < nProba ; i++ )
    std::cerr << probas[ i ] << std::endl;

  std::cerr << "Left-over : " << sum << std::endl;
#endif

  return probas;
}

// class HMM implementation
int HMM::N_OBS = -1;
std::map< uint8_t, int > HMM::evidencesHashes;


void HMM::PopulateEvidencesHashes()
{
  int i = 0;

  /*
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
        HashEvidence( (EAction)iH, (EAction)iV, (EMovement)(MOVE_DOWN | MOVE_WEST) ), i++ ) );
    }
  }
  */
  for( int iH = 0 ; iH < ACTION_STOP+1 ; iH++ )
  {
    for( int iV = 0 ; iV < ACTION_STOP+1 ; iV++ )
    {
      // ugly, ugly, ugly
      evidencesHashes.insert( std::pair< uint8_t, int >(
        HashEvidence( (EAction)iH, (EAction)iV ), i++ ) );
    }
  }

  // Now we can populate the actual evidence matrix, giving for each state
  // the probability of observing an evidence, given we are in the said
  // state.

  N_OBS = evidencesHashes.size();

#ifdef DEBUG_INIT
  std::cerr << "Number of possible evidences : " << N_OBS << std::endl;
  std::cerr << "Hashes for possible evidences : " << std::endl;
  std::map< uint8_t, int >::const_iterator itEv;
  for( itEv = evidencesHashes.begin() ; itEv != evidencesHashes.end() ; itEv++)
  {
    std::cout << (int)itEv->first << '\t' << itEv->second << '\t' << std::endl;
    UnhashEvidence( itEv->first, 1 ).Print();
  }
#endif
}

uint8_t HMM::HashEvidence( CAction const & action )
{
  return HashEvidence( action.GetHAction(),
                       action.GetVAction() );
}

uint8_t HMM::HashEvidence
(
  EAction actionH,
  EAction actionV
)
{
  return actionH + ( actionV << 2 );
}

CAction HMM::UnhashEvidence( uint8_t hash, int birdNumber )
{
  EAction   actionH = ( EAction )( hash & 0x03 );
  EAction   actionV = ( EAction )( ( hash & 0x0C ) >> 2 );

  return CAction( birdNumber, actionH, actionV, 0 );
}

void HMM::Learn( CDuck const & duck, CTime const & due )
{
#ifdef DEBUG
  std::cerr << "HMM::Learn" << std::endl;
#endif

  int  nIterations   = 1;
  PROB oldLikelyhood = MINUS_INFINITY; // something very negative;
  PROB newLikelyhood = oldLikelyhood;

  int duckSeqLength = duck.GetSeqLength();
  int duckNumber    = duck.GetAction( 0 ).GetBirdNumber();

  // hashes for the given sequence of evidences
  std::vector< int > evidenceIdxs( duckSeqLength, 0 );

  // scaling factors
  std::vector< PROB > scalFactors( duckSeqLength, 1 );

  // each column is an xxx vector for a given t, from 0 to T-1
  std::vector< std::vector< PROB > > alphas( duckSeqLength, std::vector< PROB >( B_N_BEHAVIORS, 0 ) ) ;
  std::vector< std::vector< PROB > > betas( duckSeqLength, std::vector< PROB >( B_N_BEHAVIORS, 0 ) );
  std::vector< std::vector< PROB > > diGammas( duckSeqLength - 1, std::vector< PROB >( B_N_BEHAVIORS*B_N_BEHAVIORS, 0 ) );
  std::vector< std::vector< PROB > > gammas( duckSeqLength - 1, std::vector< PROB >( B_N_BEHAVIORS, 0 ) );

  // get all hashes for the observations sequence
  for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    evidenceIdxs[ iSeq ] = evidencesHashes[ HashEvidence( duck.GetAction( iSeq ) ) ];

  CTime mark;
  int64_t itTime;

  do
  {
    mark = due.GetCurrent();

    Forward( alphas, scalFactors, duckSeqLength-1, evidenceIdxs );
    Backward( betas, scalFactors, 0, duckSeqLength-1, evidenceIdxs );

    ComputeGammas( diGammas, gammas, alphas, betas, evidenceIdxs );

    UpdateModel( diGammas, gammas, evidenceIdxs );

    oldLikelyhood = newLikelyhood;

    newLikelyhood = ComputeNewLikelyhood( scalFactors );

#ifdef DEBUG_EXT
    std::cerr << "Scaling factors from 0 to T-1" << std::endl;
    for( int i = 0 ; i < duckSeqLength ; i++ )
      std::cerr << scalFactors[ i ] << std::endl;

    std::cerr << "Alphas from 0 to T-1 : " << std::endl;
    for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    {
      for( int ai = 0 ; ai < B_N_BEHAVIORS ; ai++ )
        std::cerr << alphas[ iSeq ][ ai ] << '\t';

      std::cerr << std::endl;
    }
    std::cerr << "Betas from 0 to T-1 : " << std::endl;
    for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    {
      for( int bi = 0 ; bi < B_N_BEHAVIORS ; bi++ )
        std::cerr << betas[ iSeq ][ bi ] << '\t';

      std::cerr << std::endl;
    }
#endif
#ifdef DEBUG
    std::cerr << "New PI" << std::endl;
    PrintMatrix( PI, 1, B_N_BEHAVIORS );
    CheckSum( PI, 1, B_N_BEHAVIORS );

    std::cerr << std::endl << "New Transitions" << std::endl;
    PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
    CheckSum( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );

    std::cerr << std::endl << "New Evidences" << std::endl;
    PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
    CheckSum( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );

    std::cerr << "At iteration " << nIterations
              << " Old LH is "  << oldLikelyhood
              << " new LH is " << newLikelyhood << std::endl;
#endif

    itTime = due.GetCurrent() - mark;

    nIterations++;

  } while( ( due - due.GetCurrent() > itTime ) && 
             newLikelyhood >= oldLikelyhood );

  std::cerr<<nIterations<<std::endl;
  /*
  PrintMatrix( PI, 1, B_N_BEHAVIORS );
  PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
  PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
  */
}

CAction HMM::Predict( CDuck const & duck ) const
{
#ifdef DEBUG_PRED
  std::cerr << "HMM::Predict" << std::endl;
#endif
  // do clever stuff to predict next move of the duck
  //
  // Compute likelyhood of the given sequence of N actions
  //
  // for each possible sequence of N+1 actions compute the likelyhood
  //
  // pick up the biggest

  PROB    maxLikehood = MINUS_INFINITY;
  PROB    curLikelyhood;
  uint8_t maxLikehoodHash = 10;

  int duckSeqLength = duck.GetSeqLength();
  int duckNumber    = duck.GetAction( 0 ).GetBirdNumber();
  int evidenceIdx;

  // hashes for the given sequence of evidences
  std::vector< int > observations( duckSeqLength, 0 );

  std::vector< PROB > scalFactors( duckSeqLength, 1 );

  std::vector< std::vector< PROB > > alphas( duckSeqLength, std::vector< PROB >( B_N_BEHAVIORS, 0 ) );

  for( int iSeq = 0 ; iSeq < duckSeqLength ; iSeq++ )
    observations[ iSeq ] = evidencesHashes[ HashEvidence( duck.GetAction( iSeq ) ) ];

  Forward( alphas, scalFactors, duckSeqLength - 1, observations );

#ifdef DEBUG_PRED
  std::cerr << ComputeNewLikelyhood( scalFactors ) << std::endl;
#endif

  // make room for the N+1 observation
  alphas.push_back( std::vector< PROB >( B_N_BEHAVIORS, 0 ) );
  scalFactors.push_back( 1 );

  std::map< uint8_t, int >::const_iterator itHashes;

  for( itHashes = evidencesHashes.begin() ;
       itHashes != evidencesHashes.end() ;
       itHashes++ )
  {
    scalFactors[ duckSeqLength ] = 0;

    // find index of action giving the maximum likehood
    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      alphas[ duckSeqLength ][ i ] = 0;

      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
      {
        alphas[ duckSeqLength ][ i ] +=
          alphas[ duckSeqLength - 1 ][ j ] *
          TransitionMatrix[ j + ( B_N_BEHAVIORS*i ) ];
      }

      alphas[ duckSeqLength ][ i ] *= 
        EvidenceMatrix[ itHashes->second + ( N_OBS * i ) ];

      scalFactors[ duckSeqLength ] += alphas[ duckSeqLength ][ i ];
    }

    scalFactors[ duckSeqLength ] = 1 / scalFactors[ duckSeqLength ];

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
      alphas[ duckSeqLength ][ i ] =
        alphas[ duckSeqLength ][ i ] * scalFactors[ duckSeqLength ];

    // compute likelyhood
    curLikelyhood = ComputeNewLikelyhood( scalFactors );

#ifdef DEBUG_PRED
    std::cerr << "For action " << (int)itHashes->first << " (idx " << itHashes->second << ") "
              << " last alpha is " << std::endl;
    PrintMatrix( alphas[ duckSeqLength ], 1, B_N_BEHAVIORS );
    std::cerr << "And last scale factor is " << scalFactors[ duckSeqLength ] << std::endl;
    std::cerr << "Likelyhood is " << curLikelyhood << std::endl;
#endif

    if( curLikelyhood > maxLikehood )
    {
#ifdef DEBUG_PRED
      std::cerr << ">>>Picking up current move" << std::endl;
#endif
      maxLikehood     = curLikelyhood;
      maxLikehoodHash = itHashes->first;
    }

#ifdef DEBUG_PRED
    std::cerr << std::endl;
#endif
  }

  // convert it back to CAction
  CAction act = UnhashEvidence( maxLikehoodHash,
                         duck.GetLastAction().GetBirdNumber() );

#ifdef DEBUG_PRED
  act.Print();
#endif

  return act;
}

void HMM::InitTheMatrixes()
{
  // In which we init/populate the various matrixes needed throughout the
  // hunting.

  // First the initial state matrix and the transition matrix
  PI.reserve( B_N_BEHAVIORS );

  TransitionMatrix.reserve( B_N_BEHAVIORS * B_N_BEHAVIORS );

  std::vector< PROB > initPi = GenerateUniformNoisyProba( B_N_BEHAVIORS );
  std::vector< PROB > initTr;

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
  std::vector< PROB > initObs;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    for( int j = 0 ; j < N_OBS ; j++ )
    {
      initObs = GenerateUniformNoisyProba( N_OBS );

      EvidenceMatrix[ j + ( N_OBS * i ) ] = initObs[ j ];
    }
  }

#ifdef DEBUG
  std::cerr << "PI" << std::endl;
  PrintMatrix( PI, 1, B_N_BEHAVIORS );
  CheckSum( PI, 1, B_N_BEHAVIORS );

  std::cerr << std::endl << "Transitions" << std::endl;
  PrintMatrix( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );
  CheckSum( TransitionMatrix, B_N_BEHAVIORS, B_N_BEHAVIORS );

  std::cerr << std::endl << "Evidences" << std::endl;
  PrintMatrix( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
  CheckSum( EvidenceMatrix, B_N_BEHAVIORS, N_OBS );
#endif
}

std::vector< PROB >  HMM::Forward
(
  std::vector< std::vector< PROB > > & alphas,
  std::vector< PROB >                & scalFactors,
  int t,
  std::vector< int > const & observations
)
  const
{
#ifdef DEBUG_FW
  std::cerr << "Entering Forward routine with t = " << t << std::endl;
#endif

  PROB scalingFactor = 0;

  std::vector< PROB > alphaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = observations[ t ];

  if( t == 0 )
  {
#ifdef DEBUG_FW
    std::cerr << "FW, at t = " << t << " hits end of recursion" << std::endl;
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
    std::vector< PROB > alphaPrev = Forward( alphas, scalFactors,
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

  // populate the scaling factor array
  scalFactors[ t ] = scalingFactor;

  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    alphaT[ i ] *= scalingFactor;

#ifdef DEBUG_FW
  std::cerr << "Alpha at t = " << t << std::endl;
  PrintMatrix( alphaT, 1, B_N_BEHAVIORS );
#endif

  // populate the alphas matrix
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
  {
    alphas[ t ][ i ] = alphaT[ i ];
  }

  return alphaT;
}

std::vector< PROB > HMM::Backward
(
  std::vector< std::vector< PROB > > & betas,
  std::vector< PROB > const & scalFactors,
  int t,
  int lastT,
  std::vector< int > const & observations
)
  const
{
#ifdef DEBUG_BW
  std::cerr << "Entering Backward routine with t = " << t << std::endl;
#endif

  std::vector< PROB > betaT( B_N_BEHAVIORS, 0 );

  // getting the index of evidence's hash
  int evidenceIdx = observations[ t ];

  if( t == lastT )
  {
#ifdef DEBUG_BW
    std::cerr << "BW, at t = " << t << " hits end of recursion" << std::endl;
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
    std::vector< PROB > betaNext = Backward( betas, scalFactors, t+1, 
                                        lastT, observations );

    for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    {
      for( int j = 0 ; j < B_N_BEHAVIORS ; j++ )
      {
        betaT[ i ] += TransitionMatrix[ j + ( B_N_BEHAVIORS * i ) ] * EvidenceMatrix[ evidenceIdx + ( j * N_OBS ) ] * betaNext[ j ];
      }
    }
  }

  // do the scaling
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    betaT[ i ] *= scalFactors[ t ];

#ifdef DEBUG_BW
  std::cerr << "Beta at t = " << t << std::endl;
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
  std::vector< std::vector< PROB > >       & diGammas,
  std::vector< std::vector< PROB > >       & gammas,
  std::vector< std::vector< PROB > > const & alphas,
  std::vector< std::vector< PROB > > const & betas,
  std::vector< int >             const & observations
)
  const
{
#ifdef DEBUG_GAM
  std::cerr << "HMM::ComputeGammas" << std::endl;
#endif

  PROB denominator;
  PROB curGammaI;
  int  evidenceIdx;

  int duckSeqLength = observations.size();

  for( int t = 0 ; t < duckSeqLength - 1 ; t++ ) // carefull, there are T-1 elements in gammas and diGammas
  {
    evidenceIdx = observations[ t + 1 ];

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

#ifdef DEBUG_GAM
  std::cerr << "DiGammas from 0 to T-2 : " << diGammas.size() - 1 << std::endl;
  for( int t = 0 ; t < diGammas.size() ; t++ )
  {
    PrintMatrix( diGammas[ t ], 1, B_N_BEHAVIORS * B_N_BEHAVIORS );
    CheckSum( diGammas[ t ], 1, B_N_BEHAVIORS * B_N_BEHAVIORS );
  }

  std::cerr << "Gammas from 0 to T-2 :" << gammas.size() - 1 << std::endl;
  for( int t = 0 ; t < gammas.size() ; t++ )
  {
    PrintMatrix( gammas[ t ], 1, B_N_BEHAVIORS );
    CheckSum( gammas[ t ], 1, B_N_BEHAVIORS );
  }
#endif
}

void HMM::UpdateModel
(
  std::vector< std::vector< PROB > > const & diGammas,
  std::vector< std::vector< PROB > > const & gammas,
  std::vector< int  >              const & observations
)
{
#ifdef DEBUG
  std::cerr << "HMM::UpdateModel()" << std::endl;
#endif

  // update PI
  for( int i = 0 ; i < B_N_BEHAVIORS ; i++ )
    PI[ i ] = gammas[ 0 ][ i ];

  // update the transition matrix

  int evidenceIdx;

  PROB numerator;
  PROB denominator;

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
        evidenceIdx = observations[ t ];
        //std::cerr << "at t = " << t << " we do obs " << (int)observations[ t ] << " which has idx " << evidenceIdx << std::endl;

        if( evidenceIdx == j )
        {
          numerator += gammas[ t ][ i ];
        }

        denominator += gammas[ t ][ i ];
      }

      EvidenceMatrix[ j + ( N_OBS * i ) ] = numerator / denominator;
    }
  }
}

PROB HMM::ComputeNewLikelyhood
(
  std::vector< PROB > const & scalFactors
)
  const
{
  PROB logProb = 0;

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

  HMM::PopulateEvidencesHashes();
}

CAction CPlayer::Shoot(const CState &pState,const CTime &pDue)
{
  /*
   * Here you should write your clever algorithms to get the best action.
   * This skeleton never shoots.
   */

  CDuck duck = pState.GetDuck( 0 );

#ifdef DEBUG
    std::cerr << "Guess Duck " << std::endl;
#endif

#ifdef DEBUG
    std::cerr << "\tCreating an HMM and learning duck " << std::endl;
#endif
    HMM model;

    model.Learn( duck, pDue );
    return model.Predict( duck );

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
  std::cerr << "Entering Guess" << std::endl;
#endif

}

void CPlayer::Hit(int pDuck,ESpecies pSpecies)
{
  std::cout << "HIT DUCK!!!\n";
}

/*namespace ducks*/ }
