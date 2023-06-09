﻿#
#
#

import unittest
import target
import pykd
import re

class ModuleTest( unittest.TestCase ):

    def testCtor( self ):
        self.assertEqual( target.module.name(), pykd.module(target.module.begin() ).name() )
        self.assertEqual( target.module.name(), pykd.module(target.module.name() ).name() )

    #def testMiscellaneous( self ):
    #    self.assertFalse( target.module.unloaded() )
    #    self.assertTrue( target.module.um() )

    def testName( self ):
        self.assertEqual( target.moduleName, target.module.name() )

    def testSize( self ):
        self.assertNotEqual( 0, target.module.size() )
        self.assertTrue( pykd.isValid( target.module.begin() + target.module.size() - 1) )

    def testBegin( self ):
        self.assertNotEqual( 0, target.module.begin() )
        self.assertEqual( target.module.begin(), target.module )
        self.assertEqual( target.module.begin() + 100, target.module + 100 )

    def testEnd( self ):
        self.assertEqual( target.module.size(), target.module.end() - target.module.begin() )
        self.assertTrue( pykd.isValid( target.module.end() - 1) )

    def testPdb( self ):
       self.assertNotEqual( "", target.module.symfile() )

    def testImage( self ):
       self.assertEqual( target.module.name() + ".exe", target.module.image() )

    def testFindModule( self ):
        self.assertRaises( pykd.DbgException, pykd.module, target.module.begin() - 0x10 )

        self.assertNotEqual( None, pykd.module( target.module.begin() ) )
        self.assertNotEqual( None, pykd.module( target.module.begin() + 0x10) )

        self.assertRaises( pykd.DbgException, pykd.module, target.module.end() )
        self.assertRaises( pykd.DbgException, pykd.module, target.module.end() + 0x10 )

    def testSymbol( self ):
        self.assertEqual( target.module.rva("FastcallFunc"), target.module.offset("FastcallFunc") - target.module.begin() )
        self.assertEqual( target.module.rva("FastcallFunc"), target.module.FastcallFunc - target.module.begin() )
        self.assertEqual( target.module.rva("FastcallFunc"), pykd.getOffset( target.module.name() + "!FastcallFunc") - target.module.begin() )

    def testFindSymbol( self ):
        self.assertEqual( "LocalStaticFunc", target.module.findSymbol( target.module.offset("LocalStaticFunc") ) )
        self.assertEqual( "CdeclFunc", target.module.findSymbol( target.module.offset("CdeclFunc") ) )
        self.assertEqual( "StdcallFunc", target.module.findSymbol( target.module.offset("StdcallFunc") ) )
        self.assertEqual( "FastcallFunc", target.module.findSymbol( target.module.offset("FastcallFunc") ) )
        self.assertEqual( "_UnderscoreFunc", target.module.findSymbol( target.module.offset("_UnderscoreFunc") ) )
        self.assertEqual( "PureCStdcallFunc", target.module.findSymbol( target.module.offset("PureCStdcallFunc") ) )
        self.assertEqual( "PureCCdeclFunc", target.module.findSymbol( target.module.offset("PureCCdeclFunc") ) )
        self.assertEqual( "PureCFastcallFunc", target.module.findSymbol( target.module.offset("PureCFastcallFunc") ) )
        #self.assertEqual( "targetapp!FuncWithName0", pykd.findSymbol( target.module.offset("FuncWithName0") ) )
        #self.assertEqual( "targetapp!_FuncWithName2", pykd.findSymbol( target.module.offset("_FuncWithName2") ) )
        #self.assertEqual( "_FuncWithName2+10", target.module.findSymbol( target.module.offset("_FuncWithName2") + 0x10 ) )
        #self.assertEqual( "_FuncWithName2", target.module.findSymbol( target.module.offset("_FuncWithName2") + 0x10, showDisplacement = False ) )
        #self.assertEqual( "targetapp!_FuncWithName2+10", pykd.findSymbol( target.module.offset("_FuncWithName2") + 0x10 ) )
        #self.assertEqual( "targetapp!_FuncWithName2", pykd.findSymbol( target.module.offset("_FuncWithName2") + 0x10, showDisplacement = False ) )


    def testFindSymbolAndDisp( self ):
        #vaFuncWithName0 = target.module.offset("FuncWithName0")
        #self.assertEqual( ("FuncWithName0", 0), target.module.findSymbolAndDisp(vaFuncWithName0) )
        #self.assertEqual( ("FuncWithName0", 2), target.module.findSymbolAndDisp(vaFuncWithName0+2) )
        #self.assertEqual( ("targetapp!FuncWithName0", 0), pykd.findSymbolAndDisp(vaFuncWithName0) )
        #self.assertEqual( ("targetapp!FuncWithName0", 2), pykd.findSymbolAndDisp(vaFuncWithName0+2) )
        pass

    def testType( self ):
        self.assertEqual( "structTest", target.module.type("structTest").name() );
        self.assertEqual( "structTest", target.module.type("g_structTest").name() );

    def testSourceFile( self ):
        fileName = pykd.getSourceFile(target.module.CdeclFunc )
        self.assertTrue( re.search('testfunc\\.cpp', fileName ) )
        fileName, lineNo, displacement = pykd.getSourceLine( target.module.CdeclFunc + 2)
        self.assertEqual( 30, lineNo )
        self.assertTrue( re.search('testfunc\\.cpp', fileName ) )
        self.assertEqual( 2, displacement )
        #fileName, lineNo, displacement = pykd.getSourceLine()
        #self.assertEqual( 698, lineNo )

    def testEnumSymbols( self ):
        lst = target.module.enumSymbols()
        self.assertNotEqual( 0, len(lst) )
        lst = target.module.enumSymbols("hello*Str")
        self.assertEqual( 2, len(lst) )

        # 0:000> x targetapp!*Const
        # targetapp!ulonglongConst = 0xffff`ff000000
        # targetapp!boolConst = true
        # targetapp!ulongConst = 0x5555
        # targetapp!classChild::m_staticConst = <no type information>
        lst = target.module.enumSymbols( "*Const")
        self.assertEqual( 3, len(lst) )

        lst = target.module.enumSymbols( "*cal?Func")
        self.assertEqual( 4, len(lst) )
        lst = target.module.enumSymbols( "*virtMethod*") 
        self.assertNotEqual( 0, len(lst) )
        lst = target.module.enumSymbols( "classChild" )
        self.assertEqual( 0, len(lst) )

    #def testGetTypes( self ):
    #    lst1 = target.module.getUdts()
    #    self.assertNotEqual( 0, len(lst1) )
    #    self.assertTrue( "classChild" in lst1 )
    #    self.assertTrue( "classBase" in lst1 )
    #    self.assertTrue( "structTest" in lst1 )

    #    lst2 = target.module.getEnums()
    #    self.assertNotEqual( 0, len(lst2) )
    #    self.assertTrue( "enumType" in lst2 )

    def testCompare(self):
        self.assertTrue(target.module.begin() ==  target.module)
        self.assertTrue(target.module.begin() + 1 != target.module)
        self.assertTrue(target.module.begin() + 1 > target.module)
        self.assertTrue(target.module.begin() + 1 >= target.module)
        self.assertTrue(target.module.begin() - 1 <  target.module)
        self.assertTrue(target.module.begin() - 1 <= target.module)
        self.assertTrue( True if target.module else False )
        self.assertTrue( False if not target.module else True )
        self.assertFalse(target.module==None)
        self.assertTrue(target.module!=None)

    def testModuleList(self):
        self.assertTrue( [] != pykd.getModulesList() )

    def testContain(self):
        self.assertTrue("voidPtr" in target.module)
        self.assertTrue("structTest" in target.module)
        self.assertFalse("NotExist" in target.module)
        self.assertRaises(Exception, lambda md : 2 in md, target.module)

    def testGetByKey(self):
        self.assertFalse(None == target.module["voidPtr"])
        self.assertFalse(None == target.module["structTest"] )
        self.assertRaises(KeyError, lambda md : target.module["Not Exist"], target.module)

    def testEvalInModuleScope(self):
        self.assertEqual( target.module.rva('voidPtr'), eval("voidPtr - %#x" % target.module.begin(), globals(), target.module) )

    def testHasSymbol(self):
        self.assertTrue(target.module.hasSymbol("voidPtr"))
        self.assertTrue(target.module.hasSymbol("structTest"))
        self.assertFalse(target.module.hasSymbol("notExists"))

    def testPrint(self):
        modAsStr = str(target.module)

