/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include "twctype.h"

	
// -----------------------------------------------------------------------------
// Test methods
// -----------------------------------------------------------------------------
//                  
// -----------------------------------------------------------------------------
//Function Name :test_iswalpha
//API Tested :iswalpha
//TestCase Description:testing the behaviour for wide alphabets and also for
//  negative testcases like for control characters, punctuation characters,digits
//  and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswalpha()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalpha: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalpha: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalpha: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswalpha(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswalpha test failed!"));
			return KErrGeneral;
			}
    	}
    
	INFO_PRINTF1(_L("iswalpha test passed."));
    return KErrNone;
    }  


// -----------------------------------------------------------------------------
//Function Name :test_iswalnum
//API Tested :iswalnum
//TestCase Description:testing the behaviour for wide digits and wide alphabets
//  and also for negative testcases like for control characters, punctuation characters,
//  and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswalnum()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
       
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalnum: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalnum: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswalnum: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswalnum(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswalnum test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("iswalnum test passed."));
    return KErrNone;
    }  

// -----------------------------------------------------------------------------
//Function Name :test_iswdigit
//API Tested :iswdigit
//TestCase Description:testing the behaviour for wide digits and also for
//  negative testcases like for control characters, punctuation characters,
//  alphabets and the like.
// -----------------------------------------------------------------------------
    
TInt CTestWctype::test_iswdigit()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswdigit: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswdigit: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswdigit: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswdigit(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswdigit test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("iswdigit test passed."));
    return KErrNone;
    }  

// -----------------------------------------------------------------------------
//Function Name :test_iswcntrl
//API Tested :iswcntrl
//TestCase Description:testing the behaviour for wide control characters and also
//  for negative testcases like for alphabets, digits, punctuation characters,
//  and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswcntrl()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
		TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswcntrl: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswcntrl: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswcntrl: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswcntrl(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswcntrl test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("iswcntrl test passed."));
    return KErrNone;
    }      

// -----------------------------------------------------------------------------
//Function Name :test_iswxdigit
//API Tested :iswxdigit
//TestCase Description:testing the behaviour for wide digits, alphabets from
// 'A' to 'F' and 'a' to 'f' and also for negative testcases like for other alphabets,
// digits, control characters and punctuation characters and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswxdigit()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswxdigit: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswxdigit: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswxdigit: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswxdigit(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("test_iswxdigit test failed!"));
			return KErrGeneral;
			}
    	}
	INFO_PRINTF1(_L("test_iswxdigit passed."));
    return KErrNone;
 	}          
 


// -----------------------------------------------------------------------------
//Function Name :test_iswspace
//API Tested :iswspace
//TestCase Description:testing the behaviour for wide space, ideographic space and
//  other characters that introduce space and also for negative testcases like for 
//  alphabets,digits,control characters and punctuation characters and the like.
// -----------------------------------------------------------------------------
  
TInt CTestWctype::test_iswspace()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswspace: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswspace: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswspace: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswspace(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswspace test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("iswspace test passed."));
    return KErrNone;
    }          
    
 // -----------------------------------------------------------------------------
//Function Name :test_iswblank
//API Tested :iswblank
//TestCase Description:testing the behaviour for wide space, ideographic space and
//  tabulation and also for negative testcases like for alphabets,digits,control 
//  characters and punctuation characters and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswblank()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
		TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswblank(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswblank test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("iswblank test passed."));
    return KErrNone;
    }          
 
  // -----------------------------------------------------------------------------
//Function Name :test_iswpunct
//API Tested :iswpunct
//TestCase Description:testing the behaviour for punctuation characters
//  and also for negative testcases like for alphabets,digits,control 
//  characters and the like.
// -----------------------------------------------------------------------------
 
  TInt CTestWctype::test_iswpunct()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswblank: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
	   	result = iswpunct(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("iswalnum test failed!"));
			return KErrGeneral;
			}
    	}
	INFO_PRINTF1(_L("iswalnum test passed."));
    return KErrNone;
    }          

// -----------------------------------------------------------------------------
//Function Name :test_iswlower
//API Tested :iswlower
//TestCase Description:testing the behaviour for wide lower-case alphabets and
//  also for negative testcases like for upper-case alphabets,digits,control 
//  characters, space and punctuation characters and the like.
// -----------------------------------------------------------------------------

TInt CTestWctype::test_iswlower()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
		TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswlower: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswlower: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswlower: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswlower(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("test_iswlower test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("test_iswlower test passed."));
    return KErrNone;
    }          

// -----------------------------------------------------------------------------
//Function Name :test_iswupper
//API Tested :iswupper
//TestCase Description:testing the behaviour for wide upper-case alphabets and
//  also for negative testcases like for lower-case alphabets,digits,control 
//  characters, space and punctuation characters and the like.
// -----------------------------------------------------------------------------
 
TInt CTestWctype::test_iswupper()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
		TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswupper: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswupper: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswupper: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswupper(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("test_iswupper test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("test_iswupper test passed."));
    return KErrNone;
    }          
    
 // -----------------------------------------------------------------------------
//Function Name :test_iswprint
//API Tested :iswprint
//TestCase Description:testing the behaviour for all wide printable characters like
//  alphabets, digits, space and punctuation characters and also for negative testcases
//  like for control characters.
// -----------------------------------------------------------------------------
  
TInt CTestWctype::test_iswprint()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswprint: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswprint: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswprint: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswprint(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("test_iswprint test failed!"));
			return KErrGeneral;
			}
    	}
    INFO_PRINTF1(_L("test_iswprint test passed."));
    return KErrNone;
    }     



// -----------------------------------------------------------------------------
//Function Name :test_iswgraph
//API Tested :iswgraph
//TestCase Description:testing the behaviour for all visible wide characters like
//  alphabets, digits, and punctuation characters... and also for negative testcases
//  like for control characters and characters that introduce space.
// -----------------------------------------------------------------------------
           
 TInt CTestWctype::test_iswgraph()
    {
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result;
	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswgraph: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswgraph: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswgraph: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswgraph(i);
    	if ( result != expected )
			{
			ERR_PRINTF1(_L("test_iswgraph test failed!"));
			return KErrGeneral;
			}
    	}
    
 	
	INFO_PRINTF1(_L("test_iswgraph test passed."));
    return KErrNone;
    }          

// -----------------------------------------------------------------------------
//Function Name :test_iswctype
//API Tested :iswctype
//TestCase Description:testing the behaviour for all wide characters giving their 
//  respective categories as the value of the second argument (for alphabets, digits,
//  space and punctuation characters...etc) and also for negative testcases
//  like giving a category other than its actual category.
// -----------------------------------------------------------------------------
      
    
TInt CTestWctype::test_iswctype()
	{ 	
    struct charClass
		{
		char ClassName[15];
		int ClassValue;
		};

	struct charClass ValuesArray[]=
		{
		{"_CTYPE_A", _CTYPE_A},			//Alpha
		{"_CTYPE_C", _CTYPE_C},			//Control
		{"_CTYPE_D", _CTYPE_D},			//Digit
		{"_CTYPE_G", _CTYPE_G},			//Graph
		{"_CTYPE_L", _CTYPE_L},			//Lower
		{"_CTYPE_P", _CTYPE_P},			//Punct
		{"_CTYPE_S", _CTYPE_S},			//Space
		{"_CTYPE_U", _CTYPE_U},			//Upper
		{"_CTYPE_X", _CTYPE_X},			//X Digit
		{"_CTYPE_B", _CTYPE_B},			//Blank
		{"_CTYPE_R", _CTYPE_R},			//Print
		{"_CTYPE_I", _CTYPE_I},			//Ideogram
		{"_CTYPE_T", _CTYPE_T},			//Special
		{"_CTYPE_Q", _CTYPE_Q},			//Phonogram
		};
	
    TInt hex_val_start;
    TInt hex_val_end;
    TInt  expected, result, k, charClassValue=0;
    char charClassName[15];
    //TInt16 charClass;
   	TUint  i;
    // Read parameters
    _LIT(Kexpected, "Param%d");
    TBuf<8> pNameBuf;
    pNameBuf.Format(Kexpected,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, expected);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswctype: Failed to read integer parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_start, "Param%d");
    pNameBuf.Format(Khex_val_start,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_start);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswctype: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(Khex_val_end, "Param%d");
    pNameBuf.Format(Khex_val_end,++iParamCnt);
	res = GetHexFromConfig(ConfigSection(), pNameBuf, hex_val_end);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswctype: Failed to read hex parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
    
    _LIT(KcharClassName, "Param%d");
    pNameBuf.Format(KcharClassName,++iParamCnt);
    TPtrC string;
	res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
	if(!res)
		{
	 	_LIT(Kerr , "test_iswctype: Failed to read string parameter") ;
	 	INFO_PRINTF1(Kerr) ;
	 	}
	TBuf8<256> bufstring;
	bufstring.Copy(string);
    TInt paramLength=string.Length();
    char* text=(char *)(bufstring.Ptr());
    *(text+paramLength)='\0';
    strcpy(charClassName,text);
	
    for ( k=0 ; k < 14 ; k++)
    	{
    	if(!strcmp(charClassName,ValuesArray[k].ClassName))
    		{
    		charClassValue = ValuesArray[k].ClassValue;
    		}
    	}
    
    for ( i = hex_val_start; i <= hex_val_end; i++ )
    	{
    
    	result = iswctype(i,charClassValue);
    	if ( result != expected)
			{
			ERR_PRINTF1(_L("test_iswctype test failed!"));
			return KErrGeneral;
			}
    	}
    INFO_PRINTF1(_L("test_iswctype test passed."));
    return KErrNone;
    }          


