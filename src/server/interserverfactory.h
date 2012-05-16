#include "SecureDistributedChat.h"

class InterServerFactory
{
public:
    static sdc::InterServerIPrx create(QString url);
}
