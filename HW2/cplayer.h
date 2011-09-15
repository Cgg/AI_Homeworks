#ifndef _CHECKERS_CPLAYER_H_
#define _CHECKERS_CPLAYER_H_

#include "constants.h"
#include "ctime.h"
#include "cmove.h"
#include "cboard.h"
#include <vector>

namespace chk
{

enum Player
{
  MaxP,
  MinP
};

static const int64_t TIME_LIMIT = 5e5;

static const double SMALL = -1e6f;
static const double BIG   =  1e6f;

static const int DEPTH_L = 8;

static const double OWN_P_VAL = 1.0f;
static const double OWN_K_VAL = 2.0f;

static const double OWN_D_VAL = 1.3f;
static const double OWN_T_VAL = 1.6f;

static const double OTH_P_VAL = -1.3f;
static const double OTH_K_VAL = -2.5f;

static const double OTH_D_VAL = -1.4f;
static const double OTH_T_VAL = -1.8f;

class Node
{
  public:

    Node( CBoard const & board, CMove const & move ) :
      boardAtNode( board, move )
    {
      value = computeHeuristic();

#ifdef DEBUG
      std::cerr << "Node CTOR : value = " << value;
#endif
    }

    double AlphaBeta
    (
      bool   isMaxNode,
      CTime const & pDue,
      int    depthLimit,
      double alpha,
      double beta
    );

  private:

    double computeHeuristic();

    std::vector< Node > * expand( bool isMaxNode );

    static bool comp( Node const & n1, Node const n2 )
    {
      return ( n1.value < n2.value );
    }

  private:

    double value; // value of the node
    CBoard boardAtNode;
};

class CPlayer
{
public:
    ///constructor

    ///Shouldn't do much. Any expensive initialization should be in
    ///Initialize
    CPlayer();

    ///called when waiting for the other player to move:

    ///\param pBoard the current state of the board
    ///\return false if we don't want this function to be called again
    ///until next move, true otherwise
    bool Idle(const CBoard &pBoard);

    ///perform initialization of the player

    ///\param pFirst true if we will move first, false otherwise
    ///\param pDue time before which we must have returned. To check,
    ///for example, to check if we have less than 100 ms to return, we can check if
    ///CTime::GetCurrent()+100000>pDue or pDue-CTime::GetCurrent()<100000.
    ///All times are in microseconds.
    void Initialize(bool pFirst,const CTime &pDue);

    ///perform a move

    ///\param pBoard the current state of the board
    ///\param pDue time before which we must have returned
    ///\return the move we make
    CMove Play(const CBoard &pBoard,const CTime &pDue);
};

/*namespace chk*/ }

#endif
