#pragma once

#include "variant.h"
#include "symengine.h"
#include "typeinfo.h"
#include "typedvar.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class Module;
typedef boost::shared_ptr<Module>  ModulePtr;

///////////////////////////////////////////////////////////////////////////////////

class Module : public intBase {

public:

    static
    ModulePtr loadModuleByName( const std::string &name );

    static
    ModulePtr loadModuleByOffset( ULONG64 offset );

private:

    struct SymbolMapKey{
        std::string name;
        ULONG  size;
        ULONG  timeStamp;
        ULONG  checkSum;

        bool operator < ( const SymbolMapKey& key ) const
        {
            if ( name < key.name )
                return true;
            if ( name > key.name )
                return false;

            if ( size < key.size )
                return true;
            if ( size > key.size )
                return false;

            if ( timeStamp < key.timeStamp )
                return true;
            if ( timeStamp > key.timeStamp )
                return false;

            return checkSum < key.checkSum;
        }
    };

    typedef std::map<SymbolMapKey,SymbolSessionPtr>  SymbolSessionCache;
    static SymbolSessionCache m_symSessionCache;

    SymbolSessionPtr getFromCache();

public:

    Module(const std::string &name );

    Module(ULONG64 offset );

    std::string  getName() {
        return m_name;
    }

    ULONG64  getBase() const {
        return m_base;
    }

    ULONG64 getEnd() const {
        return m_base + m_size;
    }

    ULONG  getSize() const {
        return m_size;
    }

    std::string getSymFile() {
        prepareSymbolFile();
        //return m_symfile;
        if (m_symSession)
            return m_symSession->getSymbolFileName();
        return "";
    }

    std::string getImageName() const {
        return m_imageName;
    }

    void
    reloadSymbols();

    ULONG64
    getSymbolOffset( const std::string &symbolname ) {
        return m_base + getRvaByName(symbolname);
    }


    ULONG
    getSymbolRva( const std::string &symbolname ) {
        return getRvaByName(symbolname);
    }

    ULONG getCheckSum() const {
        return m_checkSum;
    }

    ULONG getTimeDataStamp() const {
        return m_timeDataStamp;
    }

    TypeInfoPtr getTypeByName( const std::string &typeName ) {
        return TypeInfo::getTypeInfo( boost::static_pointer_cast<Symbol>( getSymScope() ), typeName);
    }

    TypedVarPtr getTypedVarByAddr( ULONG64 addr );

    TypedVarPtr getTypedVarByName( const std::string &symName );

    TypedVarPtr getTypedVarByTypeName( const std::string &typeName, ULONG64 addr );

    python::list getTypedVarListByTypeName( ULONG64 listHeadAddres, const std::string  &typeName, const std::string &listEntryName );

    python::list getTypedVarArrayByTypeName( ULONG64 offset, const std::string  &typeName, ULONG number );

    TypedVarPtr containingRecordByName( ULONG64 offset, const std::string &typeName, const std::string &fieldName );

    ULONG64 getSymbolSize( const std::string &symName );

    SymbolPtr getSymbolByVa( ULONG64 offset, ULONG symTag, LONG* displacemnt = NULL );

    std::string getSymbolNameByVa( ULONG64 offset, bool showDisplacement = true );

    void getSymbolAndDispByVa( ULONG64 offset, std::string &symbolName, LONG &displacement);

    void getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement );

    std::string getSourceFile( ULONG64 offset );

    python::list enumSymbols( const std::string  &mask = "*" );

    python::list enumTypes( const std::string  &mask = std::string() );

    std::string print();

    std::string queryVersion( const std::string &value);

    python::tuple getVersion();

private:

    ULONG64 prepareVa(ULONG64 addr);

    SymbolPtr getSymScope();

    SymbolSessionPtr& getSymSession();

    BaseTypeVariant getValue() {
        return BaseTypeVariant(m_base);
    }

    ULONG getRvaByName(const std::string &symName);

    void prepareSymbolFile();

    std::string             m_name;
    std::string             m_imageName;
    ULONG64                 m_base;
    ULONG                   m_size;
    ULONG                   m_timeDataStamp;
    ULONG                   m_checkSum;

    SymbolSessionPtr        m_symSession;
};

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

