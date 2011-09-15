#ifndef _CHECKERS_CBOARD_H_
#define _CHECKERS_CBOARD_H_

#include "constants.h"
#include "cmove.h"
#include <stdint.h>
#include <cassert>
#include <cstring>
#include <iostream>

namespace chk {

///represents a position in an 8x8 board
class CBoard
{
public:
    static const int cSquares=32;		///< 32 valid squares
    static const int cPlayerPieces=12;	///< 12 pieces per player

    static double squareManCoeff[32];
    static double squareKingCoeff[32];

    ///if \p pInit is true, initializes the board to the starting position

    ///Otherwise, leave uninitialized
    explicit CBoard(bool pInit=true)
    {
        if(pInit)
        {
            for(int i=0;i<cPlayerPieces;i++)
            {
                mCell[i]=CELL_OWN;
                mCell[cSquares-1-i]=CELL_OTHER;
            }
            for(int i=cPlayerPieces;i<cSquares-cPlayerPieces;i++)
            {
                mCell[i]=CELL_EMPTY;
            }
        }
    }

    ///constructs a board which is the result of applying move \p pMove to board \p pRH

    /// \param pRH the starting board position
    /// \param pMove the movement to perform
    ///
    /// \sa DoMove()
    CBoard(const CBoard &pRH,const CMove &pMove)
    {
        memcpy(mCell,pRH.mCell,sizeof(mCell));

        DoMove(pMove);
    }

    ///returns the content of a cell in the board.

    ///Cells are numbered as follows:
    ///
    ///    31    30    29    28
    /// 27    26    25    24
    ///    23    22    21    20
    /// 19    18    17    16
    ///    15    14    13    12
    /// 11    10     9     8
    ///    7      6     5     4
    ///  3     2     1     0
    ///
    /// this function returns a byte representing the contents of the cell,
    /// using the enumeration values in ECell
    ///
    /// For example, to check if cell 3 contains an own piece, you would check if
    ///
    ///   (lBoard.At(3)&CELL_OWN)
    ///
    /// to check if it is a piece of the other player,
    ///
    ///   (lBoard.At(3)&CELL_OTHER)
    ///
    /// and to check if it is aking, you would check if
    ///
    ///   (lBoard.At(3)&CELL_KING)
    ///

    void GetPiecesCountWeighted( double & ownPieceCount, double & ownKingCount,
                                 double & otherPieceCount, double & otherKingCount ) const
    {
      for( int i = 0 ; i < cSquares ; i++ )
      {
        if( At( i )  & CELL_OWN )
        {
          if( At( i ) & CELL_KING )
          {
            ownKingCount += squareKingCoeff[ i ];
          }
          else
          {
            ownPieceCount += squareManCoeff[ i ];
          }
        }
        else if ( At( i ) & CELL_OTHER )
        {
          if( At( i ) & CELL_KING )
          {
            otherKingCount += squareKingCoeff[ i ];
          }
          else
          {
            otherPieceCount += squareManCoeff[ i ];
          }
        }
      }
    }

    uint8_t At(int pPos) const
    {
        assert(pPos<cSquares);
        return mCell[pPos];
    }

    ///operator version of the above function
    uint8_t operator()(int pPos) const
    {
        return At(pPos);
    }

    ///returns the content of a cell in the board.

    ///Rows are numbered (0 to 7) from the lower row in the board,
    ///as seen by the player this program is playing.
    ///
    ///Columns are numbered starting from the right (also 0 to 7).
    ///
    ///Cells corresponding to white squares in the board, which will
    ///never contain a piece, always return CELL_INVALID
    ///
    ///If the cell falls outside of the board, return CELL_INVALID
    ///
    ///You can use it in the same way as the version that requires a cell index
    uint8_t At(int pR,int pC) const
    {
        if(pR<0||pR>7||pC<0||pC>7) return CELL_INVALID;
        if((pR&1)==(pC&1)) return CELL_INVALID;
        return mCell[pR*4+(pC>>1)];
    }

    // returns the number of pieces (kings and men) for each player
    void GetPiecesCount( int & ownPieceCount, int & ownKingCount,
                         int & otherPieceCount, int & otherKingCount ) const
    {
      for( int i = 0 ; i < cSquares ; i++ )
      {
        if( At( i )  & CELL_OWN )
        {
          if( At( i ) & CELL_KING )
          {
            ownKingCount++;
          }
          else
          {
            ownPieceCount++;
          }
        }
        else if ( At( i ) & CELL_OTHER )
        {
          if( At( i ) & CELL_KING )
          {
            otherKingCount++;
          }
          else
          {
            otherPieceCount++;
          }
        }
      }
    }

private:
    ///private version of above function (allows modifying cells)
    uint8_t &PrivAt(int pR,int pC) const
    {
        return const_cast<uint8_t&>(mCell[pR*4+(pC>>1)]);
    }

public:
    ///operator version of the above function
    uint8_t operator()(int pR,int pC) const
    {
        return At(pR,pC);
    }

    ///returns the row corresponding to a cell index
    static int CellToRow(int pCell)
    {
        return (pCell>>2);
    }

    ///returns the col corresponding to a cell index
    static int CellToCol(int pCell)
    {
        int lC=(pCell&3)<<1;
        if(!(pCell&4))
            lC++;
        return lC;
    }

    ///returns the cell corresponding to a row and col

    ///It doesn't check if it corresponds to a black square in the board,
    ///or if it falls within the board.
    ///
    ///If it doesn't, the result is undefined, and the program is likely
    ///to crash
    static int RowColToCell(int pRow,int pCol)
    {
        return (pRow*4+(pCol>>1));
    }

private:
    ///tries to make a jump from a certain position of the board

    /// \param pMoves a vector where the valid moves will be inserted
    /// \param pOther the \ref ECell code corresponding to the player
    /// who is not making the move
    /// \param pR the row of the cell we are moving from
    /// \param pC the col
    /// \param pKing true if the moving piece is a king
    /// \param pBuffer a buffer where the list of jump positions is
    /// inserted (for multiple jumps)
    /// \param pDepth the number of multiple jumps before this attempt
    bool TryJump(std::vector<CMove> &pMoves,int pOther,int pR,int pC,
                 bool pKing,uint8_t *pBuffer,int pDepth=0) const
    {
        pBuffer[pDepth]=RowColToCell(pR,pC);
        bool lFound=false;

        //try capturing forward
        if(pOther==CELL_OTHER||pKing)
        {
            //try capturing right
            if((At(pR+1,pC-1)&pOther)&&At(pR+2,pC-2)==CELL_EMPTY)
            {
                lFound=true;
                uint8_t lOldValue=At(pR+1,pC-1);
                PrivAt(pR+1,pC-1)=CELL_EMPTY;
                TryJump(pMoves,pOther,pR+2,pC-2,pKing,pBuffer,pDepth+1);
                PrivAt(pR+1,pC-1)=lOldValue;
            }
            //try capturing left
            if((At(pR+1,pC+1)&pOther)&&At(pR+2,pC+2)==CELL_EMPTY)
            {
                lFound=true;
                uint8_t lOldValue=At(pR+1,pC+1);
                PrivAt(pR+1,pC+1)=CELL_EMPTY;
                TryJump(pMoves,pOther,pR+2,pC+2,pKing,pBuffer,pDepth+1);
                PrivAt(pR+1,pC+1)=lOldValue;
            }
        }
        //try capturing backwards
        if(pOther==CELL_OWN||pKing)
        {
            //try capturing right
            if((At(pR-1,pC-1)&pOther)&&At(pR-2,pC-2)==CELL_EMPTY)
            {
                lFound=true;
                uint8_t lOldValue=At(pR-1,pC-1);
                PrivAt(pR-1,pC-1)=CELL_EMPTY;
                TryJump(pMoves,pOther,pR-2,pC-2,pKing,pBuffer,pDepth+1);
                PrivAt(pR-1,pC-1)=lOldValue;
            }
            //try capturing left
            if((At(pR-1,pC+1)&pOther)&&At(pR-2,pC+2)==CELL_EMPTY)
            {
                lFound=true;
                uint8_t lOldValue=At(pR-1,pC+1);
                PrivAt(pR-1,pC+1)=CELL_EMPTY;
                TryJump(pMoves,pOther,pR-2,pC+2,pKing,pBuffer,pDepth+1);
                PrivAt(pR-1,pC+1)=lOldValue;
            }
        }

        if(!lFound&&pDepth>0)
        {
            pMoves.push_back(CMove(pBuffer,pDepth+1));
        }

        return lFound;
    }

    ///tries to make a move from a certain position

    /// \param pMoves vector where the valid moves will be inserted
    /// \param pCell the cell where the move is tried from
    /// \param pOther the \ref ECell code corresponding to the player
    /// who is not making the move
    /// \param pKing true if the piece is a king
    void TryMove(std::vector<CMove> &pMoves,int pCell,int pOther,bool pKing) const
    {
        int lR=CellToRow(pCell);
        int lC=CellToCol(pCell);
        //try moving forward
        if(pOther==CELL_OTHER||pKing)
        {
            //try moving right
            if(At(lR+1,lC-1)==CELL_EMPTY)
                pMoves.push_back(CMove(pCell,RowColToCell(lR+1,lC-1)));
            //try moving left
            if(At(lR+1,lC+1)==CELL_EMPTY)
                pMoves.push_back(CMove(pCell,RowColToCell(lR+1,lC+1)));
        }
        //try moving backwards
        if(pOther==CELL_OWN||pKing)
        {
            //try moving right
            if(At(lR-1,lC-1)==CELL_EMPTY)
                pMoves.push_back(CMove(pCell,RowColToCell(lR-1,lC-1)));
            //try moving left
            if(At(lR-1,lC+1)==CELL_EMPTY)
                pMoves.push_back(CMove(pCell,RowColToCell(lR-1,lC+1)));
        }
    }

public:
    /// returns a list of all valid moves for \p pWho

    /// \param pMoves a vector where the list of moves will be appended
    /// \param pWho the \ref ECell code (CELL_OWN or CELL_OTHER) of the
    /// player making the move
    void FindPossibleMoves(std::vector<CMove> &pMoves,int pWho) const
    {
        pMoves.clear();

        assert(pWho==CELL_OWN||pWho==CELL_OTHER);

        int lOther=pWho^(CELL_OWN|CELL_OTHER);

        bool lFound=false;
        int lPieces[cPlayerPieces];
        uint8_t lMoveBuffer[cPlayerPieces];
        int lNumPieces=0;
        for(int i=0;i<cSquares;i++)
        {
            //if it belongs to the player making the move
            if(At(i)&pWho)
            {
                bool lIsKing=At(i)&CELL_KING;

                if(TryJump(pMoves,lOther,CellToRow(i),CellToCol(i),lIsKing,
                                     lMoveBuffer))
                {
                    lFound=true;
                }
                else if(!lFound)
                {
                    lPieces[lNumPieces++]=i;
                }
            }
        }

        if(!lFound)
        {
            for(int k=0;k<lNumPieces;k++)
            {
                int lCell=lPieces[k];
                bool lIsKing=At(lCell)&CELL_KING;
                TryMove(pMoves,lCell,lOther,lIsKing);
            }
        }
    }

    ///transforms the board by performing a move

    ///it doesn't check that the move is valid, so you should only use
    ///it with moves returned by FindPossibleMoves

    /// \param pMove the move to perform
    void DoMove(const CMove &pMove)
    {
        if(pMove.IsJump())
        {
            int lSR=CellToRow(pMove[0]);
            int lSC=CellToCol(pMove[0]);

            for(int i=1;i<pMove.Length();i++)
            {
                int lDR=CellToRow(pMove[i]);
                int lDC=CellToCol(pMove[i]);

                mCell[pMove[i]]=mCell[pMove[i-1]];
                mCell[pMove[i-1]]=CELL_EMPTY;

                if((lDR==7&&(mCell[pMove[i]]&CELL_OWN))||
                   (lDR==0&&(mCell[pMove[i]]&CELL_OTHER)))
                    mCell[pMove[i]]|=CELL_KING;

                ///now we have to remove the other one
                if(lDR>lSR)
                {
                    if(lDC>lSC)
                        mCell[RowColToCell(lDR-1,lDC-1)]=CELL_EMPTY;
                    else
                        mCell[RowColToCell(lDR-1,lDC+1)]=CELL_EMPTY;
                }
                else
                {
                    if(lDC>lSC)
                        mCell[RowColToCell(lDR+1,lDC-1)]=CELL_EMPTY;
                    else
                        mCell[RowColToCell(lDR+1,lDC+1)]=CELL_EMPTY;
                }

                lSR=lDR;
                lSC=lDC;
            }
        }
        else if(pMove.IsNormal())
        {
            int lDR=CellToRow(pMove[1]);
            mCell[pMove[1]]=mCell[pMove[0]];
            mCell[pMove[0]]=CELL_EMPTY;

            if((lDR==7&&(mCell[pMove[1]]&CELL_OWN))||
               (lDR==0&&(mCell[pMove[1]]&CELL_OTHER)))
                mCell[pMove[1]]|=CELL_KING;
        }
    }

    ///prints the board

    ///Useful for debug purposes. Don't call it in the final version.
    void Print() const
    {
        for(int r=7;r>=0;r--)
        {
            for(int c=7;c>=0;c--)
            {
                if(c%2==r%2) //white
                {
                    std::cout << "\e[47m  \e[0m";
                }
                else if(At(r,c)&CELL_OWN)
                {
                    if(At(r,c)&CELL_KING)
                        std::cout << "\e[37;40m##\e[0m";
                    else
                        std::cout << "\e[37;40m()\e[0m";
                }
                else if(At(r,c)&CELL_OTHER)
                {
                    if(At(r,c)&CELL_KING)
                        std::cout << "\e[31;40m##\e[0m";
                    else
                        std::cout << "\e[31;40m()\e[0m";
                }
                else
                {
                    std::cout << "\e[37;40m  \e[0m";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    ///prints the board (no color version)

    ///Useful for debug purposes. Don't call it in the final version.
    void PrintNoColor() const
    {
        std::cout << "----------------\n";
        for(int r=7;r>=0;r--)
        {
            for(int c=7;c>=0;c--)
            {
                if(c%2==r%2) //white
                {
                    std::cout << "  ";
                }
                else if(At(r,c)&CELL_OWN)
                {
                    if(At(r,c)&CELL_KING)
                        std::cout << "WW";
                    else
                        std::cout << "ww";
                }
                else if(At(r,c)&CELL_OTHER)
                {
                    if(At(r,c)&CELL_KING)
                        std::cout << "RR";
                    else
                        std::cout << "rr";
                }
                else
                {
                    std::cout << "  ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "----------------\n";
    }

private:
    //this is a bit ugly, but is useful for the implementation of
    //FindPossibleMoves. It won't affect in single-threaded programs
    //and you're not allowed to use threads anyway
    mutable uint8_t mCell[cSquares];
};

/*namespace chk*/ }

#endif
