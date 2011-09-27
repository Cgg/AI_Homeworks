#include "cclient.h"

#include <sstream>
#include <iostream>

namespace ducks {

CClient::CClient(CPlayer &pPlayer)
    :   mPlayer(pPlayer)
{
}

CClient::~CClient()
{
}

int CClient::ReadDucks()
{
    int lLines=0;
    int lLinesLeft;

    do
    {
        ++lLines;

        std::string lString;
        mSocket.ReadLine(lString,true);

        std::istringstream lS(lString);

        int lBirdCount;
        
        lS >> lLinesLeft >> lBirdCount;
        
        if(mState.mDucks.empty())
            mState.mDucks.resize(lBirdCount);
        else if(lBirdCount!=mState.mDucks.size())
            throw std::runtime_error("wrong response from server");          
            
        for(int i=0;i<lBirdCount;i++)
        {
            int lHorz,lVert,lMovement;
            lS >> lHorz >> lVert >> lMovement;
            mState.mDucks[i].PushBackAction(CAction(i,EAction(lHorz),EAction(lVert),EMovement(lMovement)));
        }

        if(!lS) 
            throw std::runtime_error("wrong response from server");
    } while(lLinesLeft);
    
    return lLines;
}

bool CClient::ReadState(CTime &pTime)
{    
    std::string lString;
    mSocket.ReadLine(lString,true);
            
    std::istringstream lS(lString);

    int64_t lTime;
    int lDuck;
    int lSpecies;
    
    lS >> lTime >> lDuck >> lSpecies;

    if(lDuck>=0)
    {
        mState.mDucks[lDuck].SetSpecies((ESpecies)lSpecies);
        mPlayer.Hit(lDuck,ESpecies(lSpecies));
    }

    int lEnd;
    int lPlayer;
    int lNumPlayers;

    lS >> lEnd >> lPlayer >> lNumPlayers;

    if(!lS) 
        throw std::runtime_error("wrong response from server (state)");

    pTime=CTime(lTime);
    
    mState.mWhoIAm=lPlayer;
        
    if(mState.mScores.empty())
        mState.mScores.resize(lNumPlayers);
    else if(lNumPlayers!=mState.mScores.size())
        throw std::runtime_error("wrong response from server");

    for(int i=0;i<lNumPlayers;i++)
    {
        lS >> mState.mScores[i];
    }

    if(!lS) 
        throw std::runtime_error("wrong response from server");

    return lEnd;
}

void CClient::ReadEOG()
{
    std::string lLine;
    mSocket.ReadLine(lLine);
    std::cout << lLine << "\n";
}

void CClient::WriteAction(const CAction &pAction)
{
    std::ostringstream lS;
    
    lS << pAction.GetBirdNumber() << ' ' << (int)pAction.GetHAction() << ' ' << (int)pAction.GetVAction() << ' ' << (int)pAction.GetMovement();

    mSocket.WriteLine(lS.str());
}

void CClient::WriteGuess()
{
    std::ostringstream lS;

    for(int i=0;i<mState.mDucks.size();i++)
    {
        if(i!=0) lS << ' ';
        lS << (int)mState.mDucks[i].GetSpecies();
    }

    mSocket.WriteLine(lS.str());
}

void CClient::Run(const std::string &pHost,const std::string &pPort,
                  const std::string &pArgs,bool pStandalone)
{
    mStandalone=pStandalone;
    mSocket.Init(pHost,pPort);
    mSocket.WriteLine("MODE "+pArgs);

    while(true)
    {
        CTime lTime;

        mState.mNumNewTurns=ReadDucks();

        if(ReadState(lTime))
        {
            if(mState.mDucks.size()==1)
            {
                if(mStandalone)
                    lTime=CTime::GetCurrent()+60000000;
                std::cout<<"Guessing..."<<std::endl;
                mPlayer.Guess(mState.mDucks,lTime);
                WriteGuess();
            }
            break;
        }
        else
        {
            if(mStandalone)
                lTime=CTime::GetCurrent()+1000000;
            CAction lAction=mPlayer.Shoot(mState,lTime);
            WriteAction(lAction);
        }
    }

    ReadEOG();
}

/*namespace ducks*/ }
