/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年6月4日
 * This file use for 旺票的后台接口，这里用于获取电子发票的信息，一键申报
 *
 */

#include "WPHttpRequest.h"
#include "GlobalSetting.h"

std::string IWPHttpRequest::GetUrl()
{
    return GlobalSetting::GetWpHttpInterfaceUrl();
}

UnorderedStringMap IWPHttpRequest::request(const UnorderedStringMap &params)
{
    auto responseString = __super::request(params);
    return ParseResponse(responseString);
}

UnorderedStringMap IWPHttpRequest::ParseResponse(const std::string &response)
{
    auto Root = ParseResponseHelper::doCommonParse(response);

    UnorderedStringMap respMap;
    respMap.emplace("success", Json_GetString(Root, "success"));
    respMap.emplace("msg", Json_GetString(Root, "msg"));

    if (!Json_HasField(Root, "data")) {
        return respMap;
    }
    auto dataMap = DoParseResponse(Root["data"]);

    dataMap.insert(respMap.begin(), respMap.end());
    return dataMap;
}

std::string GetInvoicesRequest::GetTag()
{
    return "01";
}

UnorderedStringMap GetInvoicesRequest::MakeRequestParams(const std::string &taxNo,
                                                         std::string &startDate,
                                                         std::string &endDate)
{
    UnorderedStringMap requestmap;
    requestmap.emplace("taxNo", taxNo);
    requestmap.emplace("startDate", startDate);
    requestmap.emplace("endDate", endDate);
    requestmap.emplace("tag", GetTag());
    return requestmap;
}

UnorderedStringMap GetInvoicesRequest::DoParseResponse(const Json::Value &root)
{
    Json::Value invoiceRoot = root["invoies"];
    UnorderedStringMap spbm_calcData;

    // 3% 需要计算劳务 和资产
    // 5% 只计算资产

    double labor_rate_3_je = 0.00;
    double asserts_rate_3_je = 0.00;
    double asserts_rate_5_je = 0.00;
    for (auto i = 0; i < invoiceRoot.size(); i++) {
        auto spbm = Json_GetString(invoiceRoot[i], "spbm");
        auto typeflag = spbm.substr(0, 2);
        auto je = Json_GetString(invoiceRoot[i], "je");
        auto sl = Json_GetString(invoiceRoot[i], "sl");

        auto fje = std::stod(je);
        // 10 20 开头的商品编码为劳务
        // 30 40 50 抬头的为 资产

        //听说你用浮点数直接算金额？？ 苦笑
        if (typeflag == "10" || typeflag == "20") {
            if (sl.find("0.03") != std::string::npos)
                labor_rate_3_je += fje;
        }
        else if (typeflag == "30" || typeflag == "40" || typeflag == "50") {
            if (sl.find("0.03") != std::string::npos)
                asserts_rate_3_je += fje;
            else if (sl.find("0.05") != std::string::npos)
                asserts_rate_5_je += fje;
        }

        // auto kprq = Json_GetString(invoiceRoot[i], "kprq");
    }

    spbm_calcData["labor_rate_3_je"] = std::to_string(labor_rate_3_je);
    spbm_calcData["asserts_rate_3_je"] = std::to_string(asserts_rate_3_je);
    spbm_calcData["asserts_rate_5_je"] = std::to_string(asserts_rate_5_je);

    return spbm_calcData;
}
