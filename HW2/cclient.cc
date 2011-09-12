#include "cclient.h"

#include <sstream>

namespace chk {

CClient::CClient(CPlayer &pPlayer)
    :   mPlayer(pPlayer)
{
}

CClient::~CClient()
{
}

void CClient::ReadInit(CTime &pTime,bool &pFirst)
{
    std::string lString;
    mSocket.ReadLine(lString);
    std::istringstream lS(lString);
    int64_t lTime;
    int lFirst;
    lS >> lTime >> lFirst;
    if(!lS) 
        throw std::runtime_error("wrong response from server");

    pTime=CTime(lTime);
    pFirst=(lFirst!=0);
}

bool CClient::ReadMove(CTime &pTime,CMove &pMove,bool pBlock)
{
    std::string lString;
    if(!mSocket.ReadLine(lString,pBlock))
        return false;
    
    int lSpace=lString.find(' ');
    if(lSpace==std::string::npos) throw std::runtime_error("wrong response from server");

    std::istringstream lS(lString);
    int64_t lTime;
    lS >> lTime;
    if(!lS) 
        throw std::runtime_error("wrong response from server");
    
    pTime=CTime(lTime);
    pMove=CMove(lString.substr(lSpace+1));
    return true;
}

void CClient::Run(const std::string &pHost,const std::string &pPort,
                  const std::string &pKey)
{
    mSocket.Init(pHost,pPort);
    if(pKey.empty())
    {
        mStandalone=true;
        mSocket.WriteLine("MODE STANDALONE");
    }
    else
    {
        mStandalone=false;
        mSocket.WriteLine("MODE GAME "+pKey);
    }

    //receive the answer
    bool lFirst; //will be true if we play first
    CTime lTime; //time when initialization must be done
    ReadInit(lTime,lFirst);
    
    if(mStandalone)
        lTime=CTime::GetCurrent()+19000000;

    mPlayer.Initialize(lFirst,lTime);

    mSocket.WriteLine("INIT");
    
    while(true)
    {
        CMove lMove;

        while(!ReadMove(lTime,lMove,false))
        {
            if(!mPlayer.Idle(mBoard))
            {
                ReadMove(lTime,lMove,true);
                break;
            }
        }

        if(mStandalone)
            lTime=CTime::GetCurrent()+9000000;
        
        if(lMove.IsEOG())
        {
            if(lMove.Length())
            {
                switch(lMove[0])
                {
                case 1:
                    std::cout << "YOU WIN\n";
                    break;
                case 2:
                    std::cout << "YOU LOSE\n";
                    break;
                case 3:
                    std::cout << "DRAW\n";
                    break;
                default:
                    std::cout << "INVALID GAME\n";
                }
            }
            return;
        }

        mBoard.DoMove(lMove);

        lMove=mPlayer.Play(mBoard,lTime);
        
        mSocket.WriteLine(lMove.ToString());
        
        if(lMove.IsEOG())
            return;
        
        mBoard.DoMove(lMove);
    }
}

/*namespace chk*/ }
