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



#include "tfloat_blr.h"

TInt CTestFloat_blr::acos_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
    	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
		{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(acos) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTestFloat_blr::acosh_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(acosh) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::asin_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
    	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(asin) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTestFloat_blr::asinh_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(asinh) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::atan_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(atan) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::atanh_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(atanh) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTestFloat_blr::Cos_test()
    {
    
  	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(cos) (input);
    
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
	
    return KErrNone;
    
    }  
 
TInt CTestFloat_blr::sin_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(sin) (input);
        
    if(check_longlong(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::sinh_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(sinh) (input);
        
    if(check_longlong(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;

	}

TInt CTestFloat_blr::cosh_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(cosh) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
	
    return KErrNone;
	}

TInt CTestFloat_blr::tan_test()
	{	
	char chParam[20];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
   	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(tan) (input);
        
    if(check_longlong(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}



TInt CTestFloat_blr::tanh_test()
	{	
	char chParam[20];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(tanh) (input);
        
    if(check_longlong(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}


TInt CTestFloat_blr::atan2_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (input2);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(atan2) (input1, input2);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}
