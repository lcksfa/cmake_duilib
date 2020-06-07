#ifndef __INCLUDE_DES_H__
#define __INCLUDE_DES_H__
#include <string>

using namespace std;

namespace ckbase
{

enum
{
    PAD_ISO_1 =
        0, // ISO_1填充：数据长度不足8比特的倍数，以0x00补足，如果为8比特的倍数，补8个0x00
    PAD_ISO_2, // ISO_2填充：数据长度不足8比特的倍数，以0x80,0x00..补足，如果为8比特的倍数，补0x80,0x00..0x00
    PAD_PKCS_7 // PKCS7填充：数据长度除8余数为n,以(8-n)补足为8的倍数
};

enum
{
    ECB = 0, // ECB模式
    CBC      // CBC模式
};
enum
{
    ENCRYPT = 0, //加密
    DECRYPT      //解密
};
class yxDES
{
public:
    yxDES(){};
    ~yxDES(){};

protected:
    typedef bool (*PSubKey)[16][48];
    //计算并填充子密钥到SubKey数据中
    static void SetSubKey(PSubKey pSubKey, const char Key[8]);
    // DES单元运算
    static void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type);

public:
    /*******************************************************************/
    /*
      函 数 名 称:	RunPad
      功 能 描 述：	根据协议对加密前的数据进行填充
      参 数 说 明：	bType	:类型：PAD类型
                                    In		:数据串指针
                                    Out		:填充输出串指针
                                    datalen	:数据的长度
                                    padlen	:(in,out)输出buffer的长度，填充后的长度

      返回值 说明：	bool	:是否填充成功
      作       者:	杨虎
    /*******************************************************************/
    static bool RunPad(int nType, const char *In, unsigned datalen, char *Out,
                       unsigned &padlen);

    /*******************************************************************/
    /*
      函 数 名 称:	RunDes
      功 能 描 述：	执行DES算法对文本加解密
      参 数 说 明：	bType	:类型：加密ENCRYPT，解密DECRYPT
                                    bMode	:模式：ECB,CBC
                                    In		:待加密串指针
                                    Out		:待输出串指针
                                    datalen
    :待加密串的长度，同时Out的缓冲区大小应大于或者等于datalen Key
    :密钥(可为8位,16位,24位)支持3密钥 keylen	:密钥长度，多出24位部分将被自动裁减

      返回值 说明：	bool	:是否加密成功
      作       者:	杨虎
    /*******************************************************************/
    static bool RunDes(bool bType, bool bMode, char *In, char *Out, unsigned datalen,
                       const char *Key, const unsigned char keylen);
};
} // namespace ckbase

#endif // __INCLUDE_DES_H__