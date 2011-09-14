#ifndef _CHECKERS_CPLAYER_H_
#define _CHECKERS_CPLAYER_H_

#include "constants.h"
#include "ctime.h"
#include "cmove.h"
#include "cboard.h"
#include <vector>

namespace chk
{

typedef double H_Type;

enum Player
{
  MaxPlayer,
  MinPlayer
};

class Node
{
  // static data
  public:

    static const int DL = 10; // depth limit for alphaBeta

    static H_Type Min;
    static H_Type Max;

  protected:

    static const H_Type OWN_P_VAL = 1;
    static const H_Type OTH_P_VAL = -2;
    static const H_Type OWN_K_VAL = 2;
    static const H_Type OTH_K_VAL = -4;

  // static methods
  public:

    static H_Type AlphaBeta
    (
      Node   & origin,
      int      depth,
      H_Type   alpha,
      H_Type   beta,
      Player   player
    );

  protected:

    static std::vector< Node > * expand
    (
      Node                & origin, // node from where we extend
      Player                player  // who shall play (min or max)
    );

    static H_Type computeHeuristic
    (
      CBoard const & board
    );

    static bool comp
    (
      Node const & n1, Node const & n2
    )
    {
      return ( n1.value < n2.value );
    }

  // non static methods and data
  public:

    Node( CMove * move, CBoard * board )
      : moveToGetHere( move ), boardAtNode( board )
    {
      value = computeHeuristic( *boardAtNode );

      //std::cout << "initialization value = " << value << std::endl;
    }

    ~Node()
    {
      delete moveToGetHere;
      delete boardAtNode;
    }

    Node( Node const & rNode )
    {
      value = rNode.value;
      moveToGetHere = new CMove(*(rNode.moveToGetHere));
      boardAtNode = new CBoard(*(rNode.boardAtNode));
    }

  private:

    H_Type value;
    CMove  * moveToGetHere;
    CBoard * boardAtNode;
};

class CPlayer
{
public:
    ///constructor

    ///Shouldn't do much. Any expensive initialization should be in
    ///Initialize
    CPlayer();

    ///called when waiting for the other player to move

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
