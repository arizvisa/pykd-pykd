
#include "stdafx.h"

#include "dbghelp.h"
#include "dia/diasymbol.h"
#include "win/utils.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

#define callSymbol(method) \
    callSymbolT( &IDiaSymbol::##method, #method)

//////////////////////////////////////////////////////////////////////////////////

std::string getBasicTypeName( ULONG basicType )
{
    for ( size_t i = 0; i < DiaSymbol::cntBasicTypeName; ++i )
    {
        if ( basicType == DiaSymbol::basicTypeName[i].first )
            return std::string( DiaSymbol::basicTypeName[i].second );
    }

    std::stringstream   sstr;

    sstr << "faild to find basic type with index %d" << basicType;

    throw DiaException( sstr.str() );
}

////////////////////////////////////////////////////////////////////////////////

DiaSymbol::DiaSymbol(__inout DiaSymbolPtr &_symbol, DWORD machineType )
    : m_symbol(_symbol), m_machineType(machineType)
{
}

//////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::fromGlobalScope( IDiaSymbol *_symbol )
{
    DWORD machineType;
    HRESULT hres = _symbol->get_machineType(&machineType);
    if (S_OK != hres)
        throw DiaException("IDiaSymbol::get_machineType", hres);
    if (!machineType)
        machineType = IMAGE_FILE_MACHINE_I386;

    return SymbolPtr( new DiaSymbol(DiaSymbolPtr(_symbol), machineType) );
}

//////////////////////////////////////////////////////////////////////////////////

SymbolPtrList  DiaSymbol::findChildren(
        ULONG symTag,
        const std::string &name,
        bool caseSensitive
    )
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres;

    if ( name.empty() )
    {
        hres = m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
                NULL,
                (caseSensitive ? nsCaseSensitive : nsCaseInsensitive) | nsfUndecoratedName | nsfRegularExpression,
                &symbols);

    }
    else
    {
        hres = m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
                toWStr(name),
                (caseSensitive ? nsCaseSensitive : nsCaseInsensitive) | nsfUndecoratedName | nsfRegularExpression,
                &symbols);
    }

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    SymbolPtrList childList;

    DiaSymbolPtr child;
    ULONG celt;
    while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
    {
        childList.push_back( SymbolPtr( new DiaSymbol(child, m_machineType) ) );
        child = NULL;
    }

    return childList;
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getBaseType()
{
    return callSymbol(get_baseType);
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getBitPosition()
{
    return callSymbol(get_bitPosition);
}

//////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getChildCount(ULONG symTag)
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
            NULL,
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    return count;
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByIndex(ULONG symTag, ULONG _index )
{
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            static_cast<enum ::SymTagEnum>(symTag),
            NULL,
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (LONG(_index) >= count)
    {
        throw PyException( PyExc_IndexError, "Index out of range");
    }

    DiaSymbolPtr child;
    hres = symbols->Item(_index, &child);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::Item", hres);

    return SymbolPtr( new DiaSymbol(child, m_machineType) );
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getChildByName(const std::string &name )
{
    // ���� ������ ����������
    DiaEnumSymbolsPtr symbols;
    HRESULT hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(name),
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    LONG count;
    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }

    // _���
    std::string underscoreName;
    underscoreName += '_';
    underscoreName += name;
    symbols = 0;

    hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(underscoreName),
            nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }
    
    // _���@�����
    std::string     pattern = "_";
    pattern += name;
    pattern += "@*";
    symbols = 0;

    hres = 
        m_symbol->findChildren(
            ::SymTagNull,
            toWStr(pattern),
            nsfRegularExpression | nsfCaseSensitive | nsfUndecoratedName,
            &symbols);

    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    hres = symbols->get_Count(&count);
    if (S_OK != hres)
        throw DiaException("Call IDiaEnumSymbols::get_Count", hres);

    if (count == 0)
         throw DiaException( name + " not found");

    if (count >0 )
    {
        DiaSymbolPtr child;
        hres = symbols->Item(0, &child);
        if (S_OK != hres)
            throw DiaException("Call IDiaEnumSymbols::Item", hres);

        return SymbolPtr( new DiaSymbol(child, m_machineType) );
    }
    
    throw DiaException(name + " is not found");
}

//////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getCount()
{
    return callSymbol(get_count);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getDataKind()
{
    return callSymbol(get_dataKind);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRegRealativeId()
{
    switch (m_machineType)
    {
    case IMAGE_FILE_MACHINE_AMD64:
        return getRegRealativeIdImpl(regToRegRelativeAmd64);
    case IMAGE_FILE_MACHINE_I386:
        return getRegRealativeIdImpl(regToRegRelativeI386);
    }
    throw DiaException("Unsupported machine type");
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRegRealativeIdImpl(const DiaRegToRegRelativeBase &regToRegRelative)
{
    DiaRegToRegRelativeBase::const_iterator it = 
        regToRegRelative.find(callSymbol(get_registerId));

    if (it == regToRegRelative.end())
        throw DiaException("Cannot convert DIA register ID to relative register ID");

    return it->second;
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getIndexId()
{
    return callSymbol(get_symIndexId);
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getIndexType()
{
    DiaSymbolPtr diaSymbol(callSymbol(get_arrayIndexType));
    return SymbolPtr( new DiaSymbol(diaSymbol, m_machineType) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getLocType()
{
    return callSymbol(get_locationType);
}

//////////////////////////////////////////////////////////////////////////////

static const  boost::regex  stdcallMatch("^_(\\w+)(@\\d+)?$");
static const  boost::regex  fastcallMatch("^@(\\w+)(@\\d+)?$");

std::string DiaSymbol::getName()
{
    HRESULT hres;
    BSTR bstrName = NULL;

    ULONG symTag;
    hres = m_symbol->get_symTag( &symTag );

    if ( FAILED( hres ) )
        throw DiaException("Call IDiaSymbol::get_symTag", hres);
      
    if( symTag == SymTagData || symTag == SymTagFunction || symTag == SymTagPublicSymbol )
    {
        hres = m_symbol->get_undecoratedNameEx( UNDNAME_NAME_ONLY, &bstrName);
        if ( FAILED( hres ) )
            throw DiaException("Call IDiaSymbol::get_undecoratedNameEx", hres);

        std::string  retStr = autoBstr( bstrName ).asStr();

        if ( !retStr.empty() )
        {
            boost::cmatch  matchResult;

            if ( boost::regex_match( retStr.c_str(), matchResult, stdcallMatch ) )
                return std::string( matchResult[1].first, matchResult[1].second );

            if ( boost::regex_match( retStr.c_str(), matchResult, fastcallMatch ) )
                return std::string( matchResult[1].first, matchResult[1].second );
    
            return retStr; 
        }
    }

    bstrName = callSymbol(get_name);

    return autoBstr( bstrName ).asStr();
}

///////////////////////////////////////////////////////////////////////////////

LONG DiaSymbol::getOffset()
{
    return callSymbol(get_offset);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getRva()
{
    return callSymbol(get_relativeVirtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG DiaSymbol::getSize()
{
    return callSymbol(get_length);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getSymTag()
{
    return callSymbol(get_symTag);
}

////////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSymbol::getType()
{
    DiaSymbolPtr diaSymbol(callSymbol(get_type));
    return SymbolPtr( new DiaSymbol( diaSymbol, m_machineType ) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getUdtKind()
{
    return callSymbol(get_udtKind);
}

////////////////////////////////////////////////////////////////////////////////

ULONGLONG DiaSymbol::getVa()
{
    return callSymbol(get_virtualAddress);
}

////////////////////////////////////////////////////////////////////////////////

void DiaSymbol::getValue( BaseTypeVariant &btv )
{
    CComVariant  vtValue;
    HRESULT hres = m_symbol->get_value(&vtValue);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::get_value", hres);

    switch (vtValue.vt)
    {
    case VT_I1:
        btv = (LONG)vtValue.bVal;
        break;

    case VT_UI1:
        btv = (ULONG)vtValue.bVal;
        break;

    case VT_BOOL:
        btv = !!vtValue.iVal;
        break;

    case VT_I2:
        btv = (LONG)vtValue.iVal;
        break;

    case VT_UI2:
        btv = (ULONG)vtValue.iVal;
        break;

    case VT_I4:
    case VT_INT:
        btv = (LONG)vtValue.lVal;
        break;

    case VT_UI4:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        btv = (ULONG)vtValue.lVal;
        break;

    case VT_I8:
        btv = (ULONG64)vtValue.llVal;
        break;

    case VT_UI8:
        btv = (LONG64)vtValue.llVal;
        break;

    //case VT_R4:
    //    btv = vtValue.fltVal;
    //    break;

    //case VT_R8:
    //    fillDataBuff(vtValue.dblVal);
    //    break;

    default:
        throw DbgException( "Unsupported const value" );
    }
}

//////////////////////////////////////////////////////////////////////////////

int DiaSymbol::getVirtualBasePointerOffset()
{
    return callSymbol(get_virtualBasePointerOffset);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getVirtualBaseDispIndex()
{
    return callSymbol(get_virtualBaseDispIndex);
}

////////////////////////////////////////////////////////////////////////////////

ULONG DiaSymbol::getVirtualBaseDispSize()
{
    DiaSymbolPtr diaSymbol(callSymbol(get_virtualBaseTableType));
    SymbolPtr baseTableType = SymbolPtr( new DiaSymbol( diaSymbol, m_machineType ) );

    return (ULONG)baseTableType->getType()->getSize();
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isBasicType()
{
    DWORD baseType = btNoType;
    return 
        SUCCEEDED( m_symbol->get_baseType(&baseType) ) && 
        (btNoType != baseType);
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isConstant()
{
    HRESULT  hres;
    BOOL  retBool = FALSE;

    hres = m_symbol->get_constType( &retBool );
    if ( FAILED( hres ) )
        throw DiaException("Call IDiaSymbol::get_constType", hres, m_symbol);

    return !!retBool;
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isIndirectVirtualBaseClass()
{
    return !!callSymbol(get_indirectVirtualBaseClass);
}

//////////////////////////////////////////////////////////////////////////////

bool DiaSymbol::isVirtualBaseClass()
{
    return !!callSymbol(get_virtualBaseClass);
}

//////////////////////////////////////////////////////////////////////////////

}; // pykd namespace end
