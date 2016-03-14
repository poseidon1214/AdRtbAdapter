/***************************************
*	Description: Rtb Planning Data, get from redis redis server.
*	Author: wilson
*	Date: 2015/07/17
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include "AdRtbEngine.h"
#include "AdGlobal.h"
#include "AdTcpServer.h"
#include "AdClassifier.h"
#include "AdSession.h"
#include "AdClient.h"
#include "AdControlTask.h"


/**********************system main loop*********/

void AdBiddingPreSend(AdSession * pSession)
{
	char  sBuf[AD_KBYTES4];
	
	sprintf(sBuf, "%s  0x%x", AD_IBIDDING_ENG_REMOTE, 1);

	pSession->AdSessionWrite(sBuf, strlen(sBuf));
}

class AdUpdateTask:public AdTask
{
public:
	AdUpdateTask(AdRtbEngine* pEngine, string & sFile)
	{
		m_pEngine = pEngine;
		m_sConfigFile = sFile;
		m_sDescription = "AD Bidding Syterm, Update Thread";
	};
	virtual ~AdUpdateTask(){};
	virtual void TaskLoop()
	{
		int ret;
		int iError = 0;
		while(m_bTaskRun)
		{
			Time::wait(1);
			ret = m_pEngine->AdRtbEngineUpdate();
			if(ret == AD_FAILURE)
			{
				iError ++;
				if(iError % 10 == 9)
					AD_ERROR("Update Task Error  %d\n", iError);
			}
			else if(ret == AD_RTB_ENGINE_REINIT)
			{
				ret = m_pEngine->AdRtbEngineInit(m_sConfigFile);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdRtbEngine init error\n");
					break;
				}
				ret =  m_pEngine->AdRtbEngineUpdate();
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdRtbEngine Update error\n");
					break;
				}
			}
			m_uiLoop ++;
		}

		m_bTaskRun = false;
		return;
	};
private:
	AdRtbEngine * m_pEngine;
	string m_sConfigFile;
};

/**********************system main loop      **********************/

static void  AdSystemSignalHandle(int sig)
{
	static UINT  g_uiSignalCount = 0;

	g_uiSignalCount ++;

	if(g_uiSignalCount > 10000)
		g_uiSignalCount = 0;
	AD_ERROR("Recive HUP Signal %d\n", g_uiSignalCount);
}


static int AdSystemSetSignal()
{
	struct sigaction action;

	action.sa_handler = AdSystemSignalHandle;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	if(sigaction(SIGHUP, &action, NULL) < 0)
	{
		AD_ERROR("Set Signal Error\n");
		return AD_FAILURE;
	}
	return AD_SUCCESS;
}


int main(int argc, char ** argv)
{
	int iThread = 1;
	int iIndex;
	int ret;
	string  sPort  = "20251";
	string sIp = "127.0.0.1";
	string sConfFile  = (char*)"../conf/GtRtbEng.conf";
	string  sDetectPort = "8125";

	struct option stOptions[] = 
	{
		{"conf", 1, 0, 'c'},
		{"thread", 1, 0, 't'},
		{"dport", 1, 0,'d'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0},
	};
	
	while (true)
	{
		ret = getopt_long(argc, argv, "c:t:d:h", stOptions, &iIndex);
		if (AD_FAILURE != ret) 
		{
			switch (ret)
			{
				case 'c':
					sConfFile = optarg;
					break;
				case 'd':
					sDetectPort= optarg;
					break;
				case 't':
					iThread = atoi(optarg);
					break;
				case 'h':
					printf("Usage: %s [options]\n", argv[0]);
					printf("Valid options are:\n");
					printf("[0] -c or --conf: 	  set configure\n");	
					printf("[0] -d or --port: 	  set detect server port\n");
					printf("[0] -t or --thread:	  set thread number\n");
					printf("[0] -v or --version:	  show version.\n");
					printf("[0] -h or --help: 	  show help.\n");
					printf("Adirect Technology Co, Ltd\n");
					printf("AdRtbAdapter Build on %s %s.\n", __DATE__, __TIME__);
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

	AdSystemSetSignal();
    
	AdRtbEngine cAdEngine;
	cout<<"***********Engine Init**************\n"<<endl;
	ret = cAdEngine.AdRtbEngineInit(sConfFile);
	if(ret == AD_FAILURE)
	{
		AD_ERROR("AdRtbEngine init error\n");
		return ret;
	}
	
	ret = cAdEngine.AdRtbEngineUpdate();
	if(ret == AD_FAILURE)
	{
		AD_ERROR("AdRtbEngine Update error\n");
		return ret;
	}

	list<string> & ltServer = cAdEngine.AdRtbEngineGetAdapter();

	//init net module
	AdClient ctClient(AdBiddingPreSend);
	cout<<"***********Server Init**************\n"<<endl;
	ret = ctClient.AdClientInit(ltServer, iThread, 1);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdClient init error\n");
		return ret;
	}

	//create net (server,  work,  write) task
	ret = AdControlNetCreateTask(static_cast<AdNetBase *>(&ctClient),
		static_cast<AdNetTaskInterface *>(&cAdEngine),  iThread, 1);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdServer Create error\n");
		return ret;
	}

	//init Control output
	AdControl &ctControl = AdControl::Instance();
	ret = ctControl.OutputInit(atoi(sDetectPort.c_str()));
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Control Output init error\n");
		return ret;
	}

	AdUpdateTask * pUpdate = AdControlCreateTask<AdUpdateTask, AdRtbEngine *, string >(AD_TASK_OTHER + 1, &cAdEngine, sConfFile);
	if (pUpdate == NULL) 
	{
		AD_ERROR("Task create error\n");
		exit(AD_FAILURE);
	}

	ctControl.TaskDetect(300);
	exit(AD_SUCCESS);
}
