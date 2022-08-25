#include "stdafx.h"

#include <boost/bind.hpp>

#include "pykdver.h"

#include "variant.h"
#include "dbgexcept.h"
#include "windbgext.h"

#include "pydbgeng.h"
#include "pydbgio.h"
#include "pydisasm.h"
#include "pyevents.h"
#include "pyeventhandler.h"
#include "pymemaccess.h"
#include "pymodule.h"
#include "pysymengine.h"
#include "pytypedvar.h"
#include "pytypeinfo.h"
#include "pycpucontext.h"
#include "pyprocess.h"
#include "pytagged.h"

using namespace pykd;

///////////////////////////////////////////////////////////////////////////////

static const std::string pykdVersion = PYKD_VERSION_BUILD_STR
#ifdef _DEBUG
    " <DBG>"
#endif  // _DEBUG
;

///////////////////////////////////////////////////////////////////////////////


BOOST_PYTHON_FUNCTION_OVERLOADS( startProcess_,  pykd::startProcess, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( attachProcess_, pykd::attachProcess, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS( detachProcess_,  pykd::detachProcess, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( terminateProcess_,  pykd::terminateProcess, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS(closeDump_, pykd::closeDump, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS( attachKernel_,  pykd::attachKernel, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( evaluate_, pykd::evaluate, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( debugCommand_, pykd::debugCommand, 1, 3 );

BOOST_PYTHON_FUNCTION_OVERLOADS( dprint_, pykd::dprint, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln_, pykd::dprintln, 1, 2 );

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( Module_findSymbol, Module::getSymbolNameByVa, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( loadChars_, pykd::loadChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWChars_, pykd::loadWChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes_, pykd::loadBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords_, pykd::loadWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords_, pykd::loadDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords_, pykd::loadQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes_, pykd::loadSignBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords_, pykd::loadSignWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords_, pykd::loadSignDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords_, pykd::loadSignQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadFloats_, pykd::loadFloats, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDoubles_, pykd::loadDoubles, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemory_, pykd::compareMemory, 3, 4 );

BOOST_PYTHON_FUNCTION_OVERLOADS( writeBytes_, pykd::writeBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeWords_, pykd::writeWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeDWords_, pykd::writeDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeQWords_, pykd::writeQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeSignBytes_, pykd::writeSignBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeSignWords_, pykd::writeSignWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeSignDWords_, pykd::writeSignDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeSignQWords_, pykd::writeSignQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeFloats_, pykd::writeFloats, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( writeDoubles_, pykd::writeDoubles, 2, 3 );

BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceFile_, pykd::getSourceFile, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceFileFromSrcSrv_, pykd::getSourceFileFromSrcSrv, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceLine_, pykd::getSourceLine, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( findSymbol_, pykd::findSymbol, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getStack_, pykd::getStack, 0, 1);

BOOST_PYTHON_FUNCTION_OVERLOADS( getProcessOffset_, pykd::getProcessOffset, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS( getProcessSystemId_, pykd::getProcessSystemId, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS( getProcessExecutableName_, pykd::getProcessExecutableName, 0, 1);

BOOST_PYTHON_FUNCTION_OVERLOADS( getThreadOffset_, pykd::getThreadOffset, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS( getThreadSystemId_, pykd::getThreadSystemId, 0, 1);
BOOST_PYTHON_FUNCTION_OVERLOADS( getThreadIdBySystemId_, pykd::getThreadIdBySystemId, 0, 1);

BOOST_PYTHON_FUNCTION_OVERLOADS( createStruct_, pykd::defineStruct, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( createUnion_, pykd::defineUnion, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( defineFunction_, pykd::defineFunction, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( setSoftwareBreakpoint_, Breakpoint::setSoftwareBreakpoint, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( setHardwareBreakpoint_, Breakpoint::setHardwareBreakpoint, 3, 4 );

BOOST_PYTHON_FUNCTION_OVERLOADS( TargetHeap_getEntries, TargetHeapAdapter::getEntries, 1, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS( Module_enumSymbols, ModuleAdapter::enumSymbols, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( Module_findSymbol, ModuleAdapter::findSymbol, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( Module_enumTypes, ModuleAdapter::enumTypes, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( TypeInfo_ptrTo, TypeInfoAdapter::ptrTo, 1, 2 ); 

BOOST_PYTHON_FUNCTION_OVERLOADS( getTypeFromSource_, pykd::getTypeFromSource, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getTypeInfoProviderFromPdb_, pykd::getTypeInfoProviderFromPdb, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getTypeInfoProviderFromSource_, pykd::getTypeInfoProviderFromSource, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS( getSymbolProviderFromSource_, pykd::getSymbolProviderFromSource, 1, 2);
BOOST_PYTHON_FUNCTION_OVERLOADS(evalExpr_, pykd::evalExpr, 1, 3);

BOOST_PYTHON_FUNCTION_OVERLOADS( addSyntheticModule_, pykd::addSyntheticModule, 3, 4 );

namespace pykd {

void initialize()
{
    AutoRestorePyState  pystate;
    kdlib::initialize();
}

void remote_initialize( const std::wstring& remoteOptions )
{ 
    AutoRestorePyState  pystate;
    kdlib::remote_initialize(remoteOptions);
}

void uninitialize()
{
    AutoRestorePyState  pystate;
    kdlib::uninitialize();
}

}

void pykd_init()
{
    python::scope().attr("__version__") = pykdVersion;
    python::scope().attr("version") = pykdVersion;

    python::def( "initialize", pykd::initialize,
        "Initialize local debug engine, only for console mode" );
    python::def( "remoteConnect",pykd::remote_initialize,
        "Initialize debug engine for remoting, only for console mode" );
    python::def( "deinitialize", pykd::uninitialize,
        "Deintialize debug engine, only for console mode" );

   // DbgEng services 
    python::def( "setSymSrvDir", pykd::setSymSrvDir,
        "Set directory of SYMSRV.dll library.\nUsually this is a directory of WinDbg");
    python::def( "getExtensionSearchPath", pykd::getExtensionSearchPath,
        "Return the extension DLL search path" );
    python::def( "loadExt", pykd::loadExtension,
        "Load a WinDBG extension. Return handle of the loaded extension" );
    python::def( "removeExt", (void(*)(kdlib::EXTENSION_ID))&pykd::removeExtension,
        "Unload a WinDBG extension. Parameter: handle returned by loadExt" );
    python::def( "removeExt", (void (*)(const std::wstring& ))&pykd::removeExtension,
        "Unload a WinDBG extension. Parameter: extension path" );
    python::def( "callExt", pykd::callExtension,
        "Call a WinDBG extension's routine. Parameters: handle returned by loadExt; string command line" );

   // Manage debug target 

    python::def( "startProcess", pykd::startProcess, startProcess_( boost::python::args( "commandline", "debugOptions"), 
        "Start process for debugging" ) ); 
    python::def("attachProcess", pykd::attachProcess, attachProcess_(boost::python::args("pid", "debugOptions"),
        "Attach debugger to a existing process"));
    python::def( "detachProcess", pykd::detachProcess, detachProcess_( boost::python::args( "id" ),
        "Stop process debugging") ); 
    python::def( "detachAllProcesses", pykd::detachAllProcesses, 
        "Detach from all process and resume all their threads" );
    python::def( "killProcess", pykd::terminateProcess, terminateProcess_( boost::python::args( "id" ),
        "Stop debugging and terminate current process" ) );
    python::def( "killAllProcesses", pykd::terminateAllProcesses,
        "Detach from all process then terminate them");
    python::def( "loadDump", pykd::loadDump,
        "Load crash dump");
    python::def("closeDump", pykd::closeDump, closeDump_(boost::python::args("id"),
        "Close crash dump"));
    python::def( "isLocalKernelDebuggerEnabled", pykd::isLocalKernelDebuggerEnabled,
        "Check whether kernel debugging is enabled for the local kernel");
    python::def( "attachKernel", pykd::attachKernel, attachKernel_( boost::python::args( "connectOptions" ),
        "Connect the debugger engine to a kernel target.\n"
        "If connectOptions is not specified - attach to the local kernel") );
    python::def( "isDumpAnalyzing", pykd::isDumpAnalyzing,
        "Check if it is a dump analyzing ( not living debuggee )" );
    python::def( "isKernelDebugging", pykd::isKernelDebugging,
        "Check if kernel dubugging is running" );
    python::def( "isWindbgExt", pykd::isWindbgExt,
        "Check if script works in windbg context" );
    python::def( "writeDump", pykd::writeDump,
        "Create memory dump file" );
    python::def( "getLocalProcesses", pykd::getLocalProcesses,
        "Return list of running processes on the host system" );
    python::def("getHostProcessPath", pykd::getHostProcessPath,
        "Return image path of the process running python interpreter with a pykd");
    python::def( "getDebugOptions", pykd::getDebugOptions,
        "Return debug options" );
    python::def( "changeDebugOptions", pykd::changeDebugOptions,
        "Change debug options" );
    python::def("getOutputMask", pykd::getOutputMask,
        "Get output mask");
    python::def("setOutputMask", pykd::setOutputMask,
        "Set output mask");
    python::def("getDumpType", pykd::getDumpType,
        "Return type of the dump");
    python::def("getDumpFormat", pykd::getDumpFormat,
        "Return format the dump");

    python::def( "breakin", pykd::targetBreak,
        "Break into debugger" );
    python::def( "expr", pykd::evaluate, evaluate_( python::args( "expression", "cplusplus" ),
        "Evaluate windbg expression" ) );
    python::def( "dbgCommand", &pykd::debugCommand, debugCommand_( python::args(
        "command", "suppressOutput", "outputMask"), 
        "Run a debugger's command and return it's result as a string. You can set additional outputMask" \
        "if you want to get also error messages" ) );
    python::def( "go", pykd::targetGo,
        "Go debugging"  );
    python::def( "step", pykd::targetStep,
        "The target is executing a single instruction or--if that instruction is a subroutine call--subroutine" );
    python::def( "trace", pykd::targetStepIn,
        "The target is executing a single instruction" );
    python::def("stepout", pykd::targetStepOut,
        "The target is executing while not returned from the current subroutine");
    python::def("sourceStep", pykd::sourceStep,
        "The target is executing a single source line");
    python::def("sourceStepOver", pykd::sourceStepOver,
        "The target is executing a single source line");
    python::def( "getExecutionStatus", pykd::targetExecutionStatus,
        "Return current execution status" );

   // Debug output
    python::def( "dprint", &pykd::dprint, dprint_( python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "dprintln", &pykd::dprintln, dprintln_( python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def("dinput", &pykd::dinput,
        "Provide input for debugger");
    python::def("setStatusMessage", &pykd::setStatusMessage,
        "Set message at a windbg status bar");


    // Python debug output console helper classes
    python::class_<DbgOut>( "dout", "dout", python::no_init )
        .def( "write", &DbgOut::write )
        .def( "writedml", &DbgOut::writedml )
        .def( "flush", &DbgOut::flush )
        .def( "isatty", &DbgOut::isatty )
        .add_property( "encoding", &DbgOut::encoding )
        .add_property( "closed", &DbgOut::closed );

    python::class_<DbgIn>( "din", "din", python::no_init )
        .def( "readline", &DbgIn::readline )
        .add_property( "encoding", &DbgIn::encoding )
        .add_property("closed", &DbgIn::closed );

   // system properties
    python::def( "ptrSize", pykd::ptrSize,
        "Return effective pointer size" );
    python::def( "is64bitSystem", pykd::is64bitSystem,
       "Check if target system has 64 address space" );
    python::def( "pageSize", pykd::getPageSize,
        "Get the page size for the currently executing processor context" );
    python::def( "systemUptime", pykd::getSystemUptime,
        "Return the number of seconds the computer has been running" );
    python::def( "currentTime", pykd::getCurrentTime,
        "Return the number of seconds since the beginning of 1970" );
    python::def("getSystemVersion", pykd::getSystemVersion,
        "Return systemVersion");
    python::def( "bugCheckData", pykd::getBugCheckData,
        "Function reads the kernel bug check code and related parameters\n"
        "And return tuple: (code, arg1, arg2, arg3, arg4)" );

    // Manage target memory access
    python::def( "addr64", pykd::addr64,
        "Extend address to 64 bits formats" );
    python::def( "isValid", pykd::isVaValid,
        "Check if the virtual address is valid" );
    python::def("isVaRegionValid", pykd::isVaRegionValid,
        "Check if the virtual addresses region is valid");
    python::def( "compareMemory", pykd::compareMemory, compareMemory_( python::args( "offset1", "offset2", "length", "phyAddr" ),
        "Compare two memory buffers by virtual or physical addresses" ) );
    python::def( "searchMemory", pykd::searchMemoryLst, 
        "Search in virtual memory" );
    python::def( "searchMemory", pykd::searchMemoryStr, 
        "Search in virtual memory" );
    python::def( "findMemoryRegion", pykd::findMemoryRegion,
        "Return address of beginning valid memory region nearest to offset" );
    python::def( "getVaProtect", pykd::getVaProtect,
        "Return memory protect" );
    python::def( "getVaType", pykd::getVaType,
        "Return memory type");
    python::def( "getVaState", pykd::getVaState,
        "Return memory state");
    python::def("getVaAttributes", pykd::getVaAttributes,
        "Return memory attributes");

    python::def( "ptrByte", pykd::ptrByte,
        "Read an unsigned 1-byte integer from the target memory" );
    python::def( "ptrWord", pykd::ptrWord,
        "Read an unsigned 2-byte integer from the target memory" );
    python::def( "ptrDWord", pykd::ptrDWord,
        "Read an unsigned 4-byte integer from the target memory" );
    python::def( "ptrQWord", pykd::ptrQWord,
        "Read an unsigned 8-byte integer from the target memory" );
    python::def( "ptrMWord", pykd::ptrMWord,
        "Read an unsigned mashine's word wide integer from the target memory" );
    python::def( "ptrSignByte", pykd::ptrSignByte,
        "Read an signed 1-byte integer from the target memory" );
    python::def( "ptrSignWord", pykd::ptrSignWord,
        "Read an signed 2-byte integer from the target memory" );
    python::def( "ptrSignDWord", pykd::ptrSignDWord,
        "Read an signed 4-byte integer from the target memory" );
    python::def( "ptrSignQWord", pykd::ptrSignQWord,
        "Read an signed 8-byte integer from the target memory" );
    python::def( "ptrSignMWord", pykd::ptrSignMWord,
        "Read an signed mashine's word wide integer from the target memory" );
    python::def( "ptrFloat", pykd::ptrSingleFloat,
        "Read a float with single precision from the target memory" );
   python::def( "ptrDouble", pykd::ptrDoubleFloat,
        "Read a float with single precision from the target memory" );

    python::def( "setByte", pykd::setByte,
        "Write an unsigned 1-byte integer to the target memory" );
    python::def( "setWord", pykd::setWord,
        "Write an unsigned 2-byte integer to the target memory" );
    python::def( "setDWord", pykd::setDWord,
        "Write an unsigned 4-byte integer to the target memory" );
    python::def( "setQWord", pykd::setQWord,
        "Write an unsigned 8-byte integer to the target memory" );
    python::def( "setSignByte", pykd::setSignByte,
        "Write an signed 1-byte integer to the target memory" );
    python::def( "setSignWord", pykd::setSignWord,
        "Write an signed 2-byte integer to the target memory" );
    python::def( "setSignDWord", pykd::setSignDWord,
        "Write an signed 4-byte integer to the target memory" );
    python::def( "setSignQWord", pykd::setSignQWord,
        "Write an signed 8-byte integer to the target memory" );
    python::def( "setFloat", pykd::setSingleFloat,
        "Write a float with single precision to the target memory" );
   python::def( "setDouble", pykd::setDoubleFloat,
        "Write a float with single precision to the target memory" );

    python::def( "loadBytes", pykd::loadBytes, loadBytes_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned bytes" ) );
    python::def( "loadWords", pykd::loadWords, loadWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned shorts" ) );
    python::def( "loadDWords", pykd::loadDWords, loadDWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned long ( double word )" ) );
    python::def( "loadQWords", pykd::loadQWords, loadQWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned long long ( quad word )" ) );
    python::def( "loadSignBytes", pykd::loadSignBytes, loadSignBytes_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed bytes" ) );
    python::def( "loadSignWords", pykd::loadSignWords, loadSignWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed words" ) );
    python::def( "loadSignDWords", pykd::loadSignDWords, loadSignDWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed longs" ) );
    python::def( "loadSignQWords", pykd::loadSignQWords, loadSignQWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed long longs" ) );
    python::def( "loadChars", pykd::loadChars, loadChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadWChars", pykd::loadWChars, loadWChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadCStr", pykd::loadCStr,
        "Load string from the target buffer containing 0-terminated ansi-string" );
    python::def( "loadWStr", pykd::loadWStr,
        "Load string from the target buffer containing 0-terminated unicode-string" );
    python::def( "loadUnicodeString", pykd::loadUnicodeStr,
        "Return string representation of windows UNICODE_STRING type" );
    python::def( "loadAnsiString", pykd::loadAnsiStr,
        "Return string representation of windows ANSI_STRING type" );
    python::def( "loadFloats", pykd::loadFloats, loadFloats_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of floats" ) );
    python::def( "loadDoubles", pykd::loadDoubles, loadDoubles_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of doubles" ) );

    python::def( "writeBytes", pykd::writeBytes, writeBytes_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of unsigned bytes to the target's memory" ) );
    python::def( "writeWords", pykd::writeWords, writeWords_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of unsigned shorts to the target's memory" ) );
    python::def( "writeDWords", pykd::writeDWords, writeDWords_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of unsigned long ( double word ) to the target's memory" ) );
    python::def( "writeQWords", pykd::writeQWords, writeQWords_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of unsigned long long ( quad word ) to the target's memory" ) );
    python::def( "writeSignBytes", pykd::writeSignBytes, writeSignBytes_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of signed bytes to the target's memory" ) );
    python::def( "writeSignWords", pykd::writeSignWords, writeSignWords_( python::args( "offset", "count", "phyAddr" ),
        "Writing a list of signed words to the target's memory" ) );
    python::def( "writeSignDWords", pykd::writeSignDWords, writeSignDWords_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of signed longs to the target's memory" ) );
    python::def( "writeSignQWords", pykd::writeSignQWords, writeSignQWords_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of signed long longs to the target's memory" ) );
    python::def( "writeFloats", pykd::writeFloats, writeFloats_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of floats to the target's memory" ) );
    python::def( "writeDoubles", pykd::writeDoubles, writeDoubles_( python::args( "offset", "values", "phyAddr" ),
        "Writing a list of doubles to the target's memory" ) );
    python::def( "writeCStr", pykd::writeCStr,
        "Write string as a 0 terminated ansi string to the buffer");
    python::def( "writeWStr", pykd::writeWStr,
        "Write string as a 0 terminated unicode string to the buffer");


    python::def( "ptrPtr", pykd::ptrPtr,
        "Read an pointer value from the target memory" );
    python::def( "loadPtrList", pykd::loadPtrList,
        "Return list of pointers, each points to next" );
    python::def( "loadPtrs", pykd::loadPtrArray,
        "Read the block of the target's memory and return it as a list of pointers" );

    python::def( "setPtr", pykd::setPtr,
        "Write an pointer value to the target memory" );

    // types and variables
    python::def( "getSourceFile", pykd::getSourceFile, getSourceFile_( python::args( "offset"),
        "Return source file by the specified offset" ) );
    python::def("getSourceFileFromSrcSrv", pykd::getSourceFileFromSrcSrv, getSourceFileFromSrcSrv_(python::args("offset"),
        "Load and return source file from source server by the specified offset") );
    python::def( "getSourceLine", pykd::getSourceLine, getSourceLine_( python::args( "offset"),
        "Return source file name, line and displacement by the specified offset" ) );

    python::def( "getOffset", pykd::getSymbolOffset,
        "Return target virtual address for specified symbol" );
    python::def( "findSymbol", pykd::findSymbol, findSymbol_( python::args( "offset", "showDisplacement"),
        "Find symbol by the target virtual memory offset" ) );
    python::def("findSymbolAndDisp", pykd::findSymbolAndDisp,
        "Return tuple (module_name, symbol_name, displacement) by virtual address" );
    python::def( "sizeof", pykd::getSymbolSize,
        "Return a size of the type or variable" );
#if 0
    python::def("typedVarList", pykd::getTypedVarListByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarList", pykd::getTypedVarListByType,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarArray", pykd::getTypedVarArrayByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
    python::def("typedVarArray", pykd::getTypedVarArrayByType,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
#endif
    python::def("containingRecord", pykd::containingRecordByName,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );
    python::def("containingRecord", pykd::containingRecordByType,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );
    python::def( "createStruct", &pykd::defineStruct, createStruct_( python::args( "name", "align" ),
        "Create custom struct" ) );
    python::def( "createUnion", &pykd::defineUnion, createUnion_( python::args( "name", "align" ),
        "Create custom union" ) );
    python::def( "defineFunction", &pykd::defineFunction, defineFunction_( python::args("returnType", "callconv"),
        "Define custom function prototype" ) );
    python::def( "callFunctionByPtr", python::raw_function(pykd::callFunctionByVar, 1) );
    python::def( "callFunctionByAddr", python::raw_function(pykd::callFunctionByOffset, 2) );
    python::def( "callFunctionRaw", python::raw_function(pykd::callFunctionRaw, 1) );

#if 0
    python::def( "getTypeFromSource", &pykd::getTypeFromSource, getTypeFromSource_( python::args("sourceCode", "typeName", "compileOptions"),
        "Create typeInfo class from C/C++ source code") );
    python::def( "getTypeInfoProviderFromSource", &pykd::getTypeInfoProviderFromSource, getTypeInfoProviderFromSource_( python::args("sourceCode", "compileOptions"),
        "Create typeInfo provider from C/C++ source code") );
    python::def( "getTypeInfoProviderFromPdb", &pykd::getTypeInfoProviderFromPdb, getTypeInfoProviderFromPdb_( python::args("filePath", "baseOffset"),
        "Create typeInfo provider from pdb file") );
    python::def("getSymbolProviderFromSource", &pykd::getSymbolProviderFromSource, getSymbolProviderFromSource_(python::args("sourceCode", "compileOptions"),
        "Create symbol provider for source code"));
    python::def("evalExpr", &pykd::evalExpr, evalExpr_(python::args("expression", "scope", "typeProvider"),
        "Evaluate C++ expression with typed information"));
#endif

    // CPU registers
    python::def( "reg", pykd::getRegisterByName,
        "Return a CPU register value by the register's name" );
    python::def( "reg", pykd::getRegisterByIndex,
        "Return a CPU register value by the register's number");
    python::def( "setReg", pykd::setRegisterByName,
        "Set a CPU register value by its name" );
    python::def( "setReg", pykd::setRegisterByIndex,
        "Set a CPU register value by its index" );
    python::def( "getNumberRegisters", pykd::getNumberRegisters,
        "Return a number of CPU registers");
    python::def( "getRegisterName", pykd::getRegisterName,
        "Return register name by its index");
    python::def("getIP", pykd::getIP,
        "Return instruction pointer");
    python::def("getSP", pykd::getSP,
        "Return stack pointer");
    python::def("getFP", pykd::getFP,
        "Return frame pointer");
    python::def("setIP", pykd::setIP,
        "Change instruction pointer");
    python::def("setSP", pykd::setSP,
        "Change stack pointer");
    python::def("setFP", pykd::setFP,
        "Change frame pointer");
    python::def ( "rdmsr", pykd::loadMSR,
        "Return MSR value" );
    python::def( "wrmsr", pykd::setMSR,
        "Set MSR value" );
    python::def( "getCPUMode", pykd::getProcessorMode, 
        "Return current processor mode: CPUType" );
    python::def( "getCPUType", pykd::getProcessorType,
        "Return type of physical processor: CPUType" );
    python::def( "setCPUMode",pykd::setProcessorMode,
        "Set current processor mode (CPUType)" );
    python::def( "switchCPUMode", pykd::switchProcessorMode,
        "Switch processor mode ( X86 <-> X64 )" );

   // stack and local variables
    python::def( "getStack", pykd::getStack, getStack_(python::args("inlineFrames"),
        "Return a current stack as a list of stackFrame objects" ) );
    python::def( "getFrame", pykd::getCurrentFrame,
        "Return a current stack frame" );
    python::def("getFrameNumber", pykd::getCurrentFrameNumber,
        "Return current frame number");
    python::def( "setFrame", pykd::setCurrentFrame,
        "Change current local scope" );
    python::def( "setFrame", pykd::setCurrentFrameByIndex,
        "Change current local scope" );
    python::def( "resetFrame", pykd::resetCurrentFrame,
        "Resets the current local scope to the default scope of the current thread" );
    python::def( "getLocals", pykd::getLocals, 
        "Get list of local variables" );
    python::def( "getLocal", pykd::getLocal,
        "Get the fucntion's local variable by name" );
    python::def( "getParams", pykd::getParams, 
        "Get list of function arguments as list of tuple (name, value ) " );
    python::def( "getParam", pykd::getParam, 
        "Get the function argument by name" );
    python::def( "stackAlloc", pykd::stackAlloc,
        "Allocate bytes of space in the stack in the target process" );
    python::def( "stackFree", pykd::stackFree,
        "Free space in the stack previously allocated by stackAlloc" );
    python::def( "pushStack", pykd::pushStack,
        "Push a value into a stack" );
    python::def( "popStack", pykd::popStack,
        "Pop a value from a stack" );

    // breakpoints
    python::def( "setBp", &Breakpoint::setSoftwareBreakpoint,
        setSoftwareBreakpoint_( python::args( "offset", "callback" ),"Set software breakpoint on execution" )[python::return_value_policy<python::manage_new_object>()]);
    python::def( "setBp", &Breakpoint::setHardwareBreakpoint, 
        setHardwareBreakpoint_( python::args( "offset", "size", "accsessType", "callback" ),"Set hardware breakpoint")[python::return_value_policy<python::manage_new_object>()]);
    python::def("getNumberBreakpoints", &Breakpoint::getNumberBreakpoints,
        "Return number of breakpoints in the current process" );
    python::def( "getBp", &Breakpoint::getBreakpointByIndex, python::return_value_policy<python::manage_new_object>(), 
        "Return breakpoint object by index");
    python::def("removeBp", &Breakpoint::removeBreakpointByIndex,
        "Remove breakpoint by index");
    python::def("removeAllBp", &Breakpoint::removeAllBreakpoints,
        "Remove all breakpoints");

    // processes and threads
    python::def ( "getNumberProcesses", pykd::getNumberProcesses,
        "Return number of processes on the target system" );
    python::def( "getProcessId", pykd::getProcessIdByIndex,
        "Return process ID by index" );
    python::def( "getProcessOffset", pykd::getProcessOffset, getProcessOffset_( python::args( "Id" ), 
        "Return the location in the target's memory of the process structure ( PEB )" ) );
    python::def( "getProcessSystemID", pykd::getProcessSystemId, getProcessSystemId_(  python::args( "Id" ), 
        "Return system process ID ( PID )" ) );
    python::def("getProcessExeName", pykd::getProcessExecutableName, getProcessExecutableName_(python::args("Id"),
        "Return name of executable file of the process"));
    python::def( "getProcessIdBySystemID", pykd::getProcessIdBySystemId, 
        "Return process ID by the system's process ID ( PID )" );
    python::def( "getCurrentProcess", pykd::getCurrentProcess, 
        "Return current offset" );
    python::def( "getCurrentProcessId", pykd::getCurrentProcessId,
        "Return current process ID" );
    python::def( "setCurrentProcess", pykd::setCurrentProcess,
        "Set current process" );
    python::def( "setCurrentProcessId", pykd::setCurrentProcessId,
        "Set current process by debug ID" );
    python::def( "getImplicitProcess", pykd::getImplicitProcessOffset,
        "Return implicit process" );
    python::def( "setImplicitProcess", pykd::setImplicitProcess,
        "Set implicit process" );
    python::def( "getProcessThreads", pykd::getProcessThreads,
        "Get all process's threads " );
    python::def( "getTargetProcesses", pykd::getTargetProcesses,
        "Get all target processes " );

    python::def ( "getNumberThreads", pykd::getNumberThreads,
        "Return number of threads on the current system" );
    python::def( "getThreadId", pykd::getThreadIdByIndex,
        "Return thread id by index");
    python::def( "getThreadOffset", pykd::getThreadOffset, getThreadOffset_( python::args("Id"),
        "Return the location in the target's memory of the thread structure ( TEB )" ) );
    python::def( "getThreadSystemID", pykd::getThreadSystemId, getThreadSystemId_( python::args("Id"),
        "Return system thread ID ( TID )" ) );
    python::def( "getThreadIdByOffset", pykd::getThreadIdByOffset,
        "Return thread ID by the location in the target's memory of the thread structure" );
    python::def( "getThreadIdBySystemID", pykd::getThreadIdBySystemId, getThreadIdBySystemId_( python::args("Tid"),
        "Return thread ID by the system's thread ID ( TID )" ) );
    python::def( "getCurrentThread", pykd::getCurrentThread,
        "Return current thread offset" );
    python::def( "getCurrentThreadId", pykd::getCurrentThreadId,
        "Return current thread ID" );
    python::def("setCurrentThread", pykd::setCurrentThread,
        "Set current thread" );
    python::def("setCurrentThreadId",pykd::setCurrentThreadId,
        "Set current thread by debug ID");
    python::def( "getImplicitThread", pykd::getImplicitThreadOffset,
        "Return implicit thread" );
    python::def( "setImplicitThread", pykd::setImplicitThread,
        "Set implicit thread" );

    // modules
    python::def( "getModulesList", pykd::getModuleList,
        "Return list of modules for the current target" );

    // events
    python::def("getLastEvent", pykd::getLastEvent,
        "Get last debug event information");
    python::def("getLastException", pykd::getLastException,
        "Get last exception information");

    // symbol path
    python::def("getSymbolPath", pykd::getSymbolPath,
        "Returns current symbol path");
    python::def("setSymbolPath", pykd::setSymbolPath, 
        "Set current symbol path");
    python::def("appendSymbolPath", pykd::appendSymbolPath, 
        "Append current symbol path");

    python::def("getSrcPath", pykd::getSrcPath,
        "Return current source server path");
    python::def("setSrcPath", pykd::setSrcPath,
        "Set source path");
    python::def("appendSrcPath", pykd::appendSrcPath,
        "Append current source path");

    // synthetic symbol
    python::def("addSyntheticSymbol", pykd::addSyntheticSymbol,
        "The addSyntheticSymbol function adds a synthetic symbol to a module in the current process\n"
        "Note: reloading the symbols for the module deletes all synthetic symbols associated with that module.");
    python::def( "removeSyntheticSymbol", pykd::removeSyntheticSymbol,
        "The removeSyntheticSymbol function removes a synthetic symbol from a module in the current process" );

    // synthetic module
    python::def("addSyntheticModule", pykd::addSyntheticModule, addSyntheticModule_(python::args("base", "size", "name", "path"),
        "The addSyntheticModule function adds a synthetic module to the module list the debugger maintains for the current process"));
    python::def("removeSyntheticModule", pykd::removeSyntheticModule,
        "The removeSyntheticModule function removes a synthetic module from the module list the debugger maintains for the current process");

    // secondary callback data
    python::def("enumTagged", pykd::enumTagged,
        "Return the list of secondary callback data IDs (as a strings)" );
    python::def("loadTaggedBuffer", pykd::loadTaggedBuffer,
        "Read the buffer of secondary callback data by ID" );

    python::class_<kdlib::NumConvertable, boost::noncopyable>( "numVariant", "numVariant", python::no_init )
        //.def("__init__", python::make_constructor(&NumVariantAdaptor::getVariant) )
        .def( "__eq__", &NumVariantAdaptor::eq )
        .def( "__ne__", &NumVariantAdaptor::ne)
        .def( "__lt__", &NumVariantAdaptor::lt)
        .def( "__gt__", &NumVariantAdaptor::gt )
        .def( "__le__", &NumVariantAdaptor::le )
        .def( "__ge__", &NumVariantAdaptor::ge )
        .def( "__add__", &NumVariantAdaptor::add )
        .def( "__radd__", &NumVariantAdaptor::add )
        .def( "__sub__", &NumVariantAdaptor::sub )
        .def( "__rsub__", &NumVariantAdaptor::rsub )
        .def( "__mul__", &NumVariantAdaptor::mul )
        .def( "__rmul__", &NumVariantAdaptor::mul )
#if PY_VERSION_HEX < 0x03000000
        .def( "__div__", &NumVariantAdaptor::div )
        .def( "__rdiv__", &NumVariantAdaptor::rdiv )
#else
        .def("__truediv__", &NumVariantAdaptor::truediv)
        .def("__rtruediv__", &NumVariantAdaptor::rtruediv)
#endif
        .def("__floordiv__", &NumVariantAdaptor::div)
        .def("__rfloordiv__", &NumVariantAdaptor::rdiv)
        .def( "__mod__", &NumVariantAdaptor::mod )
        .def( "__rmod__", &NumVariantAdaptor::rmod )
        .def( "__rshift__", &NumVariantAdaptor::rshift )
        .def( "__rrshift__", &NumVariantAdaptor::rrshift )
        .def( "__lshift__", &NumVariantAdaptor::lshift )
        .def( "__rlshift__", &NumVariantAdaptor::rlshift )
        .def( "__and__", &NumVariantAdaptor::and )
        .def( "__rand__", &NumVariantAdaptor::and )
        .def( "__or__", &NumVariantAdaptor::or )
        .def( "__ror__", &NumVariantAdaptor::or )
        .def( "__xor__", &NumVariantAdaptor::xor )
        .def( "__rxor__", &NumVariantAdaptor::xor )
        .def( "__neg__", &NumVariantAdaptor::neg )
        .def( "__pos__", &NumVariantAdaptor::pos ) 
        .def( "__invert__", &NumVariantAdaptor::invert ) 
        .def( "__nonzero__", &NumVariantAdaptor::nonzero )
        .def( "__str__", &NumVariantAdaptor::str )
        .def( "__hex__", &NumVariantAdaptor::hex )
        .def( "__long__", &NumVariantAdaptor::long_ )
        .def( "__int__", &NumVariantAdaptor::int_ )
        .def( "__index__", &NumVariantAdaptor::long_ )
        .def( "__hash__", &NumVariantAdaptor::long_ )
        .def( "__float__", &NumVariantAdaptor::float_)
        .def( "isInteger", &NumVariantAdaptor::isInteger )
        ;

        NumVariantAdaptor::registerNumConvertion();


    python::class_<kdlib::TargetSystem, kdlib::TargetSystemPtr, boost::noncopyable>("targetSystem", "Class representing target system", python::no_init)
        .def("__init__", python::make_constructor(&TargetSystemAdapter::getCurrent))
        .def("__init__", python::make_constructor(&TargetSystemAdapter::getSystem))
        .def("getNumber", TargetSystemAdapter::getNumberSystems,
            "Return number of systems").staticmethod("getNumber")
        .def("getCurrent", TargetSystemAdapter::getCurrent,
            "Return current target system").staticmethod("getCurrent")
        .def("setCurrent", TargetSystemAdapter::setCurrent,
            "set system as a current")
        .def("getSystemById", TargetSystemAdapter::getSystemById,
            "Return target system by id").staticmethod("getSystemById")
        .add_property("desc", TargetSystemAdapter::getDescription,
            "Retunr target system description")
        .add_property("id", TargetSystemAdapter::getId,
            "Return id of the target system" )
        .def("isDumpAnalyzing", TargetSystemAdapter::isDumpAnalyzing,
            "Check if it is a dump analyzing ( not living debuggee )")
        .def("isKernelDebugging", TargetSystemAdapter::isKernelDebugging,
            "Check if kernel debugging is running")
        .def("is64bitSystem", TargetSystemAdapter::is64bitSystem,
            "Check if 64 bit system running" )
        .def("isCurrent", TargetSystemAdapter::isCurrent,
            "Check if the target is current")
        .def("getNumberProcesses", TargetSystemAdapter::getNumberProcesses,
            "Return processed number of the target system")
        .def("getProcess", TargetSystemAdapter::getProcessByIndex,
            "Return process by index")
        .def("getProcessById", TargetSystemAdapter::getProcessById,
            "Return process by id")
        .def("getProcessBySystemId", TargetSystemAdapter::getProcessBySystemId,
            "Return process by PID")
        .def("currentProcess", TargetSystemAdapter::getCurrentProcess,
            "Return current process")
        .def("processes", TargetSystemAdapter::getProcessesList,
            "get list of processes for the target system")
        .def("__str__", TargetSystemAdapter::print)
        ;

    python::class_<kdlib::TargetProcess, kdlib::TargetProcessPtr, boost::noncopyable>("targetProcess", "Class representing process in the target system", python::no_init )
        .def("__init__", python::make_constructor(&TargetProcessAdapter::getProcess))
        .def("__init__", python::make_constructor(&TargetProcessAdapter::getCurrent))
        .def("getNumber", TargetProcessAdapter::getNumberProcesses,
            "Return number of processes" ).staticmethod("getNumber")
        .def("getCurrent", TargetProcessAdapter::getCurrent,
            "Return a current process" ).staticmethod("getCurrent")
        .def("getProcessById", TargetProcessAdapter::getProcessById,
            "Return process by id").staticmethod("getProcessById")
        .add_property("systemID", TargetProcessAdapter::getSystemId,
            "Return system process ID ( PID )" )
        .add_property("id", TargetProcessAdapter::getId,
            "Return process id")
        .add_property("peb", TargetProcessAdapter::getPebOffset,
            "Return PEB address" )
        .add_property("exeName", TargetProcessAdapter::getExeName,
            "Return the process executable file name")
        .def("isCurrent", TargetProcessAdapter::isCurrent,
            "Check if the target is current")
        .def("isManaged", TargetProcessAdapter::isManaged,
            "Check if the target process is managed")
        .def("getNumberThreads", TargetProcessAdapter::getNumberThreads,
            "Return number of threads for this process" )
        .def("getThread", TargetProcessAdapter::getThreadByIndex,
            "Return thread by its index" )
        .def("getThreadById", TargetProcessAdapter::getThreadById,
            "Return thread by its index")
        .def("getThreadBySystemId", TargetProcessAdapter::getThreadBySystemId,
            "Return thread by tid")
        .def("currentThread", TargetProcessAdapter::getCurrentThread,
            "Return current thread" )
        .def("getNumberBreakpoints", TargetProcessAdapter::getNumberBreakpoints,
            "Return number of breakpoints for this process" )
        .def("getBreakpoint", TargetProcessAdapter::getBreakpointByIndex, python::return_value_policy<python::manage_new_object>(),
            "Return a breakpoint by it's index" )
        .def("getNumberModules", TargetProcessAdapter::getNumberModules,
            "Return number of modules for this process" )
        .def("getModule", TargetProcessAdapter::getModuleByIndex,
            "Return a module object by it's index" )
        .def("getModuleByOffset", TargetProcessAdapter::getModuleByOffset,
            "Return a module object by it's offset" )
        .def("getModuleByName", TargetProcessAdapter::getModuleByName,
            "Return a module object by it's name" )
        .def("setCurrent", TargetProcessAdapter::setCurrent,
            "Set this process as a current")
        .def("threads", TargetProcessAdapter::getThreadList,
            "Return list of threads for the target process")
        .def("breakpoints", TargetProcessAdapter::getBreakpointsList,
            "Return list of breakpoints for the target process")
        .def("modules", TargetProcessAdapter::getModulesList,
            "Return list of modules for the target process")
        .def("getManagedHeap", TargetProcessAdapter::getManagedHeap,
            "Return object representing a managed heap")
        .def("getManagedVar", TargetProcessAdapter::getManagedVar,
            "Return object representing a managed object in the target managed process")
        .def("__str__", TargetProcessAdapter::print)
         ;

    python::class_<kdlib::TargetThread, kdlib::TargetThreadPtr, boost::noncopyable>("targetThread", "Class representing thread in the target process", python::no_init )
        .def("__init__", python::make_constructor(&TargetThreadAdapter::getThread))
        .def("__init__", python::make_constructor(&TargetThreadAdapter::getCurrent))
        .def("getNumber", TargetThreadAdapter::getNumberThreads,
            "Return number of threads").staticmethod("getNumber")
        .def("getCurrent", TargetThreadAdapter::getCurrent,
            "Return a current thread").staticmethod("getCurrent")
        .def("getThreadById", TargetThreadAdapter::getThreadById,
            "Return thread by id").staticmethod("getThreadById")
        .add_property("id", TargetThreadAdapter::getId,
            "Return thread's id")
        .add_property("systemID", TargetThreadAdapter::getSystemId,
            "Return system thread ID ( TID )" )
        .add_property("teb", TargetThreadAdapter::getTebOffset,
            "Return TEB address" )
        .add_property( "ip", TargetThreadAdapter::getIP, 
            "instruction pointer" )
        .add_property( "instructionOffset", TargetThreadAdapter::getIP,
            "Return an instruction offset" )
        .add_property( "fp", TargetThreadAdapter::getFP,
            "frame pointer" )
        .add_property( "frameOffset",TargetThreadAdapter::getFP,
            "Return a frame's offset" )
        .add_property( "sp", TargetThreadAdapter::getSP, 
            "stack pointer" )
        .add_property( "stackOffset", TargetThreadAdapter::getSP,
            "Return a stack pointer" )
        .def("setCurrent", TargetThreadAdapter::setCurrent,
            "Set this thread current")
        .def("isCurrent", TargetThreadAdapter::isCurrent,
            "Check if this thread is current")
        .def("stack", TargetThreadAdapter::getStack,
            "Get thread's stack trace")
        .def("__str__", TargetThreadAdapter::print)
        ;

    python::class_<TargetHeapIterator>("targetHeapIterator", "iterator for typedVar array", python::no_init)
        .def("__iter__", &TargetHeapIterator::self)
        .def("__len__", &TargetHeapIterator::length)
#if PY_VERSION_HEX < 0x03000000
        .def("next", &TargetHeapIterator::next)
#else
        .def("__next__", &TargetHeapIterator::next)
#endif
        ;

    python::class_<kdlib::TargetHeap, kdlib::TargetHeapPtr, boost::noncopyable>("targetHeap", "Class representing heap in the target process", python::no_init )
        .def("entries", &TargetHeapAdapter::getEntries, TargetHeap_getEntries(python::args("typeName", "minSize", "maxSize"),
            "Return heap's entries iterator object")[python::return_value_policy<python::manage_new_object>()] )
         ;

    python::class_<kdlib::Module, kdlib::ModulePtr, python::bases<kdlib::NumConvertable>, boost::noncopyable>("module", "Class representing executable module", python::no_init)
        .def("__init__", python::make_constructor(&ModuleAdapter::loadModuleByName))
        .def("__init__", python::make_constructor(&ModuleAdapter::loadModuleByOffset))
        .def("begin", ModuleAdapter::getBase,
            "Return start address of the module")
        .def("end", ModuleAdapter::getEnd,
            "Return end address of the module")
        .def("size", ModuleAdapter::getSize,
            "Return size of the module")
        .def("name", ModuleAdapter::getName,
            "Return name of the module")
        .def("reload", ModuleAdapter::reloadSymbols,
            "(Re)load symbols for the module")
        .def("image", ModuleAdapter::getImageName,
            "Return name of the image of the module")
        .def("symfile", ModuleAdapter::getSymFile,
            "Return the full path to the module's symbol information")
        .def("offset", ModuleAdapter::getSymbolVa,
            "Return offset of the symbol")
        .def("findSymbol", ModuleAdapter::findSymbol, Module_findSymbol(python::args("offset", "showDisplacement"),
            "Return symbol name by virtual address"))
        .def("findSymbolAndDisp", ModuleAdapter::findSymbolAndDisp,
            "Return tuple(symbol_name, displacement) by virtual address")
        .def("rva", ModuleAdapter::getSymbolRva,
            "Return rva of the symbol")
        .def("sizeof", ModuleAdapter::getSymbolSize,
            "Return a size of the type or variable")
        .def("type", ModuleAdapter::getTypeByName,
            "Return typeInfo class by type name")
#if 0
        .def("typedVar", ModuleAdapter::getTypedVarByAddr,
            "Return a typedVar class instance")
        .def("typedVar", ModuleAdapter::getTypedVarByName,
            "Return a typedVar class instance")
        .def("typedVar", ModuleAdapter::getTypedVarByTypeName,
            "Return a typedVar class instance")
        .def("typedVar", ModuleAdapter::getTypedVarWithPrototype,
            "Return a typedVar class instance")
        .def("typedVarList", ModuleAdapter::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory")
        .def("typedVarArray", ModuleAdapter::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory")
#endif
        .def("containingRecord", ModuleAdapter::containingRecord,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does")
        .def("enumSymbols", ModuleAdapter::enumSymbols, Module_enumSymbols(python::args("mask"),
             "Return list of tuple ( symbolname, offset )"))
        .def("enumTypes", ModuleAdapter::enumTypes, Module_enumTypes(python::args("mask"),
            "Return list of types name"))
        .def("checksum", ModuleAdapter::getCheckSum,
            "Return a image file checksum: IMAGE_OPTIONAL_HEADER.CheckSum" )
        .def("timestamp", ModuleAdapter::getTimeDataStamp,
            "Return a low 32 bits of the time stamp of the image: IMAGE_FILE_HEADER.TimeDateStamp" )
        .def("unloaded", ModuleAdapter::isUnloaded,
            "Returns a flag that the module was unloaded" )
        .def("um", ModuleAdapter::isUserMode,
            "Returns a flag that the module is a user-mode module" )
        .def("queryVersion", ModuleAdapter::getVersionInfo,
            "Return string from the module's version resources" )
        .def("getVersion", ModuleAdapter::getVersion,
            "Return tuple of the module's file version")
        .def("getFixedFileInfo", ModuleAdapter::getFixedFileInfo,
            "Return FixedFileInfo" )
        .def("hasSymbol", ModuleAdapter::isContainedSymbol,
            "Check if a module has the specified symbol")
        .def("__getattr__", ModuleAdapter::getAttrByName,
            "Return symbol offset or type as attribute" )
        .def("__getitem__", ModuleAdapter::getItemByKey,
            "Return symbol offset or type as item" )
        .def("__contains__", ModuleAdapter::isContainedSymbol)
        .def( "__str__", &ModuleAdapter::print );

#if 0
    python::class_<kdlib::TypeInfo, kdlib::TypeInfoPtr, python::bases<kdlib::NumConvertable>, boost::noncopyable >("typeInfo", "Class representing typeInfo", python::no_init)
        .def("__init__", python::make_constructor(pykd::getTypeInfoByName))
        .def("name", TypeInfoAdapter::getName,
            "Return type name")
        .def("scopeName", TypeInfoAdapter::getScopeName,
            "Return name of type scope ( module name )")
        .def("size", TypeInfoAdapter::getSize,
            "Return type size")
        .def("staticOffset", TypeInfoAdapter::getStaticOffset,
            "Return offset of the static field")
        .def("fieldOffset", TypeInfoAdapter::getElementOffset,
            "Return offset of the nonstatic field")
        .def("fieldOffset", TypeInfoAdapter::getElementOffsetByIndex,
            "Return offset of the nonstatic field by index")
        .def("isStaticField", TypeInfoAdapter::isStaticField,
            "Return True if a field is a static field by field name")
        .def("isStaticField", TypeInfoAdapter::isStaticFieldByIndex,
            "Return True if a field is a static field by field index")
        .def("isConstField", TypeInfoAdapter::isConstField,
            "Return True if a field is a const field by field name")
        .def("isConstField", TypeInfoAdapter::isConstFieldByIndex,
            "Return True if a field is a const field by field index")
        .def("bitOffset", TypeInfoAdapter::getBitOffset,
            "Return bit field's offset")
        .def("bitWidth", TypeInfoAdapter::getBitWidth,
            "Return bit field's length")
        .def("getNumberFields", TypeInfoAdapter::getElementCount,
            "Return number of fields")
        .def("field", TypeInfoAdapter::getElementByIndex,
            "Return field's type by index")
        .def("field", TypeInfoAdapter::getElementByName,
            "Return field's type")
        .def("hasField", TypeInfoAdapter::hasField,
            "Return True if type has a field with the specified name")
        .def( "fieldName", TypeInfoAdapter::getElementName,
            "Return name of struct field by index" )
        .def( "fields", TypeInfoAdapter::getFields,
            "Return list of tuple ( filedName, fieldType )" )
        .def( "members", TypeInfoAdapter::getMembers,
            "Return list of tuple ( memberName, fieldType ). Only defined member, not inherited from base class")
        .def( "getNumberMethods", TypeInfoAdapter::getMethodsCount,
            "Return number of methods" )
        .def( "method", TypeInfoAdapter::getMethodByName,
            "Return method's type by name")
        .def( "method", TypeInfoAdapter::getMethodByIndex,
            "Return method's by index")
        .def( "hasMethod", TypeInfoAdapter::hasMethod,
            "Return True if type has a method with the specified name")
        .def( "methodName", TypeInfoAdapter::getMethodName,
            "Return method's name")
        .def( "methods", TypeInfoAdapter::getMethods,
            "Return list of tuples ( methodName, methodPrototype )" )
        .def( "getNumberBaseClasses", TypeInfoAdapter::getBaseClassesCount,
            "Return number of base classes" )
        .def( "baseClass", TypeInfoAdapter::getBaseClassByName,
            "Return a base class's type by name" )
        .def( "baseClass", TypeInfoAdapter::getBaseClassByIndex,
            "Return a base class's type by index" )
        .def("baseClassOffset", TypeInfoAdapter::getBaseClassOffsetByName,
            "Return a base class offset by name")
        .def("baseClassOffset", TypeInfoAdapter::getBaseClassOffsetByIndex,
            "Return a base class offset by index")
        .def("baseClasses", TypeInfoAdapter::getBaseClasses,
            "Return list of tuples ( baseClassName, baseClassOffset, baseClassType)")
        .def( "getTemplateArgs", TypeInfoAdapter::getTemplateArgs,
            "Return list if template arguments" )
        .def( "deref", TypeInfoAdapter::deref,
            "Return type of pointer" )
        .def( "append", TypeInfoAdapter::appendField,
            "Add a new field to custom defined struct" )
        .def( "ptrTo", TypeInfoAdapter::ptrTo, TypeInfo_ptrTo( python::args( "ptrSize" ),
            "Return pointer to the type" ) )
        .def( "arrayOf", TypeInfoAdapter::arrayOf,
            "Return array of the type" )
        .def( "isArray", TypeInfoAdapter::isArray,
            "Return flag: type is array" )
        .def( "isPointer", TypeInfoAdapter::isPointer,
            "Return flag: type is pointer" )
        .def( "isVoid", TypeInfoAdapter::isVoid,
            "Return flag: type is void" )
        .def( "isBase", TypeInfoAdapter::isBase,
            "Return flag: type is base" )
        .def( "isUserDefined", TypeInfoAdapter::isUserDefined,
            "Return flag: type is UDT" )
        .def( "isEnum", TypeInfoAdapter::isEnum,
            "Return flag: type is enum" )
        .def( "isBitField", TypeInfoAdapter::isBitField,
            "Return flag: type is bit field" )
        .def( "isFunction", TypeInfoAdapter::isFunction,
            "Return flag: type is function" )
        .def( "isConstant", TypeInfoAdapter::isConstant,
            "Return flag: type is constant" )
        .def( "isVtbl", TypeInfoAdapter::isVtbl,
            "Return true if no type is specified" )
        .def( "isNoType", TypeInfoAdapter::isNoType,
            "Return true if type is virtual table" )
        .def( "isTemplate", TypeInfoAdapter::isTemplate,
            "Return true if type is template" )
        .def( "getCallingConvention", TypeInfoAdapter::getCallingConvention,
            "Returns an indicator of a methods calling convention: callingConvention" )
        .def( "getClassParent", TypeInfoAdapter::getClassParent,
            "Return class parent" )
        .def( "getTypedVar", getTypedVarByTypeInfo,
            "return typedVar instance" )
        .def( "__str__", TypeInfoAdapter::str,
            "Return type as a printable string" )
        .def( "__getattr__", TypeInfoAdapter::getElementAttr )
        .def( "__len__", TypeInfoAdapter::getElementCount )
        .def( "__getitem__", TypeInfoAdapter::getElementByIndex )
        .def( "__getitem__", TypeInfoAdapter::getElementByKey )
        .def( "__dir__", TypeInfoAdapter::getElementDir )
        .def("__contains__", TypeInfoAdapter::hasFieldOrMethod)
#if PY_VERSION_HEX >= 0x03000000
        .def("__bool__", TypeInfoAdapter::isNotZero )
#else
        .def("__iszero__", TypeInfoAdapter::isZero )
#endif
        ;
#endif

#if 0
    python::class_<TypedVarIterator>("typedVarIterator", "iterator for typedVar array", python::no_init)
        .def("__iter__", &TypedVarIterator::self)
#if PY_VERSION_HEX < 0x03000000
        .def("next", &TypedVarIterator::next)
#else
        .def("__next__", &TypedVarIterator::next)
#endif
        ;
#endif

#if 0
    python::class_<kdlib::TypedVar, kdlib::TypedVarPtr, python::bases<kdlib::NumConvertable>, boost::noncopyable >("typedVar",
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance", python::no_init  )
        .def("__init__", python::make_constructor(pykd::getTypedVarByName) )
        .def("__init__", python::make_constructor(pykd::getTypedVarByTypeName) )
        .def("__init__", python::make_constructor(pykd::getTypedVarByTypeInfo) )
        .def("__init__", python::make_constructor(pykd::getTypedVarWithPrototype) )
        .def("getLocation", TypedVarAdapter::getLocation,
            "Return location of the variable")
        .def("getAddress", TypedVarAdapter::getAddress, 
            "Return virtual address" )
        .def("getDebugStart", TypedVarAdapter::getDebugStart, 
            "Return end location of the function's prologue code (virtual address)" )
        .def("getDebugEnd", TypedVarAdapter::getDebugEnd, 
            "Return beginning location of the function's epilogue code (virtual address)" )
        .def("sizeof", TypedVarAdapter::getSize,
            "Return size of a variable in the target memory" )
        .def("fieldOffset", TypedVarAdapter::getFieldOffsetByName,
            "Return target field offset" )
        .def("getNumberFields", TypedVarAdapter::getElementCount,
            "Return number of fields")
        .def("field", TypedVarAdapter::getField,
            "Return field of structure")
        .def("field", TypedVarAdapter::getElementByIndex,
            "Return field of structure or array" )
        .def("setField", TypedVarAdapter::setField,
            "Set field of structure")
        .def("setField", TypedVarAdapter::setElementByIndex,
            "Set field of a stricture or an element of array")
        .def("hasField", TypedVarAdapter::hasField,
            "Check if a typedVar object has the specified field")
        .def( "fields", TypedVarAdapter::getFields,
            "Return list of tuple ( filedName, fieldOffset, fieldValue )" )
        .def ("members", TypedVarAdapter::getMembers,
            "Return list of tuple ( filedName, fieldOffset, fieldValue )")
        .def( "fieldName", TypedVarAdapter::getElementName,
            "Return name of struct field by index" )
        .def("method", TypedVarAdapter::getMethodByName, ( python::arg("name"), python::arg("prototype") = "" ),
            "Return method of class as an object attribute" )
        .def("hasMethod", TypedVarAdapter::hasMethod,
            "Check if a typedVar object has the specified method")
        .def("deref",TypedVarAdapter::deref,
            "Return value by pointer" )
        .def("rawBytes", TypedVarAdapter::getRawBytes,
            "Return list of bytes" )
        .def("type", TypedVarAdapter::getType,
            "Return typeInfo instance" )
        .def("castTo", TypedVarAdapter::castByName,
            "Cast variable to the type and return new typedVar instance")
        .def("castTo", TypedVarAdapter::castByTypeInfo,
            "Cast variable to the type and return new typedVar instance")
        .def("call", python::raw_function(pykd::callFunctionByVar, 0) )
        .def("__getattr__", TypedVarAdapter::getFieldAttr,
            "Return field of structure as an object attribute" )
        .def("__setattr__", TypedVarAdapter::setFieldAttr )
        .def( "__str__", TypedVarAdapter::print )
        .def("__len__", TypedVarAdapter::getElementCount )
        .def("__getitem__", TypedVarAdapter::getElementByIndex )
        .def("__setitem__", TypedVarAdapter::setElementByIndex )
        .def( "__getitem__", TypedVarAdapter::getFieldByKey )
        .def( "__setitem__", TypedVarAdapter::setFieldByKey )
        .def("__dir__", TypedVarAdapter::getElementsDir)
        .def("__call__", python::raw_function(pykd::callFunctionByVar, 0) )
        .def("__iter__", TypedVarAdapter::getArrayIter, python::return_value_policy<python::manage_new_object>())
        .def("__contains__", TypedVarAdapter::hasField)
#if PY_VERSION_HEX >= 0x03000000
        .def("__bool__", TypedVarAdapter::isNotZero)
#else
        .def("__iszero__", TypedVarAdapter::isZero)
#endif
        ;
#endif

    python::class_<BaseTypesEnum>("baseTypes", "base types enumeration",  boost::python::no_init)
        .add_static_property( "UInt1B", &BaseTypesEnum::getUInt1B )
        .add_static_property( "UInt2B", &BaseTypesEnum::getUInt2B )
        .add_static_property( "UInt4B", &BaseTypesEnum::getUInt4B )
        .add_static_property( "UInt8B", &BaseTypesEnum::getUInt8B )
        .add_static_property( "Int1B", &BaseTypesEnum::getInt1B )
        .add_static_property( "Int2B", &BaseTypesEnum::getInt2B )
        .add_static_property( "Int4B", &BaseTypesEnum::getInt4B )
        .add_static_property( "Int8B", &BaseTypesEnum::getInt8B )
        .add_static_property( "Long", &BaseTypesEnum::getLong )
        .add_static_property( "ULong", &BaseTypesEnum::getULong )
        .add_static_property( "Bool", &BaseTypesEnum::getBool )
        .add_static_property( "Char", &BaseTypesEnum::getChar )
        .add_static_property( "WChar", &BaseTypesEnum::getWChar )
        .add_static_property( "VoidPtr", &BaseTypesEnum::getVoidPtr )
        .add_static_property( "Float", &BaseTypesEnum::getFloat )
        .add_static_property( "Double", &BaseTypesEnum::getDouble )
        ;

    python::class_<kdlib::StackFrame, kdlib::StackFramePtr, boost::noncopyable>( "stackFrame",
        "class for stack's frame representation", python::no_init  )
        .add_property( "ip", StackFrameAdapter::getIP, 
            "instruction pointer" )
        .add_property( "instructionOffset", StackFrameAdapter::getIP,
            "Return a frame's instruction offset" )
        .add_property( "ret",StackFrameAdapter::getRET,
            "return pointer" )
        .add_property( "returnOffset",StackFrameAdapter::getRET,
            "Return a frame's return offset" )
        .add_property( "fp", StackFrameAdapter::getFP,
            "frame pointer" )
        .add_property( "frameOffset",StackFrameAdapter::getFP,
            "Return a frame's offset" )
        .add_property( "sp", StackFrameAdapter::getSP, 
            "stack pointer" )
        .add_property( "stackOffset", StackFrameAdapter::getSP,
            "Return a frame's stack offset" )
        .def( "getParams", StackFrameAdapter::getParamsList, 
            "return set of function's parameters as a list of tuple (name, value)")
        .add_property( "params", StackFrameAdapter::getParamsDict,
            "return set of function's parameters as a dict (name : value)")
        .def( "getParam", StackFrameAdapter::getParam,
            "return function param by it's name")
        .def( "getLocals", StackFrameAdapter::getLocalsList,
            "return set of function's local variables as a list of tuple (name, value)")
        .add_property("locals", StackFrameAdapter::getLocalsDict,
            "return a set of function's local variables as a dict (name : value)")
        .def( "getLocal", StackFrameAdapter::getLocal,
            "return the function's local variable by it's name")
        .def( "switchTo", StackFrameAdapter::switchTo,
            "Make this frame a current")
        .def( "isInline", StackFrameAdapter::isInline,
            "this virtual frame of inlined function" )
        .def( "findSymbol", StackFrameAdapter::findSymbol,
            "return symbol for frame's instruction pointer")
        .def( "getSourceLine", StackFrameAdapter::getSourceLine,
            "return source line for stack frame's function" )
        .def( "__str__", StackFrameAdapter::print );

    python::class_<CPUContextAdapter>("cpu", "class for CPU context representation" )
         //.def("__init__", python::make_constructor(CPUContextAdapter::getCPUContext) )
         .add_property("ip", &CPUContextAdapter::getIP )
         .add_property("sp", &CPUContextAdapter::getSP )
         .add_property("fp", &CPUContextAdapter::getFP )
         .def("getCPUType", &CPUContextAdapter::getCPUType )
         .def("getCPUMode",  &CPUContextAdapter::getCPUMode )
         .def("__getattr__",  &CPUContextAdapter::getRegisterByName )
         .def("__getitem__",  &CPUContextAdapter::getRegisterByIndex )
         .def("__len__", &CPUContextAdapter::getRegisterNumber );

    python::class_<kdlib::SystemInfo>(
        "systemVersion", "Operation system version", python::no_init)
        //.def_readonly( "platformId", &SystemVersion::platformId,
        //    "Platform ID: VER_PLATFORM_WIN32_NT for NT-based Windows")
        .def_readonly( "win32Major", &kdlib::SystemInfo::majorVersion,
            "Major version number of the target's operating system")
        .def_readonly( "win32Minor", &kdlib::SystemInfo::minorVersion,
            "Minor version number of the target's operating system")
        .def_readonly("servicePack", &kdlib::SystemInfo::servicePackNumber,
            "Service Pack Number" )
        .def_readonly("buildNumber", &kdlib::SystemInfo::buildNumber,
            "Build number for the target's operating system")
        .def_readonly( "buildString", &kdlib::SystemInfo::buildDescription,
            "String that identifies the build of the system")
        //.def_readonly( "servicePackString", &SystemVersion::servicePackString,
        //    "String for the service pack level of the target computer")
        //.def_readonly( "isCheckedBuild", &SystemVersion::isCheckedBuild,
        //    "Checked build flag")
        .def("__str__", pykd::printSystemVersion,
            "Return object as a string");

    python::class_<kdlib::FixedFileInfo, FixedFileInfoPtr>(
        "FixedFileInfo", "Version information for a file", python::no_init )
        .def_readonly( "Signature", &kdlib::FixedFileInfo::Signature,
            "Contains the value 0xFEEF04BD" )
        .def_readonly( "StrucVersion", &kdlib::FixedFileInfo::StrucVersion,
            "The binary version number of this structure" )
        .def_readonly( "FileVersionMS", &kdlib::FixedFileInfo::FileVersionMS,
            "The most significant 32 bits of the file's binary version number" )
        .def_readonly( "FileVersionLS", &kdlib::FixedFileInfo::FileVersionLS,
            "The least significant 32 bits of the file's binary version number" )
        .def_readonly( "ProductVersionMS", &kdlib::FixedFileInfo::ProductVersionMS,
            "The most significant 32 bits of the binary version number of the product with which this file was distributed" )
        .def_readonly( "ProductVersionLS", &kdlib::FixedFileInfo::ProductVersionLS,
            "The least significant 32 bits of the binary version number of the product with which this file was distributed" )
        .def_readonly( "FileFlagsMask", &kdlib::FixedFileInfo::FileFlagsMask,
            "Contains a bitmask that specifies the valid bits in FileFlags" )
        .def_readonly( "FileFlags", &kdlib::FixedFileInfo::FileFlags,
            "Contains a bitmask that specifies the Boolean attributes of the file: FileFlag" )
        .def_readonly( "FileOS", &kdlib::FixedFileInfo::FileOS,
            "The operating system for which this file was designed" )
        .def_readonly( "FileType", &kdlib::FixedFileInfo::FileType,
            "The general type of file" )
        .def_readonly( "FileSubtype", &kdlib::FixedFileInfo::FileSubtype,
            "The function of the file. The possible values depend on the value of FileType" )
        .def_readonly( "FileDateMS", &kdlib::FixedFileInfo::FileDateMS,
            "The most significant 32 bits of the file's 64-bit binary creation date and time stamp" )
        .def_readonly( "FileDateLS", &kdlib::FixedFileInfo::FileDateLS,
            "The least significant 32 bits of the file's 64-bit binary creation date and time stamp" );

    python::class_<kdlib::ExceptionInfo>(
        "exceptionInfo", "Exception information", python::no_init )
        .def_readonly( "firstChance", &kdlib::ExceptionInfo::firstChance,
            "Specifies whether this exception has been previously encountered")
        .def_readonly( "exceptionCode", &kdlib::ExceptionInfo::exceptionCode,
            "The reason the exception occurred")
        .def_readonly( "exceptionFlags", &kdlib::ExceptionInfo::exceptionFlags,
            "The exception flags")
        .def_readonly( "exceptionRecord", &kdlib::ExceptionInfo::exceptionRecord,
            "A pointer to an associated EXCEPTION_RECORD structure")
        .def_readonly( "exceptionAddress", &kdlib::ExceptionInfo::exceptionAddress,
            "The address where the exception occurred")
        .add_property( "parameters", &getExceptionInfoParameters,
            "An array of additional arguments that describe the exception")
        .def( "__str__", pykd::printExceptionInfo,
            "Return object as a string");

    python::enum_<kdlib::FileFlag>("FileFlag", "Attributes of the file")
        .value("Debug", kdlib::FileFlagDebug)
        .value("PreRelease", kdlib::FileFlagPreRelease)
        .value("Patched", kdlib::FileFlagPatched)
        .value("PrivateBuild", kdlib::FileFlagPrivateBuild)
        .value("InfoInferred", kdlib::FileFlagInfoInferred)
        .value("SpecialBuild", kdlib::FileFlagSpecialBuild)
        ;

    python::enum_<kdlib::EventType>("eventType", "Type of debug event")
        .value("Breakpoint", kdlib::EventTypeBreakpoint)
        .value("Exception", kdlib::EventTypeException)
        .value("CreateThread", kdlib::EventTypeCreateThread)
        .value("ExitThread", kdlib::EventTypeExitThread)
        .value("CreateProcess", kdlib::EventTypeCreateProcess)
        .value("ExitProcess", kdlib::EventTypeExitProcess)
        .value("LoadModule", kdlib::EventTypeLoadModule)
        .value("UnloadModule", kdlib::EventTypeUnloadModule)
        .value("SystemError", kdlib::EventTypeSystemError)
        .value("SessionStatus", kdlib::EventTypeSessionStatus)
        .value("ChangeDebuggeeState", kdlib::EventTypeChangeDebuggeeState)
        .value("ChangeEngineState", kdlib::EventTypeChangeEngineState)
        .value("ChangeSymbolState", kdlib::EventTypeChangeSymbolState)
        ;

    python::class_<pykd::DebugEvent>("debugEvent", "Debug event descriptions", python::no_init)
        .def_readonly("type", &DebugEvent::eventType)
        .def_readonly("process", &DebugEvent::process)
        .def_readonly("thread", &DebugEvent::thread)
        ;

    python::class_<kdlib::Disasm>("disasm", "Class disassemble a processor instructions",python::no_init)
        .def( "__init__", python::make_constructor(pykd::loadDisasm ) )
        .def( "__init__", python::make_constructor(pykd::loadDisasmWithOffset ) )
        .def( "disasm", DisasmAdapter::disassemble, 
            "Disassemble next instruction" )
        .def( "disasm", DisasmAdapter::jump, 
            "Disassemble from the specified offset" )
        .def( "asm", DisasmAdapter::assembly,
            "Insert assembled instruction to current offset" )
        .def( "begin", DisasmAdapter::begin,
            "Return begin offset" )
        .def( "current", DisasmAdapter::current, 
            "Return current offset" )
        .def( "length", DisasmAdapter::length,
            "Return current instruction length" )
        .def( "instruction", DisasmAdapter::instruction, 
            "Returm current disassembled instruction" )
        .def("opcode", DisasmAdapter::opcode,
            "Return list of bytes of the instruction opcode" )
        .def("opmnemo", DisasmAdapter::opmnemo,
            "Return mnemocode of the instruction")
        .def( "ea", DisasmAdapter::ea, 
            "Return effective address for last disassembled instruction or 0" )
        .def( "reset", DisasmAdapter::reset,
            "Reset current offset to begin" )
        .def( "findOffset", DisasmAdapter::getNearInstruction,
            "Return the location of a processor instruction relative to a given location" )
        .def( "jump",DisasmAdapter::jump, 
            "Change the current instruction" )
        .def( "jumprel", DisasmAdapter::jumprel, 
            "Change the current instruction" )
        .def( "__str__", DisasmAdapter::instruction );


#if 0
    python::class_<TypeInfoProviderIterator>("typeInfoProviderIterator", "iterator for type provider", python::no_init)
        .def("__iter__", &TypeInfoProviderIterator::self)
#if PY_VERSION_HEX < 0x03000000
        .def("next", &TypeInfoProviderIterator::next)
#else
        .def("__next__", &TypeInfoProviderIterator::next)
#endif
        ;
#endif

#if 0
    python::class_<kdlib::TypeInfoProvider, kdlib::TypeInfoProviderPtr, boost::noncopyable>("typeInfoProvider",
        "Get abstract access to different type info sources", python::no_init)
        .def( "getTypeByName", TypeInfoProviderAdapter::getTypeByName,
            "Get type info by it's name" )
        .def( "typeIterator", TypeInfoProviderAdapter::getTypeIterWithMask, python::return_value_policy<python::manage_new_object>(),
            "Return type iterator with specified mask")
        .def("__iter__", TypeInfoProviderAdapter::getTypeIter, python::return_value_policy<python::manage_new_object>())
        .def( "__getattr__", TypeInfoProviderAdapter::getTypeAsAttr )
         ;

    python::class_<SymbolEnumeratorAdapter>("symbolIterator", "Iterator for symbols", python::no_init)
        .def("__iter__", SymbolEnumeratorAdapter::getIter)
#if PY_VERSION_HEX < 0x03000000
        .def("next", &SymbolEnumeratorAdapter::next)
#else
        .def("__next__", &SymbolEnumeratorAdapter::next)
#endif
        ;
#endif

#if 0
    python::class_<kdlib::SymbolProvider, kdlib::SymbolProviderPtr, boost::noncopyable>("symbolProvider",
        "Get abstract access to different type info sources", python::no_init)
        .def("iter", SymbolProviderAdapter::getIterWithMask, python::return_value_policy<python::manage_new_object>(),
            "Return type iterator with specified mask")
        .def("__iter__", SymbolProviderAdapter::getIter, python::return_value_policy<python::manage_new_object>())
        ;
#endif

    python::enum_<kdlib::DebugCallbackResult>("eventResult", "Return value of event handler")
        .value("Proceed", kdlib::DebugCallbackProceed)
        .value("NoChange", kdlib::DebugCallbackNoChange)
        .value("Break", kdlib::DebugCallbackBreak)
        ;

    python::enum_<kdlib::ExecutionStatus>("executionStatus", "Execution Status")
        .value("NoChange", kdlib::DebugStatusNoChange )
        .value("Go", kdlib::DebugStatusGo )
        .value("Break", kdlib::DebugStatusBreak )
        .value("NoDebuggee", kdlib::DebugStatusNoDebuggee )
        ;

    python::enum_<kdlib::CallingConventionType>("callingConvention", "Calling convention for a function")
        .value("NearC", kdlib::CallConv_NearC )
        .value("FarC", kdlib::CallConv_FarC )
        .value("NearPascal", kdlib::CallConv_NearPascal )
        .value("FarPascal", kdlib::CallConv_FarPascal )
        .value("NearFast", kdlib::CallConv_NearFast )
        .value("FarFast", kdlib::CallConv_FarFast )
        .value("Skipped", kdlib::CallConv_Skipped )
        .value("NearStd", kdlib::CallConv_NearStd )
        .value("FarStd0", kdlib::CallConv_FarStd )
        .value("NearSys", kdlib::CallConv_NearSys )
        .value("FarSys", kdlib::CallConv_FarSys )
        .value("ThisCall", kdlib::CallConv_ThisCall )
        .value("MipsCall", kdlib::CallConv_MipsCall )
        .value("Generic", kdlib::CallConv_Generic )
        .value("AlphaCall ", kdlib::CallConv_AlphaCall )
        .value("PpcCall", kdlib::CallConv_PpcCall )
        .value("ShCall", kdlib::CallConv_ShCall )
        .value("ArmCall", kdlib::CallConv_ArmCall )
        .value("Am33Call", kdlib::CallConv_Am33Call )
        .value("TriCall", kdlib::CallConv_TriCall )
        .value("Sh5Call", kdlib::CallConv_Sh5Call )
        .value("M32RCall", kdlib::CallConv_M32RCall )
        .value("ClrCall", kdlib::CallConv_ClrCall )
        .value("Inline", kdlib::CallConv_Inline )
        ;

    python::enum_<kdlib::CPUType>("CPUType", "type of CPU")
        .value("I386", kdlib::CPU_I386 )
        .value("AMD64", kdlib::CPU_AMD64 )
        .value("ARM64", kdlib::CPU_ARM64 )
        .value("ARM", kdlib::CPU_ARM )
        ;

    python::enum_<kdlib::VarStorage>("Location", "Location of a variable")
        .value("Reg", kdlib::RegisterVar)
        .value("Memory", kdlib::MemoryVar)
        ;

    python::enum_<kdlib::MemoryProtect>("memoryProtect", "Memory protection attributes")
        .value("PageNoAccess", kdlib::PageNoAccess)
        .value("PageReadOnly", kdlib::PageReadOnly)
        .value("PageReadWrite", kdlib::PageReadWrite)
        .value("PageWriteCopy", kdlib::PageReadOnly)
        .value("PageExecute", kdlib::PageExecute)
        .value("PageExecuteRead", kdlib::PageExecuteRead)
        .value("PageExecuteReadWrite", kdlib::PageExecuteReadWrite)
        .value("PageExecuteWriteCopy", kdlib::PageExecuteWriteCopy)
        ;

    python::enum_<kdlib::MemoryState>("memoryState", "Memory state")
        .value("Commit", kdlib::MemCommit)
        .value("Reserve", kdlib::MemReserve)
        .value("Free", kdlib::MemFree)
        ;

    python::enum_<kdlib::MemoryType>("memoryType", "Memory type")
        .value("Mapped", kdlib::MemMapped)
        .value("Image", kdlib::MemImage)
        .value("Private", kdlib::MemPrivate)
        ;

    python::enum_<kdlib::ProcessDebugOptions>("ProcessDebugOptions", "Process debug option")
        .value("BreakOnStart", kdlib::ProcessBreakOnStart)
        .value("BreakOnStop", kdlib::ProcessBreakOnStop)
        .value("DebugChildren", kdlib::ProcessDebugChildren)
        .value("NoDebugHeap", kdlib::ProcessNoDebugHeap)
        .value("Default", kdlib::ProcessDebugDefault)
        ;

    python::class_<EventHandler, boost::noncopyable>(
        "eventHandler", "Base class for overriding and handling debug notifications" )
         .def( "onBreakpoint", &EventHandler::onBreakpoint,
            "Triggered breakpoint event. Parameter is int: ID of breakpoint\n"
            "For ignore event method must return eventResult.noChange" )
        .def( "onLoadModule", &EventHandler::onModuleLoad,
            "Triggered module load event. Parameter are long: module base, string: module name\n"
            "For ignore event method must return eventResult.noChange" )
        .def( "onUnloadModule", &EventHandler::onModuleUnload,
            "Triggered module unload event. Parameter are long: module base, string: module name\n"
            "For ignore event method must return eventResult.noChange" )
        .def( "onException", &EventHandler::onException,
            "Triggered exception event. Parameter - exceptionInfo\n"
            "For ignore event method must return eventResult.noChange" )
        .def( "onExecutionStatusChange", &EventHandler::onExecutionStatusChange,
            "Triggered execution status changed. Parameter - execution status.\n"
            "There is no return value" )
        .def( "onCurrentThreadChange", &EventHandler::onCurrentThreadChange,
            "The current thread has been changed, which implies that the current target and current process might also have changed.\n"
             "There is no return value" )
        .def( "onChangeLocalScope", &EventHandler::onChangeLocalScope,
            "The current local scope has been changed.\n"
            "There is no return value" )
        .def("onChangeSymbolPaths", &EventHandler::onChangeSymbolPaths,
            "Symbol paths has been changed.\n"
            "There is no return value" )
        .def("onChangeBreakpoints", &EventHandler::onChangeBreakpoints,
            "Breakpoints is changed for current process" )
        .def( "onDebugOutput", &EventHandler::onDebugOutput,
            "Request debug output" )
        .def("onStartInput", &EventHandler::onStartInput,
            "Request debug input" )
        .def("onStopInput", &EventHandler::onStopInput,
            "Debug input is completed")
        .def("onThreadStart", &EventHandler::onThreadStart,
            "New thread is started in the current process" )
        .def("onThreadStop", &EventHandler::onThreadStop,
            "A thread is stopped in the current thread")

   //     .def( "onSymbolsLoaded", &EventHandlerWrap::onSymbolsLoaded,
   //         "Triggered debug symbols loaded. Parameter - module base or 0\n"
   //         "There is no return value")
   //     .def( "onSymbolsUnloaded", &EventHandlerWrap::onSymbolsUnloaded,
   //         "Triggered debug symbols unloaded. Parameter - module base or 0 (all modules)\n"
   //         "There is no return value");
        ;

    python::class_<Breakpoint, boost::noncopyable>( "breakpoint",
        "class for CPU context representation", python::init<kdlib::MEMOFFSET_64>()) 
        .def( python::init<kdlib::MEMOFFSET_64, size_t, kdlib::ACCESS_TYPE>() )
        .def("getId", &Breakpoint::getId,
            "Return breakpoint ID" )
        .def("getOffset", &Breakpoint::getOffset,
            "Return breakpoint's memory offset")
        .def("remove", &Breakpoint::remove,
            "Remove breakpoint" )
        .def("onHit", &Breakpoint::onHit,
            "Breakpoint hit callback")
        .def("detach", &Breakpoint::detach, python::return_value_policy<python::manage_new_object>(),
            "detach breakpoint")
        ;

    python::class_<kdlib::SyntheticSymbol>(
        "syntheticSymbol", "Structure describes a synthetic symbol within a module", python::no_init)
        .def_readonly( "moduleBase", &kdlib::SyntheticSymbol::moduleBase,
            "The location in the target's virtual address space of the module's base address")
        .def_readonly( "symbolId", &kdlib::SyntheticSymbol::symbolId,
            "The symbol ID of the symbol within the module")
        .def("__str__", pykd::printSyntheticSymbol,
            "Return object as a string");

    python::enum_<kdlib::DebugOptions>("debugOptions", "Debug options")
        .value("AllowNetworkPaths", kdlib::AllowNetworkPaths)
        .value("DisallowNetworkPaths", kdlib::DisallowNetworkPaths)
        .value("InitialBreak", kdlib::InitialBreak)
        .value("FinalBreak", kdlib::FinalBreak)
        .value("FailIncompleteInformation", kdlib::FailIncompleteInformation)
        .value("DisableModuleSymbolLoad", kdlib::DisableModuleSymbolLoad)
        .value("DisallowImageFileMapping", kdlib::DisallowImageFileMapping)
        .value("PreferDml", kdlib::PreferDml)
        ;

    python::enum_<kdlib::BreakpointAccess>("breakpointAccess", "Breakpoint access types")
        .value("Read", kdlib::Read)
        .value("Write", kdlib::Write)
        .value("Execute", kdlib::Execute)
        ;

    python::enum_<kdlib::OutputFlag>("outputFlag", "Set of output mask")
        .value("Normal", kdlib::Normal)
        .value("Error", kdlib::Error)
        .value("Warning", kdlib::Warning)
        .value("Verbose", kdlib::Verbose)
        .value("Prompt", kdlib::Prompt)
        .value("PromptRegister", kdlib::PromptRegister)
        .value("ExtensionWarning", kdlib::ExtensionWarning)
        .value("Debuggee", kdlib::Debuggee)
        .value("DebuggeePrompt", kdlib::DebuggeePrompt)
        .value("Symbols", kdlib::Symbols)
        .value("Status", kdlib::Status)
        .value("All", kdlib::All)
        ;

    python::enum_<kdlib::DumpType>("dumpType", "Dump type")
        .value("Small", kdlib::Small)
        .value("Default", kdlib::Default)
        .value("Full", kdlib::Full)
        .value("Image", kdlib::Image)
        .value("KernelSmall", kdlib::KernelSmall)
        .value("Kernel", kdlib::Kernel)
        .value("KernelFull", kdlib::KernelFull)
        ;

    python::enum_<kdlib::DumpFormat>("dumpFormat", "Dump format")
        .value("UserSmallFullMemory", kdlib::UserSmallFullMemory)
        .value("UserSmallHandleData", kdlib::UserSmallHandleData)
        .value("UserSmallUnloadedModules", kdlib::UserSmallUnloadedModules)
        .value("UserSmallIndirectMemory", kdlib::UserSmallIndirectMemory)
        .value("UserSmallDataSegments", kdlib::UserSmallDataSegments)
        .value("UserSmallFilterMemory", kdlib::UserSmallFilterMemory)
        .value("UserSmallFilterPaths", kdlib::UserSmallFilterPaths)
        .value("UserSmallProcessThreadData", kdlib::UserSmallProcessThreadData)
        .value("UserSmallPrivateReadWriteMemory", kdlib::UserSmallPrivateReadWriteMemory)
        .value("UserSmallNoOptionalData", kdlib::UserSmallNoOptionalData)
        .value("UserSmallFullMemoryInfo", kdlib::UserSmallFullMemoryInfo)
        .value("UserSmallThreadInfo", kdlib::UserSmallThreadInfo)
        .value("UserSmallCodeSegments", kdlib::UserSmallCodeSegments)
        .value("UserSmallNoAuxiliaryState", kdlib::UserSmallNoAuxiliaryState)
        .value("UserSmallFullAuxiliaryState", kdlib::UserSmallFullAuxiliaryState)
        .value("UserSmallModuleHeaders", kdlib::UserSmallModuleHeaders)
        .value("UserSmallFilterTriage", kdlib::UserSmallFilterTriage)
        .value("UserSmallAddAvxXStateContext", kdlib::UserSmallAddAvxXStateContext)
        .value("UserSmallIptTrace", kdlib::UserSmallIptTrace)
        .value("UserSmallIgnoreInaccessibleMem", kdlib::UserSmallIgnoreInaccessibleMem)
        ;

    // C++ exception translation to python
    pykd::registerExceptions();
}

//////////////////////////////////////////////////////////////////////////////////

#if PY_VERSION_HEX >= 0x03000000

void pykd_deinit(void*)
{
    if ( kdlib::isInintilized() )
        kdlib::uninitialize();
}

PyMODINIT_FUNC
PyInit_pykd(void)
{
    static PyModuleDef_Base initial_base = {
        PyObject_HEAD_INIT(NULL)
        0, /* m_init */
        0, /* m_index */
        0 /* m_copy */
    };

    static PyMethodDef initial_methods[] = { { 0, 0, 0, 0 } };

    static struct PyModuleDef moduledef = { \
        initial_base,
        "pykd",
        0, /* m_doc */
        -1, /* m_size */
        initial_methods,
        0,  /* m_reload */
        0, /* m_traverse */
        0, /* m_clear */
        pykd_deinit
    };

    return boost::python::detail::init_module(moduledef, pykd_init);
}
#else

void pykd_deinit(PyObject*)
{
    if (kdlib::isInintilized())
        kdlib::uninitialize();
}

PyMODINIT_FUNC
initpykd()
{
    PyObject* moduleObj = boost::python::detail::init_module("pykd", pykd_init);

    PyObject* moduleDeiniter = PyCapsule_New( (void*)1, "pykd.__deinit__", pykd_deinit);

    PyModule_AddObject(moduleObj, "pykd.__deinit__", moduleDeiniter);
}

#endif
//////////////////////////////////////////////////////////////////////////////////

