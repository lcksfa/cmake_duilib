#ifndef __INCLUDE_RESTREQUESTPARMS_H__
#define __INCLUDE_RESTREQUESTPARMS_H__
#pragma once
#include <string>

/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*-----------------E账通后台接口参数数据 begin-----------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------*/
#if defined KEY_PARAMETER_DEVELOP
constexpr auto pKey =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEAmlBP6PmlMenRXzJ/"
    "GABeZjGXhpi37tWv80A+"
    "36DGZLz0Er1f73pvbZ52yj5AE4Ir2gtYudCeP1A3TCInFdOsbDS79940Rrru9zO3QMLuy4g6bGUj8/"
    "TFvrqOYoLKfXA2ucAumOrBpSNLTFJNEtjXIJmoGeWblUjosIPd6SUin4T7cVV7ayrjvZhWRR7UTbhaZ9VKlV"
    "sKxFMIunzMSYGSFuraK94oBQ/"
    "YHFEixEu+yNKTkj4GZAw11ZdGwwLAdpk47ZY33A5JCDYBhjIIa8Se2d6LmbSjXMUA+"
    "ELlS9ONffNhXuw1LelV12WMGowohzAgE/hcQ+BBCxNULDuIalpEGQIDAQABAoIBAQCS5sO74PRvOMzVmGe/"
    "PJKhLLK4MmhfWFPA+Tchgpd7TRn5QQi0HNU5ALLLQ0mSiMggMc8MTVpooX+S3axhrfmlpOoQRBt1/"
    "f0BndzlLO1RGuWrTPlWA33AXS6w/xuvIIUofZiegFVKVPgYtvyiZYUehsitppRfIGJLr60whuaX/"
    "ed619ANK8q6QLI3D++7NHcIsAD0oVszAb3XIBtDuq0v6INbuoLg9Lk3udja0jdOl/"
    "a9BgQtkbSUC7VHYio0Gv6YHbd9QTly2z5/0lraYhV+s/FCZUMPbfhoeZ/"
    "ORdfP0LcRg0PuMqVZ7jX1vH+"
    "FbMxQRXmDVxdIkyqqsMTXdKsRAoGBAPlto4s7kEKwpLlnWU72BhEqKCzVaqOYnTrLKOJx8hA7vhQzoTnK/"
    "6gaRMSS7s+aODkuDaqM72HTG/jatAQRoibtrm5zb3rIU17jE6uWbaYYt6UeOBpJODxodn/gcnnaS+w6iC2Y/"
    "8sbw1K9ZOBa9uHmFuxaXlbXGEqGI28V2BkdAoGBAJ5hI1ZM6QwlbhooNWwm4xPKdscgc4MPLiVyPDUIBlWuj"
    "pYdvAC23LxWrahXMOGBrlv+"
    "7MTBPQvsD2QA1twtZtUxS6AeDnthFQfJfCUEdUIxIP3i2alrdFiBAHhHUR1VJ/TkmYOfRloYJOO9pPwuIE/"
    "4hbeeKyxDZAX+TPvpqCItAoGADBZsHos8an70/"
    "KcthM4XbS2F1OZizJrQw9CkopSzDmZWx9PYtFxKF3NucbhNKnJm1x6MOv+"
    "peiwo5t5yauAP46MRGPkvJdwfbjHC+kXH+oyoGbBlN6DkwMivNXNDGPSjSUds4gjQHJ+"
    "zz4auJNFDtZ7Y7YtTlxHFc5cckH2Wg2ECgYEAlwBeRKXHfAEj3GSBqr/"
    "YuMHMRUY3W7lZcfJcXONtxrtKzu1ClhvCkP7DS0D2JcsShjujWmQy4+"
    "8JLXtvXzUaWiFJzTta619AQdBkQJbjhVx57BYhGFqO7uLvo727JJBQLDjX9Joi5Tsn0Mq7RoRj4ls8gjiXSP"
    "hnoR84HuZDm3ECgYB6RxhqFAfDXp2dDravC0NK1Cb8TCYJYJ/"
    "FV2hXXImhCF3DYVqXc+qM+qJRaGKj3j7NPVb75Za4eFUtYtYrukdl+URuS5JntBHCyfgmMpo20+"
    "HscAXx7zSZg06s/VS1VStg/Nk6STyFAsyX43e7YRKPhYL9egkJAzlNQ+Tmno9O9Q==\n"
    "-----END RSA PRIVATE KEY-----";

#elif defined(KEY_PARAMETER_TEST)
#elif defined(KEY_PARAMETER_ONLINE)
#endif

constexpr auto Restful_req_method_registry = "bzt.sign.pc01.01";
constexpr auto Rest_req_verificationCode = "bzt.sign.pc01.02";
constexpr auto Restful_req_method_login = "bzt.sign.pc01.03";
constexpr auto Restful_req_method_find_pwd = "bzt.sign.pc01.04";
constexpr auto Restful_req_method_upload_lice_img = "bzt.sign.pc01.05";
constexpr auto Restful_req_get_user_basic_info = "bzt.sign.pc01.06";
constexpr auto Restful_req_get_user_order_info = "bzt.sign.pc01.08";

constexpr auto Restful_req_audit_reason = "bzt.sign.pc01.30";
constexpr auto Restful_req_check_user_status = "bzt.sign.pc01.33";

constexpr auto Restful_req_sihe_account_status = "bzt.sign.pc01.40";
constexpr auto Restful_req_modify_user_area_info = "bzt.sign.pc01.41";

// constexpr auto Restful_req_method_app_update = "bzt.sign.pc01.01";

/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*-----------------E账通后台接口参数数据 end-----------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*-----------------对接旺票后台接口参数数据 begin-----------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*-----------------对接旺票后台接口参数数据 end-----------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------*/

#endif // __INCLUDE_RESTREQUESTPARMS_H__