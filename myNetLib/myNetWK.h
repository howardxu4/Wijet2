#if (!defined(_MY_NETWORK_DEFINED))
#define _MY_NETWORK_DEFINED

#include "SocketSystem.h"
#include "SocketBase.h"
#include "SocketUDP.h"
#include "SocketTCP.h"
#include "MultiCastUDP.h"
#include "SocketICMP.h"

#include "Sender.h"

#ifdef _WIN32
#include "NICAccess.h"
#endif

#define WIJET2_PORT     30200



#endif // _MY_NETWORK_DEFINED
