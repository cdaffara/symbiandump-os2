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




#include "tldouble_blr.h"

TInt CTLongDouble_blr::powl_test()
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


TInt CTLongDouble_blr::fdiml_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf  );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input1 );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input2 );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTLongDouble_blr::dreml_test()
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
    FLOAT res = FUNC(drem) (input1, input2);
        
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


TInt CTLongDouble_blr::fminl_test()
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

TInt CTLongDouble_blr::fmaxl_test()
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


TInt CTLongDouble_blr::fmodl_test()
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


TInt CTLongDouble_blr::hypotl_test()
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


TInt CTLongDouble_blr::remainderl_test()
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


TInt CTLongDouble_blr::nexttowardl_test()
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


TInt CTLongDouble_blr::nextafterl_test()
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

TInt CTLongDouble_blr::copysignl_test()
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


TInt CTLongDouble_blr::jnl_test()
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


TInt CTLongDouble_blr::ynl_test()
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

TInt CTLongDouble_blr::scalbl_test()
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


TInt CTLongDouble_blr::scalbnl_test()
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


TInt CTLongDouble_blr::scalblnl_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	TInt32 input2;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadLIntParam (input2);
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


TInt CTLongDouble_blr::ldexpl_test()
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

TInt CTLongDouble_blr::fmal_test()
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
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (input2);
    ReadFloatParam (input3);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
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


TInt CTLongDouble_blr::modfl_test()
	{
	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT iPart;
  	FLOAT expected_fPart;
  	FLOAT expected_iPart;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (expected_fPart);
    ReadFloatParam (expected_iPart);
    ReadFloatParam (max_ulp);
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(modf) ((double)input1,(double*) &iPart);
        
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value  1 : %f\n"), input1 );
    INFO_PRINTF2(_L("Expected fPart : %f\n"), expected_fPart );
    INFO_PRINTF2(_L("Expected iPart : %f\n"), expected_iPart );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  fPart  : %f\n"), res );
	INFO_PRINTF2(_L("Result  iPart  : %f\n"), iPart );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}
	
	
TInt CTLongDouble_blr::frexpl_test()
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
   	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (expected_fPart);
    ReadFloatParam (expected_iPart);
    ReadFloatParam (max_ulp);
    
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
	
	
TInt CTLongDouble_blr::remquol_test()
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
	
TInt CTLongDouble_blr::remquo_remcheckl_test()
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
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (input2);
    ReadFloatParam (expected_fPart);
    ReadFloatParam (expected_iPart);
    ReadFloatParam (max_ulp);
    
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
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

TInt CTLongDouble_blr::gammal_test()
	{
	
			// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT signgam_exp;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (expected);
	ReadFloatParam (signgam_exp);
    ReadFloatParam (max_ulp);
    	
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
	INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
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

	
TInt CTLongDouble_blr::lgammal_test()
	{
		// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input1;
  	FLOAT signgam_exp;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input1);
    ReadFloatParam (expected);
	ReadFloatParam (signgam_exp);
    ReadFloatParam (max_ulp);
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
	INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
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
