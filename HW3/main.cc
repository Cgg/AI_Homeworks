#include "cplayer.h"
#include "cclient.h"

#include <iostream>

int main(int pArgC,char **pArgs)
{
    if(pArgC<4)
    {
        std::cerr << "usage: " << pArgs[0] << " host port STANDALONE [options]" << std::endl;
        return -1;
    }

    ducks::CPlayer lPlayer;
    ducks::CClient lClient(lPlayer);

    std::string lArgs=pArgs[3];

    bool lStandalone=(lArgs=="STANDALONE");
    if(!lStandalone)
    {
        std::cerr << "this version can only run in STANDALONE mode\n";
        return false;
    }
    
    for(int i=4;i<pArgC;i++)
    {
        lArgs+=" ";
        lArgs+=pArgs[i];
    }
    
    lClient.Run(pArgs[1],pArgs[2],lArgs,lStandalone);
}
