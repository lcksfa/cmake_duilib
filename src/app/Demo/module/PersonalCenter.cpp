#include "PersonalCenter.h"
#include "RestRequest.h"
#include "json/json.h"
#include "FileDownloader.h"
#include "app_env/app_ins.h"
#include "win32/string_code_convert.h"

UnorderedStringMap PersonalCenter::DoRequestUserBasicInfo(const std::string &userId)
{
    CGetUserBasicInfoRequest requestInstance;
    auto params = requestInstance.makeRequestParms(userId);

    return requestInstance.request(params);
}

UnorderedStringMap PersonalCenter::DoRequestUserOrders(const std::string &taxno)
{
    CGetUserOrderInfoRequest requestInstance;
    auto params = requestInstance.makeRequestParms(taxno);

    return requestInstance.request(params);
}

std::vector<OrderListInternal>
PersonalCenter::ParseOrderList(const UnorderedStringMap &retmap)
{
    auto infojson = retmap.find("infos")->second;
    // auto count = retmap.find("count")->second; //useless
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(infojson, root))
        return std::vector<OrderListInternal>();

    std::vector<OrderListInternal> orderList;
    for (unsigned int i = 0; i < root.size(); i++) {
        OrderListInternal listitem;
        listitem.phone = Json_GetString(root[i], "phone");
        listitem.orderNo = Json_GetString(root[i], "orderNo");
        listitem.amount = Json_GetString(root[i], "amount");
        listitem.createTime = Json_GetString(root[i], "createTime");
        listitem.startDate = Json_GetString(root[i], "startDate");
        listitem.endDate = Json_GetString(root[i], "endDate");
        orderList.emplace_back(listitem);
    }
    return orderList;
}

std::string PersonalCenter::DownloadBussinessImage(const std::string &url)
{
    auto runningPath = AppEnvironment::GetRunningPath();
    auto ansi_running_path = ckbase::win32::UnicodeToMBCS(runningPath);

    //查找后台返回的图片的后缀
    auto pos = url.rfind(".");
    if (pos == std::string::npos)
        return std::string();
    auto imageFix = url.substr(pos);
    auto imagedstPath = ansi_running_path + "lincense." + imageFix;
    FileDownloader loader(url, imagedstPath);
    if (loader.Do())
        return imagedstPath;
    else
        return std::string();
}

UnorderedStringMap PersonalCenter::DoRequestModifyUserAreaInfo(
    const std::string &userid, const std::string &province, const std::string &city)
{
    CModifyUserAreaInfoRequest requestInstance;
    auto params = requestInstance.makeRequestParms(userid, province, city);
    return requestInstance.request(params);
}

UnorderedStringMap PersonalCenter::DoRequestAuditReasons(const std::string &useId)
{
    CAuditReasonRequest requestInstance;
    auto params = requestInstance.makeRequestParms(useId);
    return requestInstance.request(params);
}

UnorderedStringMap
PersonalCenter::DoRequestModifyBussinessLincenseImage(const std::string &uploadFile,
                                                      const std::string &userId)
{
    CUploadBusinessLicenseRequest requestInstance;
    auto params = requestInstance.makeRequestParms(uploadFile, userId);
    return requestInstance.request(params);
}
