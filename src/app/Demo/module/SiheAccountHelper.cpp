#include "SiheAccountHelper.h"

UnorderedStringMap
SiheAccountHelper::DoRequestSiheAccountStatus(const std::string &userId)
{
    CSiheAccountStatusRequest requestInstance;
    auto params = requestInstance.makeRequestParms(userId);

    return requestInstance.request(params);
}
