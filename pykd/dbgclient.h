#pragma once

#include <string>
#include <dbgeng.h>
#include <dbghelp.h>

#include <boost\smart_ptr\scoped_ptr.hpp>

#include "dbgobj.h"
#include "dbgexcept.h"
#include "module.h"
#include "dbgio.h"

/////////////////////////////////////////////////////////////////////////////////

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class DebugClient;
typedef boost::shared_ptr<DebugClient>  DebugClientPtr;

/////////////////////////////////////////////////////////////////////////////////

class DebugClient : private DbgObject {

public:

    virtual ~DebugClient() {}

    static
    DebugClientPtr createDbgClient() {

        HRESULT                  hres;
        CComPtr<IDebugClient4>   client = NULL;

        hres = DebugCreate( __uuidof(IDebugClient4), (void **)&client );
        if ( FAILED( hres ) )
            throw DbgException("DebugCreate failed");

        return  createDbgClient( client );
    }

    static
    DebugClientPtr createDbgClient( IDebugClient4 *client ) {
        return DebugClientPtr( new DebugClient(client) );
    }

    static
    DebugClientPtr  setDbgClientCurrent( DebugClientPtr  newDbgClient );
 
    void loadDump( const std::wstring &fileName );

    void startProcess( const std::wstring  &processName );

    void attachProcess( ULONG  processId );

    void attachKernel( const std::wstring  &param );

    Module loadModule( const std::string  &moduleName ) {
        return Module( m_client, moduleName );
    }

    Module findModule( ULONG64  offset ) {
        return Module( m_client, offset ); 
    }

    ULONG64  addr64( ULONG64 addr );

    void dprint( const std::wstring &str, bool dml = false );

    void dprintln( const std::wstring &str, bool dml = false );

    void eprint( const std::wstring &str );

    void eprintln( const std::wstring &str );

    DbgOut  dout() {
        return DbgOut( m_client );
    }

    DbgIn din() {
        return DbgIn( m_client );
    }

public:

    CComPtr<IDebugClient4>&
    client() {
        return m_client;    
    }

    CComPtr<IDebugClient5>&
    client5() {
        return m_client5;    
    }

    CComPtr<IDebugControl4>&
    control() {
        return m_control;    
    }

private:

    DebugClient( IDebugClient4 *client ) : DbgObject( client ) {}
};

/////////////////////////////////////////////////////////////////////////////////

extern DebugClientPtr     g_dbgClient;

void loadDump( const std::wstring &fileName );

void startProcess( const std::wstring  &processName );

void attachProcess( ULONG  processId );

void attachKernel( const std::wstring  &param );

Module loadModule( const std::string  &moduleName );

Module findModule( ULONG64  offset );

/////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd










//#include "dbgext.h"
//#include "dbgeventcb.h"
//
/////////////////////////////////////////////////////////////////////////////////
//
//class dbgClient {
//
//public:
//
//    dbgClient() 
//    {
//        m_callbacks = NULL;
//        
//        IDebugClient4     *client = NULL;
//        DebugCreate( __uuidof(IDebugClient4), (void **)&client );
//    
//        m_ext = new DbgExt( client );
//        
//        client->Release();
//    }
//    
//    ~dbgClient() 
//    {
//        removeEventsMgr();
//            
//        delete m_ext;
//    }
//    
//    void startEventsMgr() {
// 
//         m_callbacks = new DbgEventCallbacksManager( m_ext->client );    
//    }
//    
//    void removeEventsMgr() {
//    
//        if ( m_callbacks )
//        {
//            delete m_callbacks;
//            m_callbacks = NULL;
//        }
//    }
//    
//private:
//
//    DbgExt                          *m_ext;
//    DbgEventCallbacksManager        *m_callbacks;  
//};
//
//extern dbgClient    g_dbgClient;
//
/////////////////////////////////////////////////////////////////////////////////