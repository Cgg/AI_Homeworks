#ifndef _CHECKERS_CMOVE_H_
#define _CHECKERS_CMOVE_H_

#include "constants.h"
#include <vector>
#include <string>
#include <sstream>

namespace chk {

///encapsulates a move

///in general, you should regard this as an opaque structure that you obtain
///from FindPossibleMoves, and provide to DoMove.
///
///The functions IsNormal() and Is Jump() can however be useful.
///
///The rest of the interface you can probably ignore it.
class CMove
{
public:
    enum EMoveType
    {
        MOVE_JUMP=1,   ///< a simple jump (numbers above that will represent multiple jumps)
        MOVE_NORMAL=0, ///< a normal move
        MOVE_EOG=-1,   ///< end of game
        MOVE_BOG=-2,   ///< beginning of game
        MOVE_NULL=-3   ///< a null move
    };

public:
    ///constructs a special type move

    ///\param pType should be one of MOVE_EOF or MOVE_BOG
    CMove(EMoveType pType=MOVE_NULL)
        :   mType(pType)
    {
    }

    ///constructs a normal move (not a jump)

    ///\param p1 the source square
    ///\param p2 the destination square
    CMove(uint8_t p1,uint8_t p2)
        :	mType(MOVE_NORMAL)
    {
        uint8_t lData[2]={p1,p2};
        mData.assign(lData,2);
    }

    ///constructs a jump move

    ///\param pData a series of squares that form the sequence of jumps
    ///\param pLen the number of squares in pData
    CMove(uint8_t *pData,std::size_t pLen)
        :	mType(pLen-1)
        ,   mData(pData,pLen)
    {
    }

    ///reconstructs the move from a string

    ///\param pString a string, which should have been previously generated
    ///by ToString(), or obtained from the server
    CMove(const std::string &pString)
    {
        std::istringstream lStream(pString);

        lStream >> mType;

        int lLen=0;

        if(mType==MOVE_NORMAL)
            lLen=2;
        else if(mType>0)
            lLen=mType+1;
        else if(mType==MOVE_EOG)
            lLen=1;

        if(lLen>12||mType<-3)
        {
            mType=MOVE_NULL;
            return;
        }

        mData.resize(lLen);

        for(int i=0;i<lLen;i++)
        {
            int lCell;
            lStream >> lCell;
            if(lCell<0||lCell>31)
            {
                mType=MOVE_NULL;
                break;
            }

            mData[i]=lCell;
        }

        if(!lStream)
        {
            mType=MOVE_NULL;
            return;
        }
    }

    ///returns true if the movement is null or invalid
    bool IsNull() const			 {	 return (mType==MOVE_NULL); }
    ///returns true if the movement marks beginning of game
    bool IsBOG() const           {   return (mType==MOVE_BOG);   }
    ///returns true if the movement marks end of game
    bool IsEOG() const           {   return (mType==MOVE_EOG);   }
    ///returns true if the movement is a jump
    bool IsJump() const			 {	 return (mType>0);			}
    ///returns true if the movement is a normal move
    bool IsNormal() const		 {	 return (mType==MOVE_NORMAL);	}

    ///returns the type of the move
    int GetType() const			 {	 return mType;				}

    ///returns (for normal moves and jumps) the number of squares
    std::size_t Length() const   {   return mData.length();    }
    ///returns the pNth square in the sequence
    uint8_t operator[](int pN) const   {   return mData[pN];    }

    ///transforms the moves from "seen by one player" to "seen by the other"

    ///It is used internally by the server, and there is no reason to use it in client code
    void Invert()
    {
        for(int i=0;i<mData.length();i++)
        {
            mData[i]=31-mData[i];
        }
    }

    ///converts the move to a string so that it can be sent to the server
    std::string ToString() const
    {
        std::ostringstream lStream;
        lStream << mType;
        for(int i=0;i<mData.size();i++)
        {
            lStream << ' ' << (int)mData[i];
        }

        return lStream.str();
    }

    ///returns true if the two objects represent the same move
    bool operator==(const CMove &pRH) const
    {
        if(mType!=pRH.mType) return false;
        if(mData.length()!=pRH.mData.length()) return false;

        for(int i=0;i<mData.length();i++)
            if(mData[i]!=pRH.mData[i]) return false;
        return true;
    }

private:
    int mType;
    std::basic_string<uint8_t> mData;
};

/*namespace chk*/ }

#endif
