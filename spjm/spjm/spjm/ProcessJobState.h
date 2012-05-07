//////////////////////////////////////////////////////////////////////////////
// The following code implements the state-machine:
//
// E40 : STANDARD FOT PROCESS MANAGEMENT
//
// Process Job State Model
//
// +------------------------------------------------------------------------+
// |                                                        PROCESSJOB      |
// |      O     +---------------------------------------------------------+ |
// |   *1 |     |                                                 ACTIVE  | |
// |      |     |  +----------------------------+      +------------+     | |
// |      V     |  |                EXECUTING   | *11  |  STOPPING  |----------+
// | +--------+ |  | *2  +----------------+     |----->|            |---+ | |  |
// | | QUEUED/|--------->|   SETTING UP   |     |      +------------+   | | |  |
// | | POOLED | |  |  +--|                |     |            ^          | | |  |
// | +--------+ |  |  |  +----------------+     |            |*12       | | |  |
// |      |     |  |  |*4        |*3            |  +------------------+ | | |  |
// |      |     |  |  |          V              |  |            PAUSE | | | |  |
// |      |     |  |  |  +----------------+     |*8|   +-----------+  | | | |  |
// |      |     |  |  |  |  WAITING FOR   |     |----->|  PAUSING  |  | | | |  |
// |      |     |  |  |  |     START      |     |  |   |           |  | | | |  |
// |      |     |  |  |  +----------------+     |  |   +-----------+  | | | |  |
// |      |     |  |  |          |*5       (H)<----|         |*9      | | | |  |
// |      |     |  |  |          V           *10|  |         V        | | | |  |
// |      |     |  |  |  +----------------+     |  |   +-----------+  | | | |  |
// |      |     |  |  +->|   PROCESSING   |     |  |   |  PAUSED   |  | | | |  |
// |      |     |  |     |                |     |  |   |           |  | | | |  |
// |      |     |  |     +----------------+     |  |   +-----------+  | | | |  |
// |      |     |  |             |*6            |  +------------------+ | | |  |
// |      |     |  |             |              |            |*15    *14| | |  |
// |      |     |  |             |              |            V          | | |  |*17
// |      |     |  |             |              | *13  +------------+   | | |  |
// |      |     |  |             |              |----->|  ABORTING  |<--+ | |  |
// |      |     |  |             |              |      |            |     | |  |
// |      |     |  |             |              |      +------------+     | |  |
// |      |     |  +-------------|--------------+            |*16         | |  |
// |      |     +----------------|---------------------------|------------+ |  |
// |      |     +----------------|---------------------------|------------+ |  |
// |      |     |                V          POSTACTIVE       V            | |  |
// |      |     |        +----------------+            +------------+     | |  |
// |      |     |        |    PROCESS     |            |  ABORTED   |     | |  |
// |      |     |        |    COMPLETE    |            |            |     | |  |
// |      |     |        +----------------+            +------------+     | |  |
// |      |     |                                      +------------+     | |  |
// |      |     |                                      |  STOPPED   |<---------+
// |      |     |                                      |            |     | |
// |      |     |                                      +------------+     | |
// |      |     +---------------------------------------------------------+ |
// +------|----------------------|------------------------------------------+                                                      
//        |                      |*7                                             
//        |                      |                                               
//        |     *18              V                                               
//        +--------------------> O                                          
//                                                                              
//
//  *1: The processing resource accepts a ProcessJob create request.
//  *2: The processing resource has been allocated to the ProcessJob.
//  *3: Job material is present and the processing resource is ready to start
//      the ProcessJob and PRProcessStart attribute is not set.
//  *4: Material is present and ready for processing. PRProcessStart attribute is set.
//  *5: Job start directive.
//  *6: Material processing completed.
//  *7: Job material departed the processing resource and resource post-conditioning
//      completed, or superceded by another process job on the same material.
//  *8: The processing resource initiated a process pause action. (it received a
//      "Pause" command or initiated an internal pause)
//  *9: The processing resource paused the job.
// *10: The processing resource resumed the job.
// *11: The processing resource initiated a process stop action. (it received a
//      "Stop" command or initiated an internal stop)
// *12: The processing resource initiated a process stop action. (it received a
//      "Stop" command or initiated an internal stop)
// *13: The processing resource initiated a process abort action. (it received a
//      "Abort" command or initiated an internal abort)
// *14: The processing resource initiated a process abort action. (it received a
//      "Abort" command or initiated an internal abort)
// *15: The processing resource initiated a process abort action. (it received a
//      "Abort" command or initiated an internal abort)
// *16: The processing resource abort procedure is complete and for some processing
//      equipment the related substrates are moved out as part of the error recovery.
// *17: The processing resource stop procedure is complete.
// *18: "Cancel", "Stop" or "Abort" command received.
//
//
#pragma once

#include <boost/statechart/event.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/shallow_history.hpp>
#include <boost/mpl/list.hpp>

//#include <boost/signals.hpp>
#include <boost/bind.hpp>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

namespace pjm
{
// define event
class EvAllocate  : public sc::event< EvAllocate > {};   //
class EvStart     : public sc::event< EvStart > {};      //
class EvCompleted : public sc::event< EvCompleted > {};  //
class EvDepart    : public sc::event< EvDepart > {};     //
class EvPause     : public sc::event< EvPause > {};      //
class EvPaused    : public sc::event< EvPaused > {};     //
class EvResume    : public sc::event< EvResume > {};     //
class EvStop      : public sc::event< EvStop > {};       //
class EvStopped   : public sc::event< EvStopped > {};    //
class EvAbort     : public sc::event< EvAbort > {};      //
class EvAborted   : public sc::event< EvAborted > {};    //
class EvCancel    : public sc::event< EvCancel > {};     //
}

namespace pjm
{// define state
class QueuedAndPooled;
class SettingUp;
class Executing;
class Pausing;
class Stopping;
class Aborting;
class WaitingForStart;
class Processing;
class ProcessCompleted;
class Paused;
class Departed;
class Active;
class Stopped;
class Aborted;
class PostActive;
}

//////////////////////////////////////////////////////////////////////////
// State Machine
using namespace pjm;
class ProcessJobState
	: public sc::state_machine<ProcessJobState, QueuedAndPooled>
{
public:
	ProcessJobState(void);
	~ProcessJobState(void);
	
	const void SetState(std::string strState)
	{
		m_strState = strState;
	}
	const std::string GetState(void)
	{
		return m_strState;
	}

	virtual void OnQueued(void) = 0;
	virtual void OnSettingUp(void) = 0;

public:
	std::string m_strState;
};

//////////////////////////////////////////////////////////////////////////
// States Defined
namespace pjm
{

//
//-----------------------------------------------------------------------------
//
class Departed 
	: public sc::state< Departed, ProcessJobState >
{
public:
	typedef sc::transition< EvAllocate, QueuedAndPooled > reactions; //only for test

	Departed(my_context ctx) : my_base(ctx)
	{
		std::cout<< "Enter Departed\n";
		outermost_context().SetState( "Departed" );
	}
	~Departed()
	{
		std::cout<< "Exit Departed\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class QueuedAndPooled 
	: public sc::simple_state< QueuedAndPooled, ProcessJobState>
{
public:
	typedef mpl::list
		< 
		sc::transition< EvAllocate, SettingUp >,
		sc::transition< EvAbort, Departed >, 
		sc::transition< EvCancel, Departed >,
		sc::transition< EvStop, Departed > 
		> reactions;

	QueuedAndPooled()
	{
		std::cout<< "Enter QueuedAndPooled\n";
	}
	~QueuedAndPooled()
	{
		std::cout<< "Exit QueuedAndPooled\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Active 
	: public sc::simple_state< Active, ProcessJobState, Executing >
{
public:
	Active()
	{
		std::cout<< "Enter Active\n";
	}
	~Active()
	{
		std::cout<< "Exit Active\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class PostActive
	: public sc::simple_state< PostActive, ProcessJobState, ProcessCompleted>
{
public:
	typedef sc::transition< EvDepart, Departed > reactions;

	PostActive()
	{
		std::cout<< "Enter PostActive\n";
	}
	~PostActive()
	{
		std::cout<< "Exit PostActive\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Executing 
	: public sc::simple_state< Executing, Active, SettingUp, sc::has_shallow_history >
{
public :
	typedef mpl::list
		<
		sc::transition< EvPause, Pausing >, 
		sc::transition< EvStop, Stopping >,
		sc::transition< EvAbort, Aborting > 
		> reactions;

	Executing()
	{
		std::cout<< "Enter Executing\n";
	}
	~Executing()
	{
		std::cout<< "Exit Executing\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class SettingUp 
	: public sc::state< SettingUp, Executing >
{
public:
	typedef mpl::list
		<
		sc::transition< EvAllocate, WaitingForStart >, 
		sc::transition< EvStart, Processing >
		> reactions;

	SettingUp(my_context ctx) : my_base(ctx)
	{
		std::cout<< "Enter SettingUp\n";
		outermost_context().SetState("SettingUp");
	}
	~SettingUp()
	{
		std::cout<< "Exit SettingUp\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class WaitingForStart 
	: public sc::state< WaitingForStart,Executing>
{
public:
	typedef sc::transition< EvStart, Processing > reactions;

	WaitingForStart(my_context ctx) : my_base(ctx)
	{
		std::cout<< "Enter WaitingForStart\n";
		outermost_context().SetState("WaitingForStart");
	}
	~WaitingForStart()
	{
		std::cout<< "Exit WaitingForStart\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Processing 
	: public sc::state< Processing, Executing>
{
public:
	typedef sc::transition< EvCompleted, ProcessCompleted > reactions;

	 Processing(my_context ctx) :  my_base(ctx)
	{
		outermost_context().SetState( "Processing");
		std::cout<< "Enter Processing\n";
	}
	~Processing()
	{
		std::cout<< "Exit Processing\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class ProcessCompleted 
	: public sc::state< ProcessCompleted, PostActive >
{
public:

	ProcessCompleted(my_context ctx) : my_base(ctx)
	{
		std::cout<< "Enter ProcessCompleted\n";
		outermost_context().SetState("ProcessCompleted");
	}
	~ProcessCompleted()
	{
		std::cout<< "Exit ProcessCompleted\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Stopping 
	: public sc::simple_state< Stopping, Active >
{
public:
	typedef mpl::list
		<
		sc::transition< EvStopped, Stopped > ,
		sc::transition< EvAbort, Aborting>
		> reactions;

	Stopping()
	{
		std::cout<< "Enter Stopping\n";
	}
	~Stopping()
	{
		std::cout<< "Exit Stopping\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Stopped 
	: public sc::simple_state< Stopped, PostActive >
{
public:

	Stopped()
	{
		std::cout<< "Enter Stopped\n";
	}
	~Stopped()
	{
		std::cout<< "Exit Stopped\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Aborting 
	: public sc::simple_state< Aborting, Active >
{
public:
	typedef sc::transition< EvAborted, Aborted > reactions;

	Aborting()
	{
		std::cout<< "Enter Aborting\n";
	};
	~Aborting()
	{
		std::cout<< "Exit Aborting\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Aborted 
	: public sc::simple_state< Aborted, PostActive >
{
public:

	Aborted()
	{
		std::cout<< "Enter Aborted\n";
	};
	~Aborted()
	{
		std::cout<< "Exit Aborted\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Pause
	: public sc::simple_state< Pause, Active,Pausing >
{
public:
	typedef mpl::list
		< 
		sc::transition< EvResume,sc::shallow_history< SettingUp > >,
		sc::transition< EvStop, Stopping >,
		sc::transition< EvAbort, Aborting > 
		> reactions;

	Pause()
	{
		std::cout<< "Enter Pause\n";
	}
	~Pause()
	{
		std::cout<< "Exit Pause\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Pausing 
	: public sc::simple_state< Pausing, Pause >
{
public:
	typedef sc::transition< EvPaused, Paused > reactions;

	Pausing()
	{
		std::cout<< "Enter Pausing\n";
	}
	~Pausing()
	{
		std::cout<< "Exit Pausing\n";
	}
};

//
//-----------------------------------------------------------------------------
//
class Paused
	: public sc::simple_state< Paused, Pause >
{
public:

	Paused()
	{
		std::cout<< "Enter Paused\n";
	}
	~Paused()
	{
		std::cout<< "Exit Paused\n";
	}
};

}
