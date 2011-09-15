#include "cplayer.h"
#include "cclient.h"

#include <iostream>

int main(int pArgC,char **pArgs)
{
    if(pArgC<3)
    {
        std::cerr << "usage: " << pArgs[0] << " host port [gamekey]" << std::endl;
        return -1;
    }

    chk::CPlayer lPlayer;
    chk::CClient lClient(lPlayer);
    
    lClient.Run(pArgs[1],pArgs[2],pArgC>3?pArgs[3]:"");
}