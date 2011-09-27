#ifndef _DUCKS_CPLAYER_H_
#define _DUCKS_CPLAYER_H_

#include "ctime.h"
#include "cstate.h"
#include <vector>
#include <map>

namespace ducks
{

enum EBehavior
{
  B_MIGRATING,
  B_QUACKING,
  B_PANICKING,
  B_FEIGNING,     // feigning death (suckers)
  B_N_BEHAVIORS   // numering element
};

void PrintMatrix( std::vector< double > & theMatrix, int nRow, int nCol );
std::vector< double > GenerateUniformNoisyProba( int nProba );

class  HMM
{
  // All matrixes are row-wise stored, so to access element i,j of matrix A
  // the thing to do is A[ j + i*nCol ]

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
      EAction actionV,
      EMovement move
    );

  static CAction UnhashEvidence( uint8_t hash, int birdNumber );

  // public methods
  public:

    HMM(){ InitTheMatrixes(); }

    // Learn HMM parameters from a duck
    void Learn( CDuck const & duck );

    // Try to predict duck's next movements
    CAction Predict( CDuck const & duck ) const;

  // protected methods
  protected:

    void InitTheMatrixes();

    // WARNING the observations here are already hashed
    std::vector< double > Forward
    (
      int t,
      std::vector< uint8_t > const & observations
    );

    // where lastT represent T, current time at which the calculation is done
    std::vector< double > Backward
    (
      int t,
      int lastT,
      std::vector< uint8_t > const & observations
    );

  // protected data, HMM core
  protected:
    // Initial states probabilities, is 1 x B_N_BEHAVIORS
    std::vector< double > PI;

    // is B_N_BEHAVIORS x B_N_BEHAVIORS
    std::vector< double > TransitionMatrix;
    // is B_N_BEHAVIORS x N_OBS
    std::vector< double > EvidenceMatrix;

    // scaling factors
    std::vector< double > scalFactors;
    // each column is an alpha vector for a given t, from 0 to T-1
    std::vector< std::vector< double > > alphas;
    // each column is a beta vector for a given t, from 0 to T-1
    std::vector< std::vector< double > > betas;
    // each column is a gamma vector for a given t, from 0 to T-1
    std::vector< std::vector< double > > gammas;


};

class CPlayer
{
  public:

    ///constructor

    ///There is no data in the beginning, so not much should be done here.
    CPlayer();

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
};

/*namespace ducks*/ }

#endif
