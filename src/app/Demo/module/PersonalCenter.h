#ifndef __INCLUDE_PERSONALCENTER_H__
#define __INCLUDE_PERSONALCENTER_H__

#pragma once
#include <string>
#include "RestRequest.h"
#include <vector>

struct OrderListInternal {
    std::string phone;
    std::string orderNo;
    std::string amount;
    std::string createTime;
    std::string startDate;
    std::string endDate;
};

class PersonalCenter
{
public:
    PersonalCenter() = default;
    ~PersonalCenter() = default;

    UnorderedStringMap DoRequestUserBasicInfo(const std::string &userId);
    UnorderedStringMap DoRequestUserOrders(const std::string &taxno);
    UnorderedStringMap DoRequestModifyUserAreaInfo(const std::string &userid,
                                                   const std::string &province,
                                                   const std::string &city);

    UnorderedStringMap DoRequestAuditReasons(const std::string &useId);
    UnorderedStringMap
    DoRequestModifyBussinessLincenseImage(const std::string &uploadFile,
                                          const std::string &useId);

    std::vector<OrderListInternal> ParseOrderList(const UnorderedStringMap &retmap);

    std::string DownloadBussinessImage(const std::string &url);
};
#endif // __INCLUDE_PERSONALCENTER_H__