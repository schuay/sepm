#include "participant.h"

namespace sdcs
{

Participant::Participant(const sdc::User &user, const QString &chatID)
    : self(user), chatID(chatID)
{
}

Participant::~Participant()
{
}

LocalParticipant::LocalParticipant(const sdc::User &user, const QString &chatID)
    : Participant(user, chatID)
{
}

}
