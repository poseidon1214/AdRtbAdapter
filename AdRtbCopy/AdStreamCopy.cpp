#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include "AdGlobal.h"
#include "AdServer.h"
#include "AdClient.h"
#include "AdSession.h"
#include "AdNetTask.h"

using namespace std;

int AdRtbOutput(char * buff, int len)
{
	return AD_FAILURE;
}

/**********************system main loop      **********************/

class AdStreamCopy:public AdNetTaskInterface
{
public:
	AdStreamCopy()
	{
		string sName = "AdStream Copy";
		SetName(sName);
	};
	virtual ~AdStreamCopy(){};

	int ClientInit()
	{
	    int ret;
	    AdTcp *pTcp;
	    
	    m_mInfo[20007] = "192.168.167.188";
	    m_mInfo[20008] = "192.168.167.188";
	    map<int, string>::iterator mIter;

	    for(mIter = m_mInfo.begin(); mIter != m_mInfo.end(); mIter ++)
	    {
	            pTcp = new AdTcp;
	            if(pTcp == NULL)
	            {
	                AD_ERROR("Tcp alloc error\n");
	                return AD_FAILURE;
	            }
	            ret = pTcp->Connect(mIter->second.c_str(), mIter->first);
	            if(ret <= 0)
	            {
	                AD_ERROR("Server Init error\n");
	                return ret;
	            }
	            m_ltClients.push_back(pTcp);
	    }

	    return AD_SUCCESS;
	}

	virtual  int Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask)
	{
		int len;
		if(pCache == NULL)
			return AD_FAILURE;
		cout<<"Get Data: "<< pCache->Size()<<endl;
		list<AdTcp * >::iterator iter;
		for(iter = m_ltClients.begin(); iter != m_ltClients.end(); iter ++)
		{
				len = (*iter)->Write(pCache->Get(), pCache->Size());
				if(len <= 0)
				{
						AD_ERROR("write Error\n");
				}
		}
	
		iter = m_ltClients.begin();
		len = (*iter)->Read(pCache->Get(), pCache->Max(), 20000);
		if(len > 0)
		{
			cout<<"Send Data: "<< len <<endl;
			pSession->AdSessionWrite(pCache->Get(), len);
		}

		return AD_SUCCESS;
	}
private:
	map<int, string>  m_mInfo;
	list<AdTcp * >  m_ltClients;
};


int main(int argc, char ** argv)
{
	int iThread = 1;
	int iIndex;
	int ret;
	string  sPort  = "20251";
	
	struct option stOptions[] = 
	{
		{"port", 1, 0, 'p'},
		{"help", 0, 0, 'h'},
	};
	
	while (true)
	{
		ret = getopt_long(argc, argv, "p:h", stOptions, &iIndex);
		if (AD_FAILURE != ret) 
		{
			switch (ret)
			{
				case 'p':
					sPort = optarg;
					break;
				case 'h':
					AD_PRINT("Usage: %s [options]\n", argv[0]);
					AD_PRINT("Valid options are:\n");
					AD_PRINT("[0] -p or --port: 	  set listen port\n");
					AD_PRINT("[0] -h or --help: 	  show help.\n");
					AD_PRINT("Adirect Technology Co, Ltd\n");
					AD_PRINT("AdRtbAdapter Build on %s %s.\n", __DATE__, __TIME__);
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

    	AdStreamCopy  ctStream;
	ctStream.ClientInit();
	
	AdServer cServer;
	cout<<"***********Server Init**************\n"<<endl;
	ret = cServer.AdServerInit(atoi(sPort.c_str()), iThread, 1);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdServer init error\n");
		return ret;
	}

	ret = AdNetTaskCreate<AdNetBase *, AdNetTaskInterface *>(
		static_cast<AdNetBase *>(&cServer), 
		static_cast<AdNetTaskInterface *>(&ctStream), iThread, 1);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdServer Create error\n");
		return ret;
	}


	while(true)
	{
		Time::wait(10);
	}
	
	return AD_SUCCESS;
}
