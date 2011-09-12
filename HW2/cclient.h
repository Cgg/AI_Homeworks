#ifndef _CHECKERS_CCLIENT_H_
#define _CHECKERS_CCLIENT_H_

#include "cplayer.h"
#include "csocket.h"
#include <vector>
#include <stdint.h>
#include <stdexcept>

namespace chk {

class CPlayer;

///encapsulates client funcionality (except playing)
class CClient
{
public:
    ///constructs the client (requires a player)
    CClient(CPlayer &pPlayer);
    ///destructor
    ~CClient();

private:
    void ReadInit(CTime &pTime,bool &pFirst);
    bool ReadMove(CTime &pTime,CMove &pMove,bool pBlock);
    void WriteMove(const CMove &pMove);

public:
    ///runs the client
    
    ///\param pHost the host to connect to
    ///\param pPort the port to connect to
    ///\param pKey the key to send to the server. If it is empty, connect
    ///in standalone mode
    void Run(const std::string &pHost,const std::string &pPort,
             const std::string &pKey);
    
private:
    CPlayer &mPlayer;
    CBoard mBoard;
    CSocket mSocket;
    
    bool mStandalone;
};

/*namespace chk*/ }

#endif