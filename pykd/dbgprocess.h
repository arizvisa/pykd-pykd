#pragma once

#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <dbgeng.h>

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getThreadList();

bool
setImplicitThread( 
    ULONG64  newThreadAddr );
    
ULONG64
getImplicitThread();        

boost::python::object
getCurrentStack();

boost::python::object
getLocals();


class dbgStackFrameClass : public DEBUG_STACK_FRAME {

public:

    dbgStackFrameClass()
    {
        memset( static_cast<DEBUG_STACK_FRAME*>( this ), 0, sizeof(DEBUG_STACK_FRAME) );
    }       
    
    dbgStackFrameClass( const DEBUG_STACK_FRAME &stackFrame );
};

boost::python::object
getProcessorMode();

void
setProcessorMode(
    const std::string &mode );
    
ULONG64    
getCurrentProcess();

VOID
setCurrentProcess(
    ULONG64  processAddr );

/////////////////////////////////////////////////////////////////////////////////