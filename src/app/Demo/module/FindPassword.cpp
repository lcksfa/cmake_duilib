#include "FindPassword.h"

UnorderedStringMap FindPassword::RequestFetchVerCode(const std::string &phone)
{
    CVerificationCodeRequest requestInstance;
    auto parms = requestInstance.makeRequestParms(phone, "002");
    return requestInstance.request(parms);
}

UnorderedStringMap FindPassword::RequestFindPwd(const std::string &verificationcode,
                                                const std::string &phone,
                                                const std::string &password)
{
    CRestFindPassword requestInstance;
    auto params = requestInstance.makeRequestParms(verificationcode, phone, password);
    return requestInstance.request(params);
}
