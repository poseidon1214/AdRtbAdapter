#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include "AdGlobal.h"
#include "AdControlTask.h"
#include "AdAdx.h"
#include "AdUtil.h"
#include "AdMult.h"


using namespace std;

class AdUpdateTask:public AdTask
{
public:
	AdUpdateTask(AdAdx * pAdx)
	{
		m_pAdx = pAdx;
		m_sDescription = "AD Adx Syterm, Update Thread";
	};
	virtual ~AdUpdateTask(){};
	virtual void TaskLoop()
	{
		int ret;
		UINT iError = 0;
		while(m_bTaskRun)
		{
			ret = m_pAdx->Update();
			if(ret != AD_FAILURE)
				iError ++;
			Time::wait(5);
			m_uiLoop ++;
		}
		return;
	};
private:
	AdAdx * m_pAdx;
};


/**********************system main loop      **********************/

static void  AdSystemSignalHandle(int sig)
{
	if(sig == SIGHUP)
		AD_ERROR("Recive HUP Signal, Ignore\n");
	else if(sig == SIGPIPE)
		AD_ERROR("Recive PIPE Signal, Ignore\n");
	else 
		AD_ERROR("Recive Unknow Signal  %d, Ignore\n", sig);
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

	if (sigaction(SIGPIPE, &action, 0) == -1) 
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
	string sConf = "../conf/AdAdx.conf";
	string     sPort  = "20251";
	string     sDetectPort = "8123";
	
	struct option stOptions[] = 
	{
		{"conf", 1, 0, 'c'},
		{"port", 1, 0, 'p'},
		{"thread", 1, 0, 't'},
		{"version", 0, 0, 'v'},
		{"detect", 1, 0,'d'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0},
	};
	
	while (true)
	{
		ret = getopt_long(argc, argv, "c:p:t:d:vh", stOptions, &iIndex);
		if (AD_FAILURE != ret) 
		{
			switch (ret)
			{
				case 'c':
					sConf = optarg;
					break;
				case 'p':
					sPort = optarg;
					break;
				case 'd':
					sDetectPort = optarg;
					break;
				case 't':
					iThread = atoi(optarg);
					break;
				case 'v':
				case 'h':
					printf("Usage: %s [options]\n", argv[0]);
					printf("Valid options are:\n");
					printf("[0] -c or --conf: 	  set configure\n");
					printf("[0] -p or --port: 	  set listen port\n");
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

	
	//init net module
	AdMult cMult(NULL);
	cout<<"***********Server Init**************\n"<<endl;
	ret = cMult.Init(iThread, 1, atoi(sPort.c_str()));
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdMultServer init error\n");
		return ret;
	}
	
	//init adx
	AdAdx  cAdx(&cMult);
	ret = cAdx.Init(sConf);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Adx init error\n");
		return ret;
	}

	//create net (server,  work,  write) task
	ret = AdControlNetCreateTask( static_cast<AdNetBase *>(&cMult), 
		 static_cast<AdNetTaskInterface*>(&cAdx),  iThread, 1);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdServer Create error\n");
		return ret;
	}

	AdUpdateTask * pTask= AdControlCreateTask<AdUpdateTask, AdAdx * >(AD_TASK_OTHER + 1, &cAdx);
	if (pTask == NULL) 
	{
		AD_ERROR("Task create error\n");
		exit(AD_FAILURE);
	}

	//init Control output
	AdControl &ctControl = AdControl::Instance();

	ret = ctControl.OutputInit(atoi(sDetectPort.c_str()));
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Control Output init error\n");
		return ret;
	}

	ctControl.TaskDetect(300);

	exit(AD_SUCCESS);
}

