#ifndef _DUCKS_CPLAYER_H_
#define _DUCKS_CPLAYER_H_

#include "ctime.h"
#include "cstate.h"
#include <vector>
#include <map>

namespace ducks
{

typedef long double PROB;

enum EBehavior
{
  B_MIGRATING,
  B_QUACKING,
  B_PANICKING,
//  B_FEIGNING,     // feigning death (suckers)
  B_N_BEHAVIORS   // numering element
};

enum EClassification
{
  C_SAFE,
  C_UNSAFE
};

struct SPrediction
{
  SPrediction( PROB prob, CAction act ) :
    predictionProb( prob ),
    theAction( act )
  {}

  PROB predictionProb;
  CAction theAction;
};

static const SPrediction DONT_SHOOT( 0, cDontShoot );

void PrintMatrix( std::vector< PROB > const & theMatrix, int nRow, int nCol );
std::vector< PROB > GenerateUniformNoisyProba( int nProba );
void CheckSum( std::vector< PROB > const & probaMatrix, int nRow, int nCol );

class  HMM
{
  // All matrixes are row-wise stored, so to access element i,j of matrix A
  // the thing to do is A[ j + i*nCol ]

  // constants
  public:

    static PROB    const LEARN_TRESHOLD = 0.000001;
    static PROB    const MINUS_INFINITY = -1e42;
    static int64_t const OVERTIME       = 10000;

  // static data, common to all HMM
  public:

    static int N_OBS;
    static std::map< uint8_t, int > evidencesHashes;

  // static methods, helpers
  public:

    // initialization of static data, to be called from CPlayer CTOR
    static void PopulateEvidencesHashes();

    // traduction between an action and its hash
    static uint8_t HashEvidence( CAction const & action );
    static uint8_t HashEvidence
    (
      EAction actionH,
      EAction actionV
    );

  static CAction UnhashEvidence( uint8_t hash, int birdNumber );

  // public methods
  public:

    HMM();

    // Learn HMM parameters from a duck
    bool Learn( CDuck const & duck, CTime const & due );

    // Try to predict duck's next movements
    SPrediction Predict( CDuck const & duck ) const;

    // This one is used in one- and multi-player mode. It involves more
    // computation and can return cDontShoot if there is no best options.
    SPrediction PredictShoot( CDuck const & duck ) const;

    bool operator==( HMM const & other ) const
    {
      return ( ( hasBOne == other.hasBOne ) &&
               ( hasBTwo == other.hasBTwo ) &&
               ( hasFeigningDeath == other.hasFeigningDeath ) &&
               ( hasMigrating == other.hasMigrating ) );
    }

  // protected methods
  protected:

    void InitTheMatrixes();

    // WARNING the observations here are already hashed
    std::vector< PROB > Forward
    (
      std::vector< std::vector< PROB > > & alphas,
      std::vector< PROB >                & scalFactors,
      int t,
      std::vector< int > const & observations
    ) const;

    // where lastT represent T, current time at which the calculation is done
    std::vector< PROB > Backward
    (
      std::vector< std::vector< PROB > > & betas,
      std::vector< PROB > const & scalFactors,
      int t,
      int lastT,
      std::vector< int > const & observations
    ) const;

    void ComputeGammas
    (
      std::vector< std::vector< PROB > >       & diGammas,
      std::vector< std::vector< PROB > >       & gammas,
      std::vector< std::vector< PROB > > const & alphas,
      std::vector< std::vector< PROB > > const & betas,
      std::vector< int >               const & observations
    ) const;

    void UpdateModel
    (
      std::vector< std::vector< PROB > > const & diGammas,
      std::vector< std::vector< PROB > > const & gammas,
      std::vector< int >               const & observations
    );

    PROB ComputeNewLikelyhood( std::vector< PROB > const & scalFactor ) const;

    // Analyse the evidence matrix to (try to) determine which behaviors
    // the duck has
    void AnalyseEvidenceMatrix();

  // protected data, HMM core
  protected:
    // Initial states probabilities, is 1 x B_N_BEHAVIORS
    std::vector< PROB > PI;

    // is B_N_BEHAVIORS x B_N_BEHAVIORS
    std::vector< PROB > TransitionMatrix;
    // is B_N_BEHAVIORS x N_OBS
    std::vector< PROB > EvidenceMatrix;

    bool hasBOne;
    bool hasBTwo;
    bool hasFeigningDeath;
    bool hasMigrating;

    bool isWellKnown;
};

class CPlayer
{
  public:

    static int const learningWatchdog = 20;
    static int const TURN_LIMIT = 100;

    ///constructor

    ///There is no data in the beginning, so not much should be done here.
    CPlayer();

    ~CPlayer();

    ///shoot!

    ///This is the function where you should do all your work.
    ///
    ///you will receive a variable pState, which contains information about all ducks,
    ///both dead and alive. Each duck contains all past actions.
    ///
    ///The state also contains the scores for all players and the number of
    ///time steps elapsed since the last time this function was called.
    ///
    ///Check their documentation for more information.
    ///\param pState the state object
    ///\param pDue time before which we must have returned
    ///\return the position we want to shoot at, or cDontShoot if we
    ///prefer to pass
    CAction Shoot(const CState &pState,const CTime &pDue);

    ///guess the species!

    ///This function will be called at the end of the game, to give you
    ///a chance to identify the species of the surviving ducks for extra
    ///points.
    ///
    ///For each alive duck in the vector, you must call the SetSpecies function,
    ///passing one of the ESpecies constants as a parameter
    ///\param pDucks the vector of all ducks. You must identify only the ones that are alive
    ///\param pDue time before which we must have returned
    void Guess(std::vector<CDuck> &pDucks,const CTime &pDue);

    ///This function will be called whenever you hit a duck.
    ///\param pDuck the duck index
    ///\param pSpecies the species of the duck (it will also be set for this duck in pState from now on)
    void Hit(int pDuck,ESpecies pSpecies);

    protected:

    std::vector< HMM * > markov;

    int elapsedTurns;

    int aliveDucks;
    int learnedBirds;

    int nextBirdToLearn;

    std::vector< bool > learnedBirdsIdx;

    int lastShootedBird;
    bool shootSuccessfull;

    int shootedBirds;

    bool rampage;
};

/*namespace ducks*/ }

#endif
