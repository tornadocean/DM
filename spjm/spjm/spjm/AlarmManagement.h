//////////////////////////////////////////////////////////////////////////////
// The following code implements the state-machine:
//
// E30 :  GEM Alarm Management
//
// Process Job State Model
//
//	    +-------------+      *1      +-------------+
//      |             | -----------> |             |
//      |   ALARM n   |              |   ALARM n   |
//      |    CLEAR    |      *2      |    SET      |
//      |             | <----------- |             |
//      +-------------+              +-------------+
//
//  *1: Alarm n is detected on the equipment.
//  *2: Alarm n is no longer detected on the equipment.
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

namespace alm
{
	// define event
	class EvSet		: public sc::event< EvSet > {};   //
	class EvClear   : public sc::event< EvClear > {};      //
}

namespace alm
{// define state
	class AlarmClear;
	class AlarmSet;
}

//////////////////////////////////////////////////////////////////////////
// State Machine
using namespace alm;
class AlarmManagementState
	: public sc::state_machine<AlarmManagementState, AlarmClear>
{
public:
	AlarmManagementState(void)
	{

	}

	~AlarmManagementState(void)
	{

	}

	AlarmManagementState(int nALCD, int nALID, std::string strALTX)
	{
		m_nALID =	nALID;
		m_nALCD =	nALCD;
		m_strALTX = strALTX;
	}

	const void SetState(std::string strState)
	{
		m_strState = strState;
	}
	const std::string GetState(void)
	{
		return m_strState;
	}

	virtual void OnSet(void) = 0;
	virtual void OnClear(void) = 0;

public:
	std::string		m_strState;

	int				m_nALCD;
	int				m_nALID;
	std::string		m_strALTX;
};

//////////////////////////////////////////////////////////////////////////
// States Defined
namespace alm
{

	//
	//--------------------------------------------------------------------
	//
	class AlarmSet 
	: public sc::state< AlarmSet, AlarmManagementState>
	{
	public:
	typedef sc::transition< EvClear, AlarmClear > reactions;

	AlarmSet(my_context ctx) : my_base(ctx)
	{
		std::cout<< "Enter AlarmSet\n";
		outermost_context().SetState("AlarmSet");
	}
	~AlarmSet()
	{
		std::cout<< "Exit AlarmSet\n";
	}
	};
	
	//
	//--------------------------------------------------------------------
	//
	class AlarmClear 
		: public sc::state< AlarmClear, AlarmManagementState >
	{
	public:
		typedef sc::transition< EvSet, AlarmSet > reactions; 

		AlarmClear(my_context ctx) : my_base(ctx)
		{
			std::cout<< "Enter AlarmClear\n";
			outermost_context().SetState( "AlarmClear" );
		}
		~AlarmClear()
		{
			std::cout<< "Exit AlarmClear\n";
		}
	};

}
