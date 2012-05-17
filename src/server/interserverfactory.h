#include "SecureDistributedChat.h"

namespace sdcs
{

class InterServerFactory
{
public:
    static sdc::InterServerIPrx create(QString url);
}

}
