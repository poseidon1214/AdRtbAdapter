#include <map>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "AdTcp.h"
#include "AdGlobal.h"
#include "AdUtil.h"

using namespace std;

int AdRtbOutput(char * buff, int len)
{
	return AD_FAILURE;
}

#define AD_RTB_ADP_PRINT_INPUT                                          0x00000001
#define AD_RTB_ADP_PRINT_REQUEST                                        0x00000002
#define AD_RTB_ENGINE_PRINT_REQUEST                                     0x00000004
#define AD_RTB_ENGINE_PRINT_FILTER                                      0x00000008
#define AD_RTB_ENGINE_PRINT_RESULT                                      0x00000010
#define AD_RTB_ENGINE_PRINT_RANK                                        0x00000020
#define AD_RTB_ENGINE_PRINT_RESPONSE                                    0x00000040
#define AD_RTB_ADP_PRINT_RESPONSE                                       0x00000080
#define AD_RTB_ADP_PRINT_OUTPUT                                         0x00000100


struct AdDebug
{
    UINT   uiThread;
    UINT   uiType;
    UINT   uiData;
};

#define AD_DEBUG_PRINT_REALTIME                     1
#define AD_DEBUG_PRINT_PROCESS                      2
#define AD_DEBUG_PRINT_INFO                         3
#define AD_DEBUG_PRINT_DETAIL                      4

class SumInfo
{
public:
	SumInfo()
	{
		//adpter
		m_llAll = 0;
		m_llError = 0;
		m_llTimeout = 0;
		m_llAdpRequest = 0;
		m_llAdpResponse = 0;
		m_llAllRate = 0;
		m_llErrorRate = 0;
		m_llTimeoutRate = 0;
		m_llAdpRequestRate = 0;
		m_llAdpResponseRate = 0;
		//biding
		m_llBidRequest = 0;
		m_llBidResponse = 0;
		m_llBidRequestRate = 0;
		m_llBidResponseRate = 0;

		m_vInvalidChar.push_back(' ');
		m_vInvalidChar.push_back('\t');
		m_vInvalidChar.push_back('\n');
		m_vInvalidChar.push_back('\r');
		m_vInvalidChar.push_back(')');

		m_sStr.reserve(AD_KBYTES8);
	};

	void Sum(char * pBuf, int len)
	{
		m_sStr += pBuf;
	};
	
	void Sum()
	{
		char *pCur  = const_cast<char *>(m_sStr.c_str());
		char *pLast = pCur;
		int type = 0;
		string line;
		string::size_type pos;
		vector<string> vStr;

		pCur= strcasestr(pLast, "\n");
		while(pCur != NULL)
		{
			line.assign(pLast, pCur - pLast + 1);
			StrDeleteInvalid(line);
			pos = line.find("Work");
			if(pos != string::npos)
			{
				type = 1;
				pLast = pCur + 1;
				pCur= strcasestr(pLast, "\n");
				continue;
			}
			pos = line.find("Bidding");
			if(pos != string::npos)
			{
				type = 2;
				pLast = pCur + 1;
				pCur= strcasestr(pLast, "\n");
				continue;
			}
			if(type > 0)
			{
				pos = line.find("ALL");
				if(pos != string::npos)
				{
					AdString::StrSplit(line,vStr,":");
					if(vStr.size() == 2)
					{
						line = vStr[1];
						vStr.clear();
						AdString::StrSplit(line,vStr,"(");
						if(vStr.size() == 2)
						{
							m_llAll += atoi(vStr[0].c_str());
							m_llAllRate += atoi(vStr[1].c_str());
						}
					}
					vStr.clear();
				}
				pos = line.find("Error");
				if(pos != string::npos)
				{
					AdString::StrSplit(line,vStr,":");
					if(vStr.size() == 2)
					{
						line = vStr[1];
						vStr.clear();
						AdString::StrSplit(line,vStr,"(");
						if(vStr.size() == 2)
						{
							m_llError+= atoi(vStr[0].c_str());
							m_llErrorRate += atoi(vStr[1].c_str());
						}
					}
					vStr.clear();
				}
				pos = line.find("Timeout");
				if(pos != string::npos)
				{
					AdString::StrSplit(line,vStr,":");
					if(vStr.size() == 2)
					{
						line = vStr[1];
						vStr.clear();
						AdString::StrSplit(line,vStr,"(");
						if(vStr.size() == 2)
						{
							m_llTimeout+= atoi(vStr[0].c_str());
							m_llTimeoutRate+= atoi(vStr[1].c_str());
						}
					}
					vStr.clear();
				}
				pos = line.find("Request");
				if(pos != string::npos)
				{
					AdString::StrSplit(line,vStr,":");
					if(vStr.size() == 2)
					{
						line = vStr[1];
						vStr.clear();
						AdString::StrSplit(line,vStr,"(");
						if(vStr.size() == 2)
						{
							if(type == 1)
							{
								m_llAdpRequest += atoi(vStr[0].c_str());
								m_llAdpRequestRate += atoi(vStr[1].c_str());
							}
							else
							{
								m_llBidRequest += atoi(vStr[0].c_str());
								m_llBidRequestRate += atoi(vStr[1].c_str());
							}
						}
					}
					vStr.clear();
				}
				pos = line.find("Response");
				if(pos != string::npos)
				{
					AdString::StrSplit(line,vStr,":");
					if(vStr.size() == 2)
					{
						line = vStr[1];
						vStr.clear();
						AdString::StrSplit(line,vStr,"(");
						if(vStr.size() == 2)
						{
							if(type == 1)
							{
								m_llAdpResponse+= atoi(vStr[0].c_str());
								m_llAdpResponseRate += atoi(vStr[1].c_str());
							}
							else
							{
								m_llBidResponse += atoi(vStr[0].c_str());
								m_llBidResponseRate += atoi(vStr[1].c_str());
							}
						}
					}
					vStr.clear();
				}
			}
			pLast = pCur + 1;
			pCur= strcasestr(pLast, "\n");
		}
	};
	void print()
	{
		Sum();
		AD_PRINT("ALL  : AD Work task\n");
		AD_PRINT("	ALL:           %lld(%lld/sec)\n", m_llAll, m_llAllRate);
		AD_PRINT("	Error:         %lld(%lld/sec)\n", m_llError, m_llErrorRate);
		AD_PRINT("	Timeout:       %lld(%lld/sec)\n", m_llTimeout, m_llTimeoutRate);
		AD_PRINT("	Request:       %lld(%lld/sec)\n", m_llAdpRequest, m_llAdpRequestRate);
		AD_PRINT("	Response:      %lld(%lld/sec)\n", m_llAdpResponse, m_llAdpResponseRate);
		AD_PRINT("ALL  : AD Bidding task\n");
		AD_PRINT("	Request:       %lld(%lld/sec)\n", m_llBidRequest, m_llBidRequestRate);
		AD_PRINT("	Response:      %lld(%lld/sec)\n", m_llBidResponse, m_llBidResponseRate);
	};
private:
	int StrDeleteInvalid(string& str)
	{
		string::iterator it = str.begin();
		while(it != str.end())
		{
			if(find(m_vInvalidChar.begin(), m_vInvalidChar.end(), *it) != m_vInvalidChar.end())
				str.erase(it);
			else
				it ++;
		}
		return AD_SUCCESS;
	};
	
	long long  m_llAll;
	long long m_llError;
	long long m_llTimeout;
	long long m_llAdpRequest;
	long long m_llAdpResponse;
	long long m_llAllRate;
	long long m_llErrorRate;
	long long m_llTimeoutRate;
	long long m_llAdpRequestRate;
	long long m_llAdpResponseRate;

	long long m_llBidRequest;
	long long m_llBidResponse;
	long long m_llBidRequestRate;
	long long m_llBidResponseRate;

	vector<char> m_vInvalidChar;

	string m_sStr;
};



int GetInfo(struct AdDebug * pData, string & sIp,  int iPort)
{
	int ret;
	AdTcp ctTcp;
	int iWait = 0;
	char buff[AD_KBYTES4];
	SumInfo ctSum;
	
	ret = ctTcp.Connect(sIp.c_str(), iPort);
	if(ret <= 0)
	{
		AD_ERROR("Server Init error\n");
		return ret;
	}
	ret = ctTcp.Write((char *)pData, sizeof(struct AdDebug));
	if(ret <= 0)
	{
        	AD_ERROR("write Error\n");
        	return ret;
	}
	while(true)
	{
		ret = ctTcp.Read(buff, AD_KBYTES4, 100000);
		if(ret == AD_FAILURE)
		{
           		 break;
		}
	        if(ret == AD_SUCCESS)
	        {
	        	iWait ++;
	        	if(pData->uiType == AD_DEBUG_PRINT_PROCESS)
	        	{
	        		if(iWait >= 1000)
	                		break;
	        	}
			else
			{
				if(iWait >= 5)
	                		break;
			}
	               continue;
	        }
	        iWait = 0;
	        buff[ret] = '\0';
		if(pData->uiType != AD_DEBUG_PRINT_INFO)
			printf("%s", buff);
		else
		{
			ctSum.Sum(buff, ret);
		}
	}

	if(pData->uiType == AD_DEBUG_PRINT_INFO)
	{
		AD_PRINT("------%s:%d----------------\n", sIp.c_str(), iPort);
		ctSum.print();
	}
	return 0;
}

int main(int argc, char ** argv)
{
	struct AdDebug ctDebug;
	ctDebug.uiType = AD_DEBUG_PRINT_INFO;
	string sIp;
	int ret;
	int iIndex;
	int iPort = 8123;
	struct option stOptions[] = 
	{
		{"ip", 1, 0, 'i'},
		{"type", 1, 0, 't'},
		{"data", 1, 0, 'd'},
		{"thread", 1, 0, 'r'},
		{"port", 1, 0, 'p'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0},
	};
	
	while (true)
	{
		ret = getopt_long(argc, argv, "i:t:d:r:p:h", stOptions, &iIndex);
		if (AD_FAILURE != ret) 
		{
			switch (ret)
			{
				case 'i':
					sIp = optarg;
					break;
				case 't':
					ctDebug.uiType = atoi(optarg);
					break;
				case 'd':
					ctDebug.uiData = atoi(optarg);
					break;
				case 'r':
					ctDebug.uiThread = atoi(optarg);
					break;
				case 'p':
					iPort= atoi(optarg);
					break;
				case 'h':
					AD_PRINT("Usage: %s [options]\n", argv[0]);
					AD_PRINT("Valid options are:\n");
					AD_PRINT(" -i or --ip:            Server Ip\n");
					AD_PRINT(" -p or --port:           Server Port\n");
					AD_PRINT(" -r or --thread:        thread number\n");
					AD_PRINT(" -t or --type:          Info Type\n");
					AD_PRINT("            1:          RealTime Info\n");
					AD_PRINT("            2:          Process Info\n");
					AD_PRINT("            3:          System Info\n");
					AD_PRINT("            4:          System Detail Info\n");
					AD_PRINT(" -d or --data:          Output module\n");
					AD_PRINT("        t   1:          d:  exec id\n");
					AD_PRINT("        t   2:          0x01    Input Info\n");
					AD_PRINT("                        0x02    Request Info\n");
					AD_PRINT("                        0x04    Filter Info\n");
					AD_PRINT("                        0x08    Filter Result\n");
					AD_PRINT("                        0x10    Deal Rank info\n");
					AD_PRINT("                        0x20    Response info\n");
					AD_PRINT("                        0x40    Output info\n");
					AD_PRINT(" -h or --help:          show help.\n\n");
					AD_PRINT("Addirects Technology Co, Ltd\n");
					AD_PRINT("AdRtbEngine Test on %s %s.\n", __DATE__, __TIME__);
					return AD_SUCCESS;
				default:
					return AD_SUCCESS;
			}
		}
		else 
		{
			break;
		}
	}
	if(!sIp.empty())
		GetInfo(&ctDebug,sIp, iPort);
	else
	{
		AD_PRINT("Usage: %s [options]\n", argv[0]);
		AD_PRINT("Valid options are:\n");
		AD_PRINT(" -i or --ip:            Server Ip\n");
		AD_PRINT(" -r or --thread:        thread number, needed by process info\n");
		AD_PRINT(" -t or --type:          Info Type\n");
		AD_PRINT("            1:          RealTime Info\n");
		AD_PRINT("            2:          Process Info\n");
		AD_PRINT("            3:          System Info\n");
		AD_PRINT("            4:          System Detail Info\n");
		AD_PRINT(" -d or --data:          Output module\n");
		AD_PRINT("        t   1:          d:  exec id\n");
		AD_PRINT("        t   2:          0x01    Input Info\n");
		AD_PRINT("                        0x02    Request Info\n");
		AD_PRINT("                        0x04    Filter Info\n");
		AD_PRINT("                        0x08    Filter Result\n");
		AD_PRINT("                        0x10    Deal Rank info\n");
		AD_PRINT("                        0x20    Response info\n");
		AD_PRINT("                        0x40    Output info\n");
		AD_PRINT(" -h or --help:          show help.\n\n");
		AD_PRINT("Addirects Technology Co, Ltd\n");
		AD_PRINT("AdRtbEngine Test on %s %s.\n", __DATE__, __TIME__);
	}
	return AD_SUCCESS;
}
