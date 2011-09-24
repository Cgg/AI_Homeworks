#ifndef _DUCKS_CCLIENT_H_
#define _DUCKS_CCLIENT_H_

#include "cplayer.h"
#include "csocket.h"
#include <vector>
#include <stdint.h>
#include <stdexcept>

namespace ducks {

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
    int ReadDucks();
    bool ReadState(CTime &pTime);
    void WriteAction(const CAction &pPos);
    void WriteGuess();
    void ReadEOG();

public:
    ///runs the client

    ///\param pHost the host to connect to
    ///\param pPort the port to connect to
    ///\param pKey the key to send to the server. If it is empty, connect
    ///in standalone mode
    void Run(const std::string &pHost,const std::string &pPort,
             const std::string &pArgs,bool pStandalone);

private:
    CPlayer &mPlayer;
    CState mState;
    CSocket mSocket;

    bool mStandalone;
};

/*namespace ducks*/ }

#endif
