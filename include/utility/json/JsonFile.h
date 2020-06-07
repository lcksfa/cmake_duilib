#pragma once
#include <string>
#include <map>
using namespace std;
#define LEN_JSONFILE_TAG		16
class CJsonFile
{
public:
	CJsonFile();
	~CJsonFile();
	void Load(wstring strPath);
	void Save(wstring strPath);
	bool GetTagValue(string strTag,string& strValue);
	bool SetTagValue(string strTag,string& value);
	void DelTagValue(string strTag);
	void DelTagNotInclude(map<string, int>& mapTags);

public:
	static string ToStandardTagString(string strTag);
private:
	map<string, string> m_mapDatas;
};

