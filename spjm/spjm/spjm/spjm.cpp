// spjm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ProcessJobState.h"
#include "AlarmManagement.h"
#include <iostream>
#include <string>

char GetKey()
{
	char key;
	std::cin >> key;
	return key;
}

std::string GetEvent()
{
	std::string key;
	std::cin >> key;
	return key;
}

class ProcessJob 
	: public ProcessJobState
{
	virtual void OnQueued(void)
	{

	};

	virtual void OnSettingUp(void)
	{

	};
};

class AlarmManagement
	: public AlarmManagementState
{
	virtual void OnSet()
	{

	};

	virtual void OnClear()
	{

	};
};


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "boost::fsm ProcessJob state model example\n\n";
	std::cout << "abort<CR>: Aborts process job\n";
	std::cout << "aborted<CR>: Aborted process job\n";
	std::cout << "start<CR>: Starts process job\n";
	std::cout << "cancel<CR>: Cancels process job\n";
	std::cout << "depart<CR>: Depart process job\n";
	std::cout << "complete<CR>: Comletes process job\n";
	std::cout << "alloc<CR>: Allocates process job\n";
	std::cout << "pause<CR>: Pauses process job\n";
	std::cout << "paused<CR>: Paused process job\n";
	std::cout << "resume<CR>: Resumes process job\n";
	std::cout << "stop<CR>: Stops process job\n";
	std::cout << "stopped<CR>: Stopped process job\n";
	std::cout << "quit<CR>: Quits the program\n\n";
	
	ProcessJob Pjm;
	Pjm.initiate();

	std::string key = GetEvent();

	while ( key != "quit" )
	{
		if( key == "abort")
		{
			Pjm.process_event(EvAbort());
		}
		else if(key == "aborted")
		{
			Pjm.process_event(EvAborted());
		}
		else if(key == "start")
		{
			Pjm.process_event(EvStart());
		}
		else if(key == "cancel")
		{
			Pjm.process_event(EvCancel());
		}
		else if(key == "depart")
		{
			Pjm.process_event(EvDepart());
		}
		else if(key == "complete")
		{
			Pjm.process_event(EvCompleted());
		}
		else if(key == "alloc")
		{
			Pjm.process_event(EvAllocate());
		}
		else if(key == "pause")
		{
			Pjm.process_event(EvPause());
		}
		else if(key == "paused")
		{
			Pjm.process_event(EvPaused());
		}
		else if(key == "resume")
		{
			Pjm.process_event(EvResume());
		}
		else if(key == "stop")
		{
			Pjm.process_event(EvStop());
		}
		else if(key == "stopped")
		{
			Pjm.process_event(EvStopped());
		}
		else if(key == "state")
		{
			std::cout<< "State: "<< Pjm.GetState() << "\n";
		}

		key = GetEvent();
	}

	AlarmManagement ALM;
	ALM.initiate();

	key = "";

	while ( key != "quit" )
	{
		if( key == "set")
		{
			ALM.process_event(EvSet());
		}
		else if(key == "clear")
		{
			ALM.process_event(EvClear());
		}
		else if(key == "state")
		{
			std::cout<< "State: "<< ALM.GetState() << "\n";
		}

		key = GetEvent();
	}

	return 0;
}