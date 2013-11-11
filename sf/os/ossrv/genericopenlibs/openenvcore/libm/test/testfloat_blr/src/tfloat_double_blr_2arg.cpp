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


TInt CTestFloat_blr::fdim_test()
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
    FLOAT res = FUNC(fdim) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTestFloat_blr::fmax_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(fmax) (input1, input2);
        
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

	
TInt CTestFloat_blr::fmin_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(fmin) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTestFloat_blr::fmod_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(fmod) (input1, input2);
        
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


TInt CTestFloat_blr::hypot_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(hypot) (input1, input2);
        
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


TInt CTestFloat_blr::remainder_test()
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
    
  	TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
  	// Do some testing
    FLOAT res = FUNC(remainder) (input1, input2);
        
	
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


TInt CTestFloat_blr::nexttoward_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(nexttoward) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::nextafter_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(nextafter) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}

	
TInt CTestFloat_blr::copysign_test()
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
    FLOAT res = FUNC(copysign) (input1, input2);
        
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


TInt CTestFloat_blr::jn_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(jn) (input1, input2);
        
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

TInt CTestFloat_blr::yn_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(yn) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTestFloat_blr::scalb_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(scalb) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTestFloat_blr::scalbn_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	  	 
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(scalbn) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}

TInt CTestFloat_blr::pow_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(pow) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


	
TInt CTestFloat_blr::fma_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT input3;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( input3);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(fma) (input1, input2, input3);
        
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
    INFO_PRINTF2(_L("Input Value  1 : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value  2 : %f\n"), input2 );
    INFO_PRINTF2(_L("Input Value  3 : %f\n"), input3 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTestFloat_blr::ldexp_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(ldexp) (input1, input2);
        
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

	
TInt CTestFloat_blr::frexp_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	int iPart;
  	FLOAT expected_fPart;
  	FLOAT expected_iPart;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( expected_fPart);
    ReadFloatParam ( expected_iPart);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(frexp) (input1, &iPart);
        
    if(   check_int(iPart, expected_iPart, max_ulp)
    	&&check_float(res, expected_fPart, max_ulp, gen_ulp)
      )
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
    INFO_PRINTF2(_L("Input Value  1 : %f\n"), input1 );
    INFO_PRINTF2(_L("Expected fPart : %f\n"), expected_fPart );
    INFO_PRINTF2(_L("Expected iPart : %f\n"), expected_iPart );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  fPart  : %f\n"), res );
	INFO_PRINTF2(_L("Result  iPart  : %d\n"), iPart );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
		
	}	


TInt CTestFloat_blr::remquo_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	int iPart;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected);
    ReadFloatParam ( max_ulp);
    
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(remquo) (input1, input2, &iPart);
        
    if(  check_float(res, expected, max_ulp, gen_ulp))
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
    INFO_PRINTF2(_L("Input Value  1 : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value  2 : %f\n"), input1 );
    INFO_PRINTF2(_L("Expected iPart : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  fPart  : %f\n"), res );
	INFO_PRINTF2(_L("Result  iPart  : %d\n"), iPart );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	
	}	

	
TInt CTestFloat_blr::remquo_remcheck_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT input2;
  	int iPart;
  	FLOAT expected_fPart;
  	FLOAT expected_iPart;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( input2);
    ReadFloatParam ( expected_fPart);
    ReadFloatParam ( expected_iPart);
    ReadFloatParam ( max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(remquo) (input1, input2, &iPart);
    
    if(   check_int(iPart, expected_iPart, max_ulp)
    	&&check_float(res, expected_fPart, max_ulp, gen_ulp)
      )
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
    INFO_PRINTF2(_L("Input Value  1 : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value  2 : %f\n"), input1 );
    INFO_PRINTF2(_L("Expected iPart : %f\n"), expected_fPart );
    INFO_PRINTF2(_L("Expected iPart : %f\n"), expected_iPart );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  fPart  : %f\n"), res );
	INFO_PRINTF2(_L("Result  iPart  : %d\n"), iPart );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
		
	}
	
TInt CTestFloat_blr::gamma_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT signgam_exp;
   	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( expected);
	ReadFloatParam ( signgam_exp);
    ReadFloatParam ( max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
	signgam = 0;
	FLOAT res = FUNC(gamma) (input1);
    
	if(  check_float(res, expected, max_ulp, gen_ulp) && check_int((TInt)signgam_exp,signgam, 0))
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
	INFO_PRINTF2(_L("Input Value   	  : %f\n"), input1 );
	INFO_PRINTF2(_L("Expected Res  	  : %f\n"), expected );
	INFO_PRINTF2(_L("Expected signagam  : %f\n"), signgam_exp );
	INFO_PRINTF2(_L("Max ULP value      : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		      : %f\n"), res );
	INFO_PRINTF2(_L("Obtained Signgam	  : %d\n"), signgam );
	INFO_PRINTF2(_L("Generated Ulp      : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
 
	return KErrNone;
		
	}

		
TInt CTestFloat_blr::lgamma_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT signgam_exp;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam ( chParam);
    ReadFloatParam ( input1);
    ReadFloatParam ( expected);
	ReadFloatParam ( signgam_exp);
    ReadFloatParam ( max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
	// Do some testing
	signgam = 0;
	FLOAT res = FUNC(lgamma) (input1);
    
	if(  check_float(res, expected, max_ulp, gen_ulp) && signgam_exp==signgam)
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
	INFO_PRINTF2(_L("Input Value   	  : %f\n"), input1 );
	INFO_PRINTF2(_L("Expected Res  	  : %f\n"), expected );
	INFO_PRINTF2(_L("Expected signagam  : %f\n"), signgam_exp );
	INFO_PRINTF2(_L("Max ULP value      : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		      : %f\n"), res );
	INFO_PRINTF2(_L("Obtained Signgam	  : %d\n"), signgam );
	INFO_PRINTF2(_L("Generated Ulp      : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
 
	
	return KErrNone;
		
	}

		
TInt CTestFloat_blr::scalbln_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	TInt32 input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam(chParam);
    ReadFloatParam(input1);
    ReadLIntParam(input2);
    ReadFloatParam(expected);
    ReadFloatParam(max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(scalbln) (input1, input2);
        
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
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %d\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
     return KErrNone;
	}

