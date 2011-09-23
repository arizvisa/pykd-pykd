
#include "stdafx.h"

#include "diawrapper.h"
#include "utils.h"

namespace pyDia {

////////////////////////////////////////////////////////////////////////////////

PyObject *Exception::diaExceptTypeObject =  NULL;

const std::string Exception::descPrefix("pyDia: ");

////////////////////////////////////////////////////////////////////////////////

#define callSymbol(method) \
    callSymbolT( &IDiaSymbol::##method, __FUNCTION__, #method)

////////////////////////////////////////////////////////////////////////////////

std::string Exception::makeFullDesc(const std::string &desc, HRESULT hres)
{
    std::stringstream sstream;
    sstream << descPrefix << desc << " failed" << std::endl;
    sstream << "Return value is 0x" << std::hex << hres;

    PCHAR errMessage = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        hres,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (PCHAR)&errMessage,
        0,
        NULL);
    if (errMessage)
    {
        sstream << ": " << std::endl;
        sstream << errMessage;
        LocalFree(errMessage);
    }
    else
    {
        sstream << std::endl;
    }

    return sstream.str();
}

////////////////////////////////////////////////////////////////////////////////

void Exception::exceptionTranslate( const Exception &e )
{
    boost::python::object pyExcept(e);

    PyErr_SetObject( diaExceptTypeObject, pyExcept.ptr() );
}

////////////////////////////////////////////////////////////////////////////////

std::list< Symbol > Symbol::findChildrenImpl(
    ULONG symTag,
    const std::string &name,
    DWORD nameCmpFlags
)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum SymTagEnum>(symTag),
            toWStr(name),
            nameCmpFlags,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    std::list< Symbol > childList;

    DiaSymbolPtr child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
        childList.push_back( Symbol(child, m_machineType) );

    return childList;
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG Symbol::getSize()
{
    return callSymbol(get_length);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::getName()
{
    autoBstr retValue( callSymbol(get_name) );
    return retValue.asStr();
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getType()
{
    return Symbol( callSymbol(get_type), m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getIndexType()
{
    return Symbol( callSymbol(get_arrayIndexType), m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getSymTag()
{
    return callSymbol(get_symTag);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRva()
{
    return callSymbol(get_relativeVirtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getLocType()
{
    return callSymbol(get_locationType);
}

////////////////////////////////////////////////////////////////////////////////

LONG Symbol::getOffset()
{
    return callSymbol(get_offset);
}

////////////////////////////////////////////////////////////////////////////////
void Symbol::getValueImpl(IDiaSymbol *_symbol, VARIANT &vtValue)
{
    HRESULT hres = _symbol->get_value(&vtValue);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::get_value", hres);
}

////////////////////////////////////////////////////////////////////////////////

python::object Symbol::getValue()
{
    VARIANT vtValue = { VT_EMPTY };
    getValueImpl(m_symbol, vtValue);
    switch (vtValue.vt)
    {
    case VT_I1:
    case VT_UI1:
        return python::object( static_cast<ULONG>(vtValue.bVal) );

    case VT_BOOL:
        return python::object( static_cast<bool>(!!vtValue.iVal) );

    case VT_I2:
    case VT_UI2:
        return python::object( static_cast<ULONG>(vtValue.iVal) );

    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        return python::object( vtValue.lVal );

    case VT_I8:
    case VT_UI8:
        return python::object( float(vtValue.llVal) );

    case VT_R4:
        return python::object( double(vtValue.fltVal) );

    case VT_R8:
        return python::object( vtValue.dblVal );

    case VT_BSTR:
        return python::object( autoBstr::asStr(vtValue.bstrVal).c_str() );

    }
    throw Exception("Unknown value type");
}

////////////////////////////////////////////////////////////////////////////////

bool Symbol::isBasicType()
{
    DWORD baseType = btNoType;
    return 
        SUCCEEDED( m_symbol->get_baseType(&baseType) ) && 
        (btNoType != baseType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getBaseType()
{
    return callSymbol(get_baseType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getBitPosition()
{
    return callSymbol(get_bitPosition);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getIndexId()
{
    return callSymbol(get_symIndexId);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getUdtKind()
{
    return callSymbol(get_udtKind);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getRegisterId()
{
    return callSymbol(get_registerId);
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getChildByName(const std::string &_name)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            toWStr(_name),
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    if (!count)
        throw Exception(_name + " not found");

    if (count != 1)
        throw Exception(_name + " is not unique");

    DiaSymbolPtr child;
    hres = symbols->Item(0, &child);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::Item", hres);

    return Symbol(child, m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

ULONG Symbol::getChildCount()
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    return count;
}

////////////////////////////////////////////////////////////////////////////////

Symbol Symbol::getChildByIndex(ULONG _index)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK != hres)
        throw Exception("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::get_Count", hres);

    if (LONG(_index) >= count)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        boost::python::throw_error_already_set();
    }

    DiaSymbolPtr child;
    hres = symbols->Item(_index, &child);
    if (S_OK != hres)
        throw Exception("Call IDiaEnumSymbols::Item", hres);

    return Symbol(child, m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::print()
{
    return printImpl(m_symbol, m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

GlobalScope::GlobalScope(
    __inout DiaDataSourcePtr &_scope,
    __inout DiaSessionPtr &_session,
    __inout DiaSymbolPtr &_globalScope
)   : Symbol(_globalScope, CV_CFL_80386)
    , m_source( _scope.Detach() )
    , m_session( _session.Detach() )
{
    m_symbol->get_machineType(&m_machineType);
}

////////////////////////////////////////////////////////////////////////////////

GlobalScope GlobalScope::loadPdb(const std::string &filePath)
{
    DiaDataSourcePtr _scope;

    HRESULT hres = 
        _scope.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);
    if ( S_OK != hres )
        throw Exception("Call ::CoCreateInstance", hres);

    hres = _scope->loadDataFromPdb( toWStr(filePath) );
    if ( S_OK != hres )
        throw Exception("Call IDiaDataSource::loadDataFromPdb", hres);

    DiaSessionPtr _session;
    hres = _scope->openSession(&_session);
    if ( S_OK != hres )
        throw Exception("Call IDiaDataSource::openSession", hres);

    DiaSymbolPtr _globalScope;
    hres = _session->get_globalScope(&_globalScope);
    if ( S_OK != hres )
        throw Exception("Call IDiaSymbol::get_globalScope", hres);

    return GlobalScope(_scope, _session, _globalScope);
}

////////////////////////////////////////////////////////////////////////////////

Symbol GlobalScope::findByRvaImpl(
    __in ULONG rva,
    __in ULONG symTag,
    __out LONG &displacement
)
{
    DiaSymbolPtr child;
    HRESULT hres = 
        m_session->findSymbolByRVAEx(
            rva,
            static_cast<enum SymTagEnum>(symTag),
            &child,
            &displacement);
    if (S_OK != hres)
        throw Exception("Call IDiaSession::findSymbolByRVAEx", hres);
    if (!child)
        throw Exception("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);

    return Symbol( child, m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

Symbol GlobalScope::getSymbolById(ULONG symId)
{
    DiaSymbolPtr _symbol;
    HRESULT hres = m_session->symbolById(symId, &_symbol);
    if (S_OK != hres)
        throw Exception("Call IDiaSession::findSymbolByRVAEx", hres);
    if (!_symbol)
        throw Exception("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);

    return Symbol( _symbol, m_machineType );
}

////////////////////////////////////////////////////////////////////////////////

}