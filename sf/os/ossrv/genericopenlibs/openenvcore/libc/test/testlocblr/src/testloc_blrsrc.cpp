/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <Stiftestinterface.h>
#include <StifParser.h>
#include <e32math.h>
#include "libc_loc_blr.h"

#define SUPPORTED_LOCALES 104
#define SUPPORTED_CHARSETS 100
struct localeStrings
	{
	char localeName[50];
	TInt flag;		
	};

struct charconvStrings
	{
	char charsetName[30];
	};
struct localeStrings locales[SUPPORTED_LOCALES];
struct charconvStrings charsets[SUPPORTED_CHARSETS];


const TInt KMaxParamLength=100;
const TInt KZeroLength=0;
const TInt KMaxLength=99;


_LIT (KStartTag_testmodule, "<testmodule>");
_LIT (KEndTag_testmodule, "</testmodule>");
_LIT (KStartTag_group, "<group>");
_LIT (KEndTag_group, "</group>");
_LIT (KStartTag_case, "<case>");
_LIT (KEndTag_case, "</case>");
_LIT (KTag_ID, "<title/>");
_LIT (KTag_method, "<method/>");
_LIT (KTag_mode, "<mode/>");
_LIT (KValue_grouped, "grouped");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Clibc_loc_blr::Clibc_loc_blr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Clibc_loc_blr::Clibc_loc_blr()
    {
    
    }

// -----------------------------------------------------------------------------
// Clibc_loc_blr::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void Clibc_loc_blr::ConstructL()
    {

    // Create iLog
    iLog = CStifLogger::NewL( Klibc_loc_blrLogPath, 
                          Klibc_loc_blrLogFile);
    iRandomizerSeed = 1234567890;
    LocateSupportedDLLs();
    SupportedCharsets();
    }

// -----------------------------------------------------------------------------
// Clibc_loc_blr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Clibc_loc_blr* Clibc_loc_blr::NewL()
    {
    Clibc_loc_blr* self = new (ELeave) Clibc_loc_blr;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    
    }

// Destructor
Clibc_loc_blr::~Clibc_loc_blr()
    {
    TInt i;
   	 // Delete MRT specific arrays
    for (i=0;i<iCases;i++) delete iParser[i];
    iMethod.Close();
    iParser.Close();
    iCasesInGroup.Close();
    iFirstCaseInGroup.Close();
        
    // Delete iLog
    delete iLog;   
    }


// -----------------------------------------------------------------------------
// Clibc_loc_blr::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt Clibc_loc_blr::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// Clibc_loc_blr::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt Clibc_loc_blr::GetTestCasesL( 
    const TFileName& aConfig, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {
		// List available methods here, in the same order as in the script
		static TStifFunctionInfo const KFunctions[] =
		{      
	    // Copy this line for every implemented function.
	    // First string is the function name (test method ID) used in TestScripter script file.
	    // Second is the actual implementation member function. 
	    ENTRY( "SetLocale", Clibc_loc_blr::TestSetLocaleL),
	    ENTRY( "Nl_langinfo_Pass", Clibc_loc_blr::TestNl_Langinfo_PassL),
	    ENTRY( "Nl_langinfo_Fail", Clibc_loc_blr::TestNl_Langinfo_FailL),
	    ENTRY( "LocaleConv", Clibc_loc_blr::TestLocaleConvL),
	    ENTRY( "Iconv_Open_Pass", Clibc_loc_blr::TestIconv_Open_PassL),
	    ENTRY( "Iconv_Open_Fail", Clibc_loc_blr::TestIconv_Open_FailL),
	    ENTRY( "Iconv_Pass", Clibc_loc_blr::TestIconv_PassL),
	 };
        
	 TPtrC groupid, methodname, caseid, mode;
	 TName funcName;
	 TInt method, groupnumber, actualcase;
	       
	 // Reset the number of testcases and the method number
	 iCases = 0;

	 // Start parsing the script file
	 CStifParser* parser = NULL;
	 parser = CStifParser::NewL( KNullDesC, aConfig, CStifParser::ECStyleComments );
	    
	 CleanupStack::PushL(parser);
    
	 // Start parsing the common <testmodule> section
	 CStifSectionParser* testmodule = NULL;
	 testmodule = parser->SectionL(KStartTag_testmodule, KEndTag_testmodule);
	    
	 CleanupStack::PushL(testmodule);
	    
	 // Check parsing mode from the script (normal or "release")
	 testmodule->GetLine(KTag_mode, mode, ENoTag);
	  
	 if (mode == KValue_grouped)
		{
	    iGrouping = ETrue;
	    }
	 else
	    {
	    iGrouping = EFalse;
	  	}
	    
	 // Start parsing the first <group> section
	 CStifSectionParser* section = NULL;
	 section = testmodule->SubSectionL(KStartTag_group, KEndTag_group);
	    
	 groupnumber = 0;
	 actualcase = 0;
	    
     // As long as there are <group> sections in the script, read them
	 while ( section )
    	{
	    CleanupStack::PushL( section );
	    	
	    if ( iGrouping )
	    	{
	    	// Add number of cases and first case in method variable to arrays
	    	iFirstCaseInGroup.Append(actualcase);
	    	iCasesInGroup.Append(0);
	    	}
    	
    	// Get group ID from the script (given to STIF instead of case ID in grouping mode)
	    section->GetLine(KTag_ID, groupid, ENoTag);
	    	
    	// Get method name from the script
	    section->GetNextLine(KTag_method, methodname, ENoTag);
    	
    	// Retrieve method number corresponding to method name
    	method = 0;
		for (TUint i=0;i<(sizeof(KFunctions)/sizeof(TStifFunctionInfo));i++)
			{
			funcName = KFunctions[i].iFunctionName;
		    if (funcName == methodname)
				{
			   	method=i;
			   	}
			}
			
	    // Start parsing the first case subsection
	    CStifSectionParser* subsection = NULL;
	    subsection = section->SubSectionL(KStartTag_case, KEndTag_case);
    	
	    // As long as there are cases for the method, read them
	    while ( subsection )
    		{
    		CleanupStack::PushL(subsection);
    	        
    		// Get testcase ID from the script
		    subsection->GetLine(KTag_ID, caseid, ENoTag);   	    
		    		
    		// If in "release" mode
			if ( iGrouping )
				{
				// Increase number of cases for the method
				if ( groupnumber < iCasesInGroup.Count() )
					{
				 	iCasesInGroup[groupnumber]++;
				  	}
					
				}
				else
					{
					// Add number of cases and first case in method variable to arrays
	    			iFirstCaseInGroup.Append(actualcase);
	    			iCasesInGroup.Append(1);
						
					// Start reading testcase info
		    		TTestCaseInfo* tc = NULL;
		    		tc = new (ELeave) TTestCaseInfo();
	    	    
		    		CleanupStack::PushL(tc);
		    		
					// Save testcase data to TTestCaseInfo structure
	    			tc->iTitle.Copy(caseid);
					tc->iCaseNumber = ++iCases;
					
					// Add TTestCaseinfo structure to testcase array
					aTestCases.Append(tc);
					
					CleanupStack::Pop(tc);
				
					groupnumber++;
										
					}
				
				// Add method address to testcase method address list
				iMethod.Append(KFunctions[method].iMethod);
  				// Add subsection parser to testcase section parser list
				iParser.Append(subsection);
			  
				// Get new method name from the script, if found
	    		if (section->GetNextLine(KTag_method, methodname, ENoTag) != KErrNotFound)   	
	    			{
	   	  				// Retrieve method number corresponding to method name
		    			method = 0;
						for (TUint i=0;i<(sizeof(KFunctions)/sizeof(TStifFunctionInfo));i++)
							{
							funcName = KFunctions[i].iFunctionName;
							if (funcName == methodname)
					    		{
					    		method=i;
								}
							}
					}
				
				// Increment the current case number
				actualcase++;
				
				// Move on to next case subsection, but leave subsection in memory for the test method to parse
				CleanupStack::Pop(subsection);
	    		subsection = section->NextSubSectionL(KStartTag_case, KEndTag_case);    
	    	
	    		}
	    	
	    // If in grouping mode
		if ( iGrouping )
			{
			  // Start creating testcase info
	    	TTestCaseInfo* tc = NULL;
	    	tc = new (ELeave) TTestCaseInfo();
	    	    
    		CleanupStack::PushL(tc);
	    	    
				// Save testcase data to TTestCaseInfo structure
    	    tc->iTitle.Copy(groupid);
			tc->iCaseNumber = ++iCases;
				
			// Add TTestCaseinfo structure to testcase array
			aTestCases.Append(tc);
			
			CleanupStack::Pop(tc);
				
			groupnumber++;
				
			}

		// Check parsing mode from the script ("normal" or "group")
	  	testmodule->GetNextLine(KTag_mode, mode, ENoTag);
	  
	  	if (mode == KValue_grouped)
	  		{
	    	iGrouping = ETrue;
	    	}
	  	else
		    {
		    iGrouping = EFalse;
	  		}
	  				
	    // Move on to next <group> section
	    CleanupStack::PopAndDestroy (section);
	    section = testmodule->NextSubSectionL(KStartTag_group, KEndTag_group);
	    
    	}
    
    	CleanupStack::PopAndDestroy (testmodule);
    	
    	CleanupStack::PopAndDestroy (parser);
        
    	return KErrNone;
    	
	}
    
    
// -----------------------------------------------------------------------------
// Clibc_loc_blr::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt Clibc_loc_blr::RunTestCaseL(
    const TInt aCaseNumber,
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {
    
    _LIT(KStepsPassed, "All cases in the group passed.");
    _LIT(KStepsFailed, "Some of the grouped cases failed!");
    
    TInt groupnumber = aCaseNumber-1; // The internal group number is the same as the STIF case number, decremented by 1 (in STIF the first case is number 1, here first group is number 0)
    
    // Run the actual method(s), giving section parser as argument
     
    if ( groupnumber < iCasesInGroup.Count() )
    	{
    	if ( iCasesInGroup[groupnumber] == 1 )
    	  	// Groups (<group> sections) with only one case (grouping mode) and <case> sections run separately (normal mode)
	      	{
	      	if (((groupnumber)<iMethod.Count())&&((groupnumber)<iParser.Count()))
	        	{
	        	// Run the actual method
	         	( this->*(iMethod[iFirstCaseInGroup[groupnumber]]) )( iParser[iFirstCaseInGroup[groupnumber]], aResult );
	         	}
	        } 
	    else 
			// Groups (<group> sections) with more than one case (<case> section, grouping mode)
	        {
	        TInt result = KErrNone;
	        for (TInt casetorun=iFirstCaseInGroup[groupnumber];casetorun<iFirstCaseInGroup[groupnumber]+iCasesInGroup[groupnumber];casetorun++)
	        	{
	        	if ((casetorun<iMethod.Count())&&(casetorun<iParser.Count()))
	        		{
	        		// Run the actual method
	        		( this->*(iMethod[casetorun]) )( iParser[casetorun], aResult ); 
	        			
	        		if (aResult.iResult != KErrNone)
	        			{
	        			result = aResult.iResult;
	        			}
	        		}
	        	else 
	        		{
	        		result = KErrNotFound;
	        		}
	        	}
	        	if (result == KErrNone)
	        		{
	        		aResult.SetResult(result, KStepsPassed);
	        		}
	        	else
	        		{
	        		aResult.SetResult(result, KStepsFailed);
	        		}
	        }
	    }
        
    // Test case was executed
    return KErrNone;
    
    }
    
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return Clibc_loc_blr::NewL();
    
    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// Utility functions for testing.
// -----------------------------------------------------------------------------

void Clibc_loc_blr::ReadIntParam(CStifSectionParser* aSection, TInt &aInt)
	{
	TPtrC descriptor;
	
	aSection->GetNextLine(descriptor);
	
	TLex lex(descriptor);
	lex.Val(aInt);
	
	}
	
void Clibc_loc_blr::ReadStringParam(CStifSectionParser* aSection, char* aString)
	{
	TPtrC descriptor;
	TInt i;
	
	aSection -> GetNextLine(descriptor);
	
	if ( descriptor == _L("\"\"") )
	  {
	  i = 0;
	  }
	else
		{
		// If the string is quoted, take only the insides
		if ((descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
	    {
	    for ( i=0; i<descriptor.Length()-2; i++ )
    	  {
        aString[i]=descriptor[i+1];
        }
      }
    // Otherwise, take the whole string
    else
      {
	    for ( i=0; i<descriptor.Length(); i++ )
    	  {
        aString[i]=descriptor[i];
        }
      }
    }
    
	aString[i]=0;
    
	}
	
TBool Clibc_loc_blr::SameString(char *aString1, char *aString2)
	{
	TInt i;
	
	for ( i=0; ( (aString1[i]!=0) && (aString2[i]!=0) && (aString2[i] == aString1[i]) ); i++)	{} // Go through the strings
	
	if ( aString2[i] != aString1[i] )	// If the scanning ended because of a difference between the strings
		{
		return EFalse;
		}
		
	else	// else
		{
		return ETrue;
		}
	
	}
	
void Clibc_loc_blr::StringToDes(char *aString, TDes &aDescriptor)
	{
	aDescriptor.Zero();
	
	for ( TInt i=0; aString[i]!=0; i++ )
		{
		aDescriptor.Append(aString[i]);
		}
		
	}
	
void Clibc_loc_blr::RandomizeString(char *aString, TInt aLength)
	{
	TInt16 integer;
	TInt i;
  
	for (i=0; i<aLength; i++)
    {
    Math::Int(integer, Math::FRand(iRandomizerSeed)*255);
    aString[i] = integer;
    }
    
	aString[i] = 0;
  
	}
	
TInt Clibc_loc_blr::RandomInt(TInt aMaxValue)
	{
	TReal64 maxValue = aMaxValue;
	
	TInt32 result;
	
	Math::Int (result, Math::FRand(iRandomizerSeed)*maxValue);
	
	return result;
	
	}
	
// -----------------------------------------------------------------------------
// Test methods
// -----------------------------------------------------------------------------
//                  

TInt  Clibc_loc_blr::TestSetLocaleL( CStifSectionParser* aSection, TTestResult& aResult )
    {
    
  	// Create temporary variables in stack
  	TInt retVal = KErrGeneral;
  	char localeName[KMaxParamLength];
  	char categoryName[KMaxParamLength];
  	char exptOutput[KMaxParamLength];
  	
  	//char* input2;
  	char* output = NULL;
  	int	category;
  	// Read parameters
  	ReadStringParam(aSection, categoryName);
    ReadStringParam (aSection, localeName);
    ReadStringParam (aSection, exptOutput);
    
    iLog->Log(_L("\r\n"));
    iLog->Log(_L8("Testing %s  for %s category"), localeName,categoryName);
    
    if(SameString("LC_ALL",categoryName))
		{
    	category = LC_ALL;
		}
    if(SameString("LC_CTYPE",categoryName))
		{
    	category = LC_CTYPE;
		}
	if(SameString("LC_COLLATE",categoryName))
		{
		category = LC_COLLATE;
		}
	if(SameString("LC_MONETARY",categoryName))
		{
		category = LC_MONETARY;		    	
		}
	if(SameString("LC_NUMERIC",categoryName))
		{
		category = LC_NUMERIC;    	
		}
	if(SameString("LC_TIME",categoryName))
		{
		category = LC_TIME;    	    	
		}
    // Do some testing
    
    
    TInt j = 0;
	TInt flag = 0;
    while(j < SUPPORTED_LOCALES)
		{
    	if(SameString(locales[j].localeName,localeName) && locales[j].flag)
    		{
    		flag = 1;
			break;	    						
    		}
    	j++;
		}
    
    
	if(SameString("NULL",localeName))
		{
		output=setlocale(category,NULL);	
		}
	
    else if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
		{
    	output=setlocale(category,localeName);
		}
    else
		{
		if(flag)
    		{
    		output=setlocale(category,localeName);	
			}
    	else
			{
	    	aResult.SetResult(KErrNone, _L("Test passed."));
			iLog->Log(_L8("%s Locale not Supported :: Test passed.\t\t"), localeName);
			return KErrNone;
			}
		}
    
	if(output!=NULL && SameString(output,exptOutput))
    	{
    	aResult.SetResult(KErrNone, _L("Test passed."));
    	iLog->Log(_L("Test passed.\t\t"));
    	retVal = KErrNone;
    	}
			
	else if(output==NULL &&  SameString("NULL",exptOutput))
		{
		aResult.SetResult(KErrNone, _L("Test passed."));
		iLog->Log(_L("Test passed.\t\t"));
    	retVal = KErrNone;
		}
			
	else
		{
		aResult.SetResult(KErrGeneral, _L("Test failed."));
		iLog->Log(_L("Test failed.\t\t"));
    	iLog->Log(_L8("API Output = NULL\t Expected Output = %s\n"), exptOutput);
		retVal = KErrGeneral;
		}	
    
    // Set test result
    return retVal;
   
    
	} 


TInt  Clibc_loc_blr::TestNl_Langinfo_PassL( CStifSectionParser* aSection, TTestResult& aResult )
    {
    
	TInt ret = KErrGeneral;
	// Create temporary variables in stack
	char localeName[KMaxParamLength];
	char* locale = NULL;
		  
	// Read parameters
	ReadStringParam (aSection, localeName);
	iLog->Log(_L("\r\n"));
	iLog->Log(_L8("Testing nl_langinfo() API for %s \t\t"), localeName);	    
	    
	TInt j = 0;
	TInt flag = 0;
	while(j < SUPPORTED_LOCALES)
		{
	    if(SameString(locales[j].localeName,localeName) && locales[j].flag)
			{
	    	flag = 1;
			break;	    						
	    	}
	    j++;
	    }
	    
	if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
		{
		locale=setlocale(LC_ALL,localeName);
		}
	
    else
		{
       	j = 0;
    	TInt charsetFlag = 0;
    	char* srcCharset =  strchr(localeName, '.');
    	srcCharset++;
    	while(j < SUPPORTED_CHARSETS)
    		{
	    	if(SameString(charsets[j].charsetName,srcCharset))
	    		{
	    		charsetFlag = 1;
	    		break;
	    		}
	    	j++;
    		}
    	if(flag && charsetFlag)
    		{
    		locale=setlocale(LC_ALL,localeName);	
    		}
    	else
			{
	    	aResult.SetResult(KErrNone, _L("Test passed."));
	    	if(!flag)
				{
				iLog->Log(_L8("%s Locale is not Supported :: Test passed.\t\t"), localeName);
				}
	    	if(!charsetFlag)
				{
				iLog->Log(_L8("%s Charset is not Supported :: Test passed.\t\t"), srcCharset);	    	
				}
			return KErrNone;
			}
		}
    	
    if(locale)
		{
    	
    	TInt retVal = Langinfo(aSection);
    	if(KErrNone == retVal)
			{
	    	aResult.SetResult(KErrNone, _L("Test passed."));
	    	iLog->Log(_L("Test passed.\t\t"));
	    	ret = KErrNone;
			}
	    else
			{
	    	aResult.SetResult(KErrGeneral, _L("Test failed."));	
	    	ret = KErrGeneral;
			}
	    
		}
    else
		{
    	aResult.SetResult(KErrGeneral, _L("Test failed."));
    	iLog->Log(_L("Locale setting failed \t\t"));	    
    	ret = KErrGeneral;
		}

    // Set test result
    return ret;
   
    }  

TInt  Clibc_loc_blr::TestNl_Langinfo_FailL( CStifSectionParser* aSection, TTestResult& aResult )
    {
    
	// Create temporary variables in stack
	char localeName[KMaxParamLength];
	TInt ret = KErrGeneral;
	  
	// Read parameters
    ReadStringParam (aSection, localeName);
    
	char* locale = setlocale(LC_ALL,localeName);

    if(locale)
		{
    	TInt retVal = Langinfo(aSection);
    	if(KErrNone != retVal)
			{
	    	aResult.SetResult(KErrNone, _L("Test passed."));
	    	ret = KErrNone;
			}
	    else
			{
	    	aResult.SetResult(KErrGeneral, _L("Test failed."));	
	    	ret = KErrGeneral;
			}
	    
		}
    else
		{
    	aResult.SetResult(KErrGeneral, _L("Test failed."));
    	ret = KErrGeneral;
		}
   
    
    // Set test result
    return ret;
   
    }  
    
TInt Clibc_loc_blr::TestLocaleConvL( CStifSectionParser* aSection, TTestResult& aResult)
	{
	TInt ret = KErrGeneral;
	char localeName[KMaxParamLength];
	
	char decimal[KMaxParamLength];
	char decimal_point[KMaxParamLength];
	
	char thousand[KMaxParamLength];
	char thousands_sep[KMaxParamLength];
	
	char group[KMaxParamLength];
	int grouping;
	
	char int_curr[KMaxParamLength];
	char int_curr_symbol[KMaxParamLength];
	
	char currency[KMaxParamLength];
	char currency_symbol[KMaxParamLength];
	
	char mon_decimal[KMaxParamLength];
	char mon_decimal_point[KMaxParamLength];
	
	char mon_thousand[KMaxParamLength];
	char mon_thousands_sep[KMaxParamLength];
	
	char mon_group[KMaxParamLength];
	int mon_grouping;
	
	char pos[KMaxParamLength];
	char positive_sign[KMaxParamLength];
	
	char neg[KMaxParamLength];
	char negative_sign[KMaxParamLength];
	
	char int_fraction[KMaxParamLength];
	int int_frac_digits;
	
	char fraction[KMaxParamLength];
	int frac_digits;
	
	char p_precedes[KMaxParamLength];
	int p_cs_precedes;
	
	char p_sapce[KMaxParamLength];
	int p_sep_by_space;
	
	char p_sign[KMaxParamLength];
	int p_sign_posn;
	
	char n_sign[KMaxParamLength];
	int n_sign_posn;
	
	char int_p_precedes[KMaxParamLength];
	int int_p_cs_precedes;
	
	char int_p_sapce[KMaxParamLength];
	int int_p_sep_by_space;

	char int_p_sign[KMaxParamLength];
	int int_p_sign_posn;
	
	char int_n_sign[KMaxParamLength];
	int int_n_sign_posn;

	  // Read parameters
    ReadStringParam (aSection, localeName);

    ReadStringParam (aSection, decimal);
    ReadStringParam (aSection, decimal_point);
    
    ReadStringParam (aSection, thousand);
    ReadStringParam (aSection, thousands_sep);
    
    ReadStringParam (aSection, group);
    ReadIntParam(aSection,grouping );
    
    ReadStringParam (aSection, int_curr);
    ReadStringParam (aSection, int_curr_symbol);
    
    ReadStringParam (aSection, currency);
    ReadStringParam (aSection, currency_symbol);
    
    ReadStringParam (aSection, mon_decimal);
    ReadStringParam (aSection, mon_decimal_point);
    
    ReadStringParam (aSection, mon_thousand);
    ReadStringParam (aSection, mon_thousands_sep);
    
    ReadStringParam (aSection, mon_group);
    ReadIntParam(aSection,mon_grouping );
    
    ReadStringParam (aSection, pos);
    ReadStringParam (aSection, positive_sign);
    
    ReadStringParam (aSection, neg);
    ReadStringParam (aSection, negative_sign);
    
    ReadStringParam (aSection, int_fraction);
    ReadIntParam(aSection,int_frac_digits );
    
    ReadStringParam (aSection, fraction);
    ReadIntParam(aSection,frac_digits );
    
    ReadStringParam (aSection, p_precedes);
    ReadIntParam(aSection,p_cs_precedes );
    
    ReadStringParam (aSection, p_sapce);
    ReadIntParam(aSection,p_sep_by_space );
    
    ReadStringParam (aSection, p_sign);
    ReadIntParam(aSection,p_sign_posn );
    
    ReadStringParam (aSection, n_sign);
    ReadIntParam(aSection,n_sign_posn );
    
    ReadStringParam (aSection, int_p_precedes);
    ReadIntParam(aSection,int_p_cs_precedes );
    
    ReadStringParam (aSection, int_p_sapce);
    ReadIntParam(aSection,int_p_sep_by_space );
    
    ReadStringParam (aSection, int_p_sign);
    ReadIntParam(aSection,int_p_sign_posn );
    
    ReadStringParam (aSection, int_n_sign);
    ReadIntParam(aSection,int_n_sign_posn );
    
  	iLog->Log(_L("\r\n"));
  	iLog->Log(_L8("Testing localeconv for  %s \t\t"), localeName);	    
	char* locale = NULL;
  	TInt j = 0;
	TInt flag = 0;
    while(j < SUPPORTED_LOCALES)
		{
    	if(SameString(locales[j].localeName,localeName) && locales[j].flag)
    		{
    		flag = 1;
			break;	    						
    		}
    	j++;
		}

       
	if(SameString("C",localeName) || SameString("POSIX",localeName) || SameString("",localeName) )
		{
		locale = setlocale(LC_ALL,localeName);
		}
	
    else
		{
    	j = 0;
		TInt charsetFlag = 0;
		char* srcCharset =  strchr(localeName, '.');
		srcCharset++;
		while(j < SUPPORTED_CHARSETS)
			{
	    	if(SameString(charsets[j].charsetName,srcCharset))
	    		{
	    		charsetFlag = 1;
	    		break;
	    		}
	    	j++;
			}
	
    	if(flag && charsetFlag)
    		{
    		locale = setlocale(LC_ALL,localeName);	
    		}
    	else
			{
	    	aResult.SetResult(KErrNone, _L("Test passed."));
			if(!flag)
			iLog->Log(_L8("%s Locale is not Supported :: Test passed.\t\t"), localeName);
	    	if(!charsetFlag)
			iLog->Log(_L8("%s Charset is not Supported :: Test passed.\t\t"), srcCharset);	    	
			return KErrNone;
			}
		}
  	
  	if(locale)
		{
    	struct lconv*  result = localeconv();
    	if(result)
    		{
    		TInt retVal = 1;
	    	if(!SameString(decimal_point,result->decimal_point))
	    		{
	    		iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual decimal_point =%s\t API returned decimal_point is  = %s\n"), decimal_point,result->decimal_point);
	    		retVal = 0;
	    		}
	    	if(!SameString(thousands_sep, result->thousands_sep))
	    		{	
	    		iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual thousands_sep =%s\t API returned thousands_sep is  = %s\n"), thousands_sep,result->thousands_sep);
	    		retVal = 0;
	    		}
			if(!SameString(int_curr_symbol, result->int_curr_symbol))
				{
				iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual int_curr_symbol =%s\t API returned int_curr_symbol is  = %s\n"), int_curr_symbol,result->int_curr_symbol);
				retVal = 0;
				}
			char temp[100];
    		ConvertHexTochar(temp, currency_symbol);
			if(!SameString(temp, result->currency_symbol))
				{
				iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual currency_symbol =%s\t API returned currency_symbol is  = %s\n"), temp,result->currency_symbol);
				retVal = 0;
				}
			if(!SameString(mon_decimal_point, result->mon_decimal_point))
				{
				iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual mon_decimal_point =%s\t API returned mon_decimal_point is  = %s\n"), mon_decimal_point,result->mon_decimal_point);
				retVal = 0;
				}
			if(!SameString(mon_thousands_sep, result->mon_thousands_sep))
				{
				iLog->Log(_L("Test Failed.\t\t"));
    			iLog->Log(_L8("Actual mon_thousands_sep =%s\t API returned mon_thousands_sep is  = %s\n"), mon_thousands_sep,result->mon_thousands_sep);
				retVal = 0;
				}
			if(!SameString(positive_sign, result->positive_sign))
				{
				retVal = 0;
				}
			if(!SameString(negative_sign, result->negative_sign))
				{
				retVal = 0;
				}
			
			
			if(grouping !=  ((int) result->grouping[0] - 48))
				{
				retVal = 0;	
				}
			if(mon_grouping != ((int) result->mon_grouping[0] - 48))
				{
				retVal = 0;	
				}
			if((char) int_frac_digits != result->int_frac_digits)
				{
				retVal = 0;
				}
			if((char) frac_digits != result->frac_digits)
				{
				retVal = 0;
				}
			if((char) p_cs_precedes != result->p_cs_precedes)
				{
				retVal = 0;
				}
			if((char) p_sep_by_space != result->p_sep_by_space)
				{
				retVal = 0;
				}
			if((char) p_sign_posn != result->p_sign_posn)
				{
				retVal = 0;
				}
			if((char) n_sign_posn != result->n_sign_posn)
				{
				retVal = 0;
				}
			if((char) int_p_cs_precedes != result->int_p_cs_precedes)
				{
				retVal = 0;
				}
			if((char) int_p_sep_by_space != result->int_p_sep_by_space)
				{
				retVal = 0;
				}
			if((char) int_p_sign_posn != result->int_p_sign_posn)
				{
				retVal = 0;
				}
			if((char) int_n_sign_posn != result->int_n_sign_posn)
				{
				retVal = 0;
				}
				        
	     	if(retVal)	
	    		{
	    		aResult.SetResult(KErrNone, _L("Test passed."));
	    		iLog->Log(_L("Test passed.\t\t"));
	    		ret = KErrNone;
	    		}
	    	else
	    		{
	    		aResult.SetResult(KErrGeneral, _L("Test failed."));
	    		ret = KErrGeneral;
	    		}	
    		}
    	
	    else
			{
	    	aResult.SetResult(KErrGeneral, _L("Test failed."));
	    	ret = KErrGeneral;
			}
		}
    else
		{
    	aResult.SetResult(KErrGeneral, _L("Test failed."));
    	iLog->Log(_L8("%s  setting failed\t\t"), localeName);	    
    	ret = KErrGeneral;
		}
    
    return ret;
    
	}

TInt Clibc_loc_blr::Langinfo(CStifSectionParser* aSection)
	{
	TInt retVal = KErrGeneral;
	char codeset[10];
	char codesetExpected[15];
	ReadStringParam (aSection, codeset);
    ReadStringParam(aSection,codesetExpected);
    retVal = ReadLocaleInformation(codeset, codesetExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abday2[8];
	char abday2Expected[40];
	ReadStringParam (aSection, abday2);
    ReadStringParam(aSection,abday2Expected);
    retVal = ReadLocaleInformation(abday2, abday2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abday3[8];
	char abday3Expected[40];
	ReadStringParam (aSection, abday3);
    ReadStringParam(aSection,abday3Expected);
    retVal = ReadLocaleInformation(abday3, abday3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abday4[8];
	char abday4Expected[40];
	ReadStringParam (aSection, abday4);
    ReadStringParam(aSection,abday4Expected);
    retVal = ReadLocaleInformation(abday4, abday4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
	
	char abday5[8];
	char abday5Expected[40];
	ReadStringParam (aSection, abday5);
    ReadStringParam(aSection,abday5Expected);
    retVal = ReadLocaleInformation(abday5, abday5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}	
    
	char abday6[8];
	char abday6Expected[40];
	ReadStringParam (aSection, abday6);
    ReadStringParam(aSection,abday6Expected);
    retVal = ReadLocaleInformation(abday6, abday6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abday7[8];
	char abday7Expected[40];
	ReadStringParam (aSection, abday7);
    ReadStringParam(aSection,abday7Expected);
    retVal = ReadLocaleInformation(abday7, abday7Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abday1[8];
	char abday1Expected[40];
	ReadStringParam (aSection, abday1);
    ReadStringParam(aSection,abday1Expected);
    retVal = ReadLocaleInformation(abday1, abday1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day2[6];
	char day2Expected[100];
	ReadStringParam (aSection, day2);
    ReadStringParam(aSection,day2Expected);
    retVal = ReadLocaleInformation(day2, day2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day3[6];
	char day3Expected[100];
	ReadStringParam (aSection, day3);
    ReadStringParam(aSection,day3Expected);
    retVal = ReadLocaleInformation(day3, day3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day4[6];
	char day4Expected[100];
	ReadStringParam (aSection, day4);
    ReadStringParam(aSection,day4Expected);
    retVal = ReadLocaleInformation(day4, day4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day5[6];
	char day5Expected[100];
	ReadStringParam (aSection, day5);
    ReadStringParam(aSection,day5Expected);
    retVal = ReadLocaleInformation(day5, day5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day6[6];
	char day6Expected[100];
	ReadStringParam (aSection, day6);
    ReadStringParam(aSection,day6Expected);
    retVal = ReadLocaleInformation(day6, day6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day7[6];
	char day7Expected[100];
	ReadStringParam (aSection, day7);
    ReadStringParam(aSection,day7Expected);
    retVal = ReadLocaleInformation(day7, day7Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char day1[6];
	char day1Expected[100];
	ReadStringParam (aSection, day1);
    ReadStringParam(aSection,day1Expected);
    retVal = ReadLocaleInformation(day1, day1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth1[8];
	char abmonth1Expected[100];
	ReadStringParam (aSection, abmonth1);
    ReadStringParam(aSection,abmonth1Expected);
    retVal = ReadLocaleInformation(abmonth1, abmonth1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth2[8];
	char abmonth2Expected[100];
	ReadStringParam (aSection, abmonth2);
    ReadStringParam(aSection,abmonth2Expected);
    retVal = ReadLocaleInformation(abmonth2, abmonth2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth3[8];
	char abmonth3Expected[100];
	ReadStringParam (aSection, abmonth3);
    ReadStringParam(aSection,abmonth3Expected);
    retVal = ReadLocaleInformation(abmonth3, abmonth3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth4[8];
	char abmonth4Expected[100];
	ReadStringParam (aSection, abmonth4);
    ReadStringParam(aSection,abmonth4Expected);
    retVal = ReadLocaleInformation(abmonth4, abmonth4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth5[8];
	char abmonth5Expected[100];
	ReadStringParam (aSection, abmonth5);
    ReadStringParam(aSection,abmonth5Expected);
    retVal = ReadLocaleInformation(abmonth5, abmonth5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth6[8];
	char abmonth6Expected[100];
	ReadStringParam (aSection, abmonth6);
    ReadStringParam(aSection,abmonth6Expected);
    retVal = ReadLocaleInformation(abmonth6, abmonth6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth7[8];
	char abmonth7Expected[100];
	ReadStringParam (aSection, abmonth7);
    ReadStringParam(aSection,abmonth7Expected);
    retVal = ReadLocaleInformation(abmonth7, abmonth7Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth8[8];
	char abmonth8Expected[100];
	ReadStringParam (aSection, abmonth8);
    ReadStringParam(aSection,abmonth8Expected);
    retVal = ReadLocaleInformation(abmonth8, abmonth8Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth9[8];
	char abmonth9Expected[100];
	ReadStringParam (aSection, abmonth9);
    ReadStringParam(aSection,abmonth9Expected);
    retVal = ReadLocaleInformation(abmonth9, abmonth9Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth10[8];
	char abmonth10Expected[100];
	ReadStringParam (aSection, abmonth10);
    ReadStringParam(aSection,abmonth10Expected);
    retVal = ReadLocaleInformation(abmonth10, abmonth10Expected);
	if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char abmonth11[8];
	char abmonth11Expected[100];
	ReadStringParam (aSection, abmonth11);
    ReadStringParam(aSection,abmonth11Expected);
    retVal = ReadLocaleInformation(abmonth11, abmonth11Expected);
    if(retVal == KErrGeneral)
		{	
    	return KErrGeneral;
		}
		
	char abmonth12[8];
	char abmonth12Expected[100];
	ReadStringParam (aSection, abmonth12);
    ReadStringParam(aSection,abmonth12Expected);
    retVal = ReadLocaleInformation(abmonth12, abmonth12Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month1[6];
	char month1Expected[100];
	ReadStringParam (aSection, month1);
    ReadStringParam(aSection,month1Expected);
    retVal = ReadLocaleInformation(month1, month1Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month2[6];
	char month2Expected[100];
	ReadStringParam (aSection, month2);
    ReadStringParam(aSection,month2Expected);
    retVal = ReadLocaleInformation(month2, month2Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
		
	char month3[6];
	char month3Expected[100];
	ReadStringParam (aSection, month3);
    ReadStringParam(aSection,month3Expected);
    retVal = ReadLocaleInformation(month3, month3Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month4[6];
	char month4Expected[100];
	ReadStringParam (aSection, month4);
    ReadStringParam(aSection,month4Expected);
    retVal = ReadLocaleInformation(month4, month4Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month5[6];
	char month5Expected[100];
	ReadStringParam (aSection, month5);
    ReadStringParam(aSection,month5Expected);
    retVal = ReadLocaleInformation(month5, month5Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month6[6];
	char month6Expected[100];
	ReadStringParam (aSection, month6);
    ReadStringParam(aSection,month6Expected);
    retVal = ReadLocaleInformation(month6, month6Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
	char month7[6];
	char month7Expected[100];
	ReadStringParam (aSection, month7);
    ReadStringParam(aSection,month7Expected);
    retVal = ReadLocaleInformation(month7, month7Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
		
	char month8[6];
	char month8Expected[100];
	ReadStringParam (aSection, month8);
    ReadStringParam(aSection,month8Expected);
    retVal = ReadLocaleInformation(month8, month8Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month9[6];
	char month9Expected[100];
	ReadStringParam (aSection, month9);
    ReadStringParam(aSection,month9Expected);
    retVal = ReadLocaleInformation(month9, month9Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month10[6];
	char month10Expected[100];
	ReadStringParam (aSection, month10);
    ReadStringParam(aSection,month10Expected);
    retVal = ReadLocaleInformation(month10, month10Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
			}
		
	char month11[6];
	char month11Expected[100];
	ReadStringParam (aSection, month11);
    ReadStringParam(aSection,month11Expected);
    retVal = ReadLocaleInformation(month11, month11Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char month12[6];
	char month12Expected[100];
	ReadStringParam (aSection, month12);
    ReadStringParam(aSection,month12Expected);
    retVal = ReadLocaleInformation(month12, month12Expected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char am[7];
	char amExpected[20];
	ReadStringParam (aSection, am);
    ReadStringParam(aSection,amExpected);
    retVal = ReadLocaleInformation(am, amExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char pm[7];
	char pmExpected[20];
	ReadStringParam (aSection, pm);
    ReadStringParam(aSection,pmExpected);
    retVal = ReadLocaleInformation(pm, pmExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char dfmt[7];
	char dfmtExpected[10];
	ReadStringParam (aSection, dfmt);
    ReadStringParam(aSection, dfmtExpected);
    retVal = ReadLocaleInformation(dfmt, dfmtExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char tfmt[7];
	char tfmtExpected[12];
	ReadStringParam (aSection, tfmt);
    ReadStringParam(aSection,tfmtExpected);
    retVal = ReadLocaleInformation(tfmt, tfmtExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char tfmtAmPm[7];
	char tfmtAmPmExpected[12];
	ReadStringParam (aSection, tfmtAmPm);
    ReadStringParam(aSection,tfmtAmPmExpected);
    retVal = ReadLocaleInformation(tfmtAmPm, tfmtAmPmExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char era[4];
	char eraExpected[2];
	ReadStringParam (aSection, era);
    ReadStringParam(aSection,eraExpected);
    retVal = ReadLocaleInformation(era, eraExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char eratfmt[10];
	char eratfmtExpected[2];
	ReadStringParam (aSection, eratfmt);
    ReadStringParam(aSection,eratfmtExpected);
    retVal = ReadLocaleInformation(eratfmt, eratfmtExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char eradtfmt[10];
	char eradtfmtExpected[2];
	ReadStringParam (aSection, eradtfmt);
    ReadStringParam(aSection,eradtfmtExpected);
    retVal = ReadLocaleInformation(eradtfmt, eradtfmtExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char altdigits[15];
	char altdigitsExpected[2];
	ReadStringParam (aSection, altdigits);
    ReadStringParam(aSection,altdigitsExpected);
    retVal = ReadLocaleInformation(altdigits, altdigitsExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char radixchar[10];
	char radixcharExpected[2];
	ReadStringParam (aSection, radixchar);
    ReadStringParam(aSection,radixcharExpected);
    retVal = ReadLocaleInformation(radixchar, radixcharExpected);
    if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    
	char thousep[10];
	char thousepExpected[2];
	ReadStringParam (aSection, thousep);
    ReadStringParam(aSection,thousepExpected);
    retVal = ReadLocaleInformation(thousep, thousepExpected);
	if(retVal == KErrGeneral)
		{
    	return KErrGeneral;
		}
    return retVal;
	}
TInt Clibc_loc_blr::ReadLocaleInformation(char* itemName, char* expectedOuput)
	{
	// Do some testing
	int item;	  	         
	if(SameString("CODESET",itemName))
		{
		item = CODESET;
		}
	if(SameString("D_T_FMT",itemName))
		{
		item = D_T_FMT;
		}
	if(SameString("D_FMT",itemName))
		{
		item = D_FMT;
		}

	if(SameString("T_FMT",itemName))
		{
		item = T_FMT;
		}

	if(SameString("T_FMT_AMPM",itemName))
		{
		item = T_FMT_AMPM;
		}

	if(SameString("AM_STR",itemName))
		{
		item = AM_STR;
		}

	if(SameString("PM_STR",itemName))
		{
		item = PM_STR;	
		}

	if(SameString("DAY_1",itemName)) 
		{
		item = DAY_1;	
		}
	if(SameString("DAY_2",itemName))
		{
		item = DAY_2;	
		}
	if(SameString("DAY_3",itemName))
		{
		item = DAY_3;	
		}
	if(SameString("DAY_4",itemName))
		{
		item = DAY_4;	
		}
	if(SameString("DAY_5",itemName))
		{
		item = DAY_5;	
		}
	if(SameString("DAY_6",itemName))
		{	
		item = DAY_6;	
		}
	if(SameString("DAY_7",itemName))
		{
		item = DAY_7;	
		}
	if(SameString("ABDAY_1",itemName)) 
		{
		item = ABDAY_1;	
		}
	if(SameString("ABDAY_2",itemName)) 
		{
		item = ABDAY_2;
		}
	if(SameString("ABDAY_3",itemName))
		{
		item = ABDAY_3;
		}
	if(SameString("ABDAY_4",itemName)) 
		{
		item = ABDAY_4;
		}
	if(SameString("ABDAY_5",itemName)) 
		{
		item = ABDAY_5;
		}
	if(SameString("ABDAY_6",itemName)) 
		{
		item = ABDAY_6;
		}
	if(SameString("ABDAY_7",itemName))
		{
		item = ABDAY_7;
		}

	if(SameString("MON_1",itemName))
		{
		item = MON_1;
		}
	if(SameString("MON_2",itemName))
		{
		item = MON_2;
		}
	if(SameString("MON_3",itemName))
		{
		item = MON_3;
		}
	if(SameString("MON_4",itemName))
		{
		item = MON_4;
		}
	if(SameString("MON_5",itemName))
		{
		item = MON_5;
		}
	if(SameString("MON_6",itemName))
		{
		item = MON_6;
		}
	if(SameString("MON_7",itemName))
		{
		item = MON_7;
		}
	if(SameString("MON_8",itemName))
		{
		item = MON_8;
		}
	if(SameString("MON_9",itemName))
		{
		item = MON_9;
		}
	if(SameString("MON_10",itemName)) 
		{
		item = MON_10;
		}
	if(SameString("MON_11",itemName)) 
		{
		item = MON_11;
		}
	if(SameString("MON_12",itemName))
		{
		item = MON_12;
		}

	if(SameString("ABMON_1",itemName)) 
		{
		item = ABMON_1;
		}
	if(SameString("ABMON_2",itemName)) 
		{
		item = ABMON_2;
		}
	if(SameString("ABMON_3",itemName)) 
		{
		item = ABMON_3;
		}
	if(SameString("ABMON_4",itemName))
		{	
		item = ABMON_4;
		}
	if(SameString("ABMON_5",itemName)) 
		{
		item = ABMON_5;
		}
	if(SameString("ABMON_6",itemName)) 
		{
		item = ABMON_6;
		}
	if(SameString("ABMON_7",itemName)) 
		{
		item = ABMON_7;
		}
	if(SameString("ABMON_8",itemName))
		{
		item = ABMON_8;
		}
	if(SameString("ABMON_9",itemName)) 
		{
		item = ABMON_9;
		}
	if(SameString("ABMON_10",itemName)) 
		{
		item = ABMON_10;
		}
	if(SameString("ABMON_11",itemName)) 
		{
		item = ABMON_11;
		}
	if(SameString("ABMON_12",itemName))
		{
		item = ABMON_12;
		}
	if(SameString("ERA",itemName))
		{
		item = ERA;
		}
	if(SameString("ERA_D_FMT",itemName))
		{
		item = ERA_D_FMT;
		}
	if(SameString("ERA_D_T_FMT",itemName))
		{
		item = ERA_D_T_FMT;
		}
	if(SameString("ERA_T_FMT",itemName))
		{
		item = ERA_T_FMT;
		}
	if(SameString("ALT_DIGITS",itemName))
		{
		item = ALT_DIGITS;
		}	
	if(SameString("RADIXCHAR",itemName))
		{
		item = RADIXCHAR;
		}
	if(SameString("THOUSEP",itemName))
		{
		item = THOUSEP;	
		}
	
	char temp[100];
    ConvertHexTochar(temp, expectedOuput);
    
	char* result =  nl_langinfo(item);
	if(result != NULL)
		{
		if(SameString(result,temp))	
			{
			return KErrNone;    		
			}
		else
			{
			iLog->Log(_L("Test Failed.\t\t"));
    		iLog->Log(_L8("ITEM = %s\t API Output =%s\t Expected Output = %s\n"), itemName, result,temp);
			return KErrGeneral;
			}
		}	
	else
		{
		return KErrGeneral;
		}
	}

TInt Clibc_loc_blr::TestIconv_Open_PassL( CStifSectionParser* aSection, TTestResult& aResult)
	{
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	TInt retVal = KErrGeneral;
		
	ReadStringParam (aSection, srcCharset);
	ReadStringParam (aSection, dstCharset);
	
	iLog->Log(_L8("Conversion from %s to %s"), srcCharset, dstCharset);
	TInt srcFlag = 0;
	TInt dstFlag = 0;
	TInt j = 0;
    while(j < SUPPORTED_CHARSETS)
		{
    	if(SameString(charsets[j].charsetName,srcCharset))
    		{
    		srcFlag = 1;
    		}
    	if(SameString(charsets[j].charsetName,dstCharset))
    		{
    		dstFlag = 1;
    		}
    	if(srcFlag && dstFlag)
    		{
    		break;
    		}
    	j++;
		}
	
	if(srcFlag && dstFlag)
		{
		errno = 0;
	
		iconv_t cd = iconv_open(dstCharset, srcCharset);
		if((errno != EINVAL) && (cd != (iconv_t) -1))
			{
			aResult.SetResult(KErrNone, _L("Test passed."));
			iLog->Log(_L("Test passed.\r\n"));	
			retVal =  KErrNone;
			}
		else
			{
			aResult.SetResult(KErrGeneral, _L("Test failed."));
			iLog->Log(_L("Test Failed.\r\n"));	
			retVal =  KErrGeneral;
			}
		iconv_close(cd);	
		}
	else
		{	
		aResult.SetResult(KErrNone, _L("Test passed."));
		retVal =  KErrNone;
		if(!srcFlag)
			iLog->Log(_L8("%s Character set not found :Test passed.\r\n"), srcCharset);
		if(!dstFlag)
			iLog->Log(_L8("%s Character set not found :Test passed.\r\n"), dstCharset);
		
		}
	
	
	return retVal;
	}

TInt Clibc_loc_blr::TestIconv_Open_FailL( CStifSectionParser* aSection, TTestResult& aResult)
	{
	TInt retVal = KErrGeneral;
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	
	ReadStringParam (aSection, srcCharset);
	ReadStringParam (aSection, dstCharset);
	
	iLog->Log(_L8("Conversion from %s to %s"), srcCharset, dstCharset);

	errno = 0;

	iconv_t cd = iconv_open(dstCharset, srcCharset);
	if((errno == EINVAL) && (cd == (iconv_t) -1))
		{
		aResult.SetResult(KErrNone, _L("Test passed.\r\n"));
		iLog->Log(_L("Test Passed"));
		retVal =  KErrNone;
		}
	else
		{
		aResult.SetResult(KErrGeneral, _L("Test failed.\r\n"));
		iLog->Log(_L("Test Failed"));
		retVal =  KErrGeneral;
		}
	iconv_close(cd);	

	return retVal;
	}


TInt Clibc_loc_blr::TestIconv_PassL(CStifSectionParser* aSection, TTestResult& aResult)
	{
	TInt retVal = KErrGeneral;
	char srcCharset[KMaxParamLength];
	char dstCharset[KMaxParamLength];
	char inputString[KMaxParamLength];
	char outputString[KMaxParamLength];
	char errorString[KMaxParamLength];
	TInt inputBytes;
	TInt inBytesAfterConversion;
	TInt outputBytes;
	TInt outBytesAfterConversion;
	TInt errNo = 0;
	TInt ret;
	
	
		
	ReadStringParam (aSection, srcCharset);
	ReadStringParam (aSection, dstCharset);
	ReadStringParam (aSection, inputString);
	ReadStringParam (aSection, outputString);
	ReadIntParam(aSection,inputBytes);
	ReadIntParam(aSection,inBytesAfterConversion);
	ReadIntParam(aSection,outputBytes);
	ReadIntParam(aSection,outBytesAfterConversion);
	ReadStringParam(aSection,errorString);
	ReadIntParam(aSection,ret);
	
	
	if(SameString("0",errorString))
		{
		errNo = 0;
		}
	if(SameString("EINVAL",errorString))
		{
		errNo = EINVAL;
		}
	if(SameString("EILSEQ",errorString))
		{
		errNo = EILSEQ;	
		}
	if(SameString("E2BIG",errorString))
		{
		errNo = E2BIG;	
		}
	
	char inputStringBeforeConvertion[KMaxParamLength];
	ConvertHexTochar(inputStringBeforeConvertion, inputString);
	
	iLog->Log(_L8("Conversion from %s to %s"), srcCharset, dstCharset);
	TInt srcFlag = 0;
	TInt dstFlag = 0;
	TInt j = 0;
    while(j < SUPPORTED_CHARSETS)
		{
    	if(SameString(charsets[j].charsetName,srcCharset))
    		{
    		srcFlag = 1;
    		}
    	if(SameString(charsets[j].charsetName,dstCharset))
    		{
    		dstFlag = 1;
    		}
    	if(srcFlag && dstFlag)
    		{
    		break;
    		}
    	j++;
		}
    if(srcFlag && dstFlag)
		{
    	errno = 0;
		iconv_t cd = iconv_open(dstCharset, srcCharset);
		if((errno != EINVAL) && (cd != (iconv_t) -1))
			{
			char* outbuf = outputString;
			const char* inbuf = inputStringBeforeConvertion;
			size_t inbytes = (size_t) inputBytes;
			size_t outbytes = (size_t) outputBytes;
			
			if(SameString("NULL",inputString) && SameString("NOT NULL",outputString))
				{
				retVal = iconv(cd, NULL,&inbytes, &outbuf, &outbytes);	
				}
			else if(SameString("NOT NULL",inputString) && SameString("NULL",outputString))
				{
				retVal = iconv(cd, &inbuf,&inbytes, NULL, &outbytes);	
				}
			else if(SameString("NULL",inputString) && SameString("NULL",outputString))
				{
				retVal = iconv(cd, NULL,&inbytes, NULL, &outbytes);	
				}
			else
				{
				retVal = iconv(cd, &inbuf,&inbytes, &outbuf, &outbytes);	
				}
			
			if((inbytes == inBytesAfterConversion) && 
			   (outbytes == outBytesAfterConversion) &&
			   (errNo == errno) && (ret == retVal ))
				{
				aResult.SetResult(KErrNone, _L("Test Passed.\r\n"));
				iLog->Log(_L("Test Passed"));
				retVal = KErrNone;
				}
			else
				{
				aResult.SetResult(KErrGeneral, _L("Test failed.\r\n"));
				iLog->Log(_L("Test Failed\r\n"));
				retVal = KErrGeneral;
				}
			}
		else
			{
			aResult.SetResult(KErrGeneral, _L("Test failed.\r\n"));
			iLog->Log(_L("Test Failed\r\n"));
			retVal = KErrGeneral;
			}
		
		iconv_close(cd);
		}
    else
		{
    	aResult.SetResult(KErrNone, _L("Test passed.\r\n"));
		retVal =  KErrNone;
		if(!srcFlag)
			iLog->Log(_L8("%s Character set not found :Test passed.\r\n"), srcCharset);
		if(!dstFlag)
			iLog->Log(_L8("%s Character set not found :Test passed.\r\n"), dstCharset);
		}
	

	return retVal;
	
	}
void Clibc_loc_blr::ConvertHexTochar(char* dst, char*src)
	{
	int i = 0;
	
	while(*src != '\0')
		{
		if(*src == '\\')
			{
			src++;
			if(*src == 'x')
				{	
				src++;
				char temp[2];
				temp[0] = *src;
				src++;
				temp[1] = *src;
				int tenPos = 0;
				int digitPos = 0;
				if(isalpha(temp[0]))
					{
					tenPos = isupper(temp[0])?temp[0] - 'A' + 10: temp[0] - 'a' + 10;
					
					}
				if(isdigit(temp[0]))
					{
					tenPos = temp[0] - '0';
					}
				if(isalpha(temp[1]))
					{
					digitPos = isupper(temp[1])?temp[1] - 'A' + 10: temp[1] - 'a' + 10;
					
					}
				if(isdigit(temp[1]))
					{
					digitPos = temp[1] - '0';
					}
				dst[i++] = tenPos *16 + digitPos;
				}
			}
		else
			{
			dst[i++] = *src;
			}
		src++;
		}
	dst[i++] = '\0';
	}

void Clibc_loc_blr::LocateSupportedDLLs()
	{
	TInt j = 0;
	FILE* stream = NULL;
	stream = fopen("C:\\locales.txt", "r");
	
	RSessionBase aSessionBase;
	RLoader loader;
			
	char buffer[25];
	char* buf = fgets(buffer, 50, stream);
	
	while(buf != NULL)
		{
		int  i = 0;
		while(buffer[i] != '=')
			{
			locales[j].localeName[i] = buffer[i];
			i++;				
			}
		locales[j].localeName[i++] = '\0';

		TBuf<20> DllName;
		_LIT(KDllName, "elocl.");
		DllName.Copy(KDllName);
				
		while(buffer[i] && ('\r' != buffer[i]) && ('\n' != buffer[i]))
			{
			DllName.Append(buffer[i]);	
			i++;
			}
		
		TInt r = loader.Connect();
		if(KErrNone == r)
			{
			TLibraryFunction data[KNumLocaleExports];
			TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
			TPtr8 functionListBuf((TUint8*) data, size, size);
			r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&DllName, &functionListBuf) );
			locales[j].flag = (KErrNone == r)? 1:0;
			}
		aSessionBase.Close();
		loader.Close();
		
		buffer[0] = '\0';
		buf = fgets(buffer, 50, stream);
		j++;
		}
	fclose(stream);
	}

void Clibc_loc_blr::SupportedCharsets()
	{
	RFs fileSession;
	fileSession.Connect();
	CleanupClosePushL(fileSession);
		
	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewL() ;
	CleanupStack::PushL(conv);
		
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet> *charSet = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(fileSession);
	CleanupStack::PushL( charSet );
	
	TInt i = 0;
	TInt j = 0;
	
	TInt count = charSet->Count();
	while(i < count)
		{
		CCnvCharacterSetConverter::SCharacterSet characterSet = charSet->At(i);	
		TUint charSetUID = characterSet.Identifier();
				
		const TBufC<25> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();								
		TBuf8<25> aCharSetname;
		aCharSetname.Copy(charSetName);
	
		strncpy(charsets[j].charsetName, (char*) aCharSetname.Ptr(), aCharSetname.Length());
		charsets[j].charsetName[aCharSetname.Length()] = '\0';
		j++;
		
		HBufC8* stdInterName = NULL;		 
		stdInterName = conv->ConvertCharacterSetIdentifierToStandardNameL(charSetUID, fileSession);					
		
		if(NULL != stdInterName)
			{
			strncpy(charsets[j].charsetName, (char*) stdInterName->Ptr(), stdInterName->Length());
			charsets[j].charsetName[stdInterName->Length()] = '\0';
			j++;
			}
		delete stdInterName;
		stdInterName = NULL;
		i++;
		}
	
	CleanupStack::PopAndDestroy(3); 
	}
// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);
    
    }
#endif // EKA2

//  End of File

