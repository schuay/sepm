#ifndef INTERSERVERFACTORY_H
#define INTERSERVERFACTORY_H

#include "SecureDistributedChat.h"

namespace sdcs
{

class InterServerFactory
{
public:
    static sdc::InterServerIPrx create(QString url);
}

}

#endif
