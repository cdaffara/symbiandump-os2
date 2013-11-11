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



// INCLUDE FILES
#include <e32math.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "tfloat_blr.h"

CTestFloat_blr::~CTestFloat_blr() 
	{ 
	}  

CTestFloat_blr::CTestFloat_blr(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
    iParamCnt=0;
	SetTestStepName(aStepName);		
	}

TVerdict CTestFloat_blr::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestFloat_blr::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestFloat_blr::doTestStepL()
	{
	int err;

	if(TestStepName() == Kcbrt_test)
		{
   		INFO_PRINTF1(_L("cbrt_test():"));
   		err = cbrt_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kceil_test)
   		{
   		INFO_PRINTF1(_L("ceil_test():"));
   		err = ceil_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kerf_test)
   		{
   		INFO_PRINTF1(_L("erf_test():"));
   		err = erf_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kerfc_test)
   		{
   		INFO_PRINTF1(_L("erfc_test():"));
   		err = erfc_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kexp_test)
   		{
   		INFO_PRINTF1(_L("exp_test():"));
   		err = exp_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kexp2_test)
   		{
   		INFO_PRINTF1(_L("exp2_test():"));
   		err = exp2_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kexpm1_test)
   		{
   		INFO_PRINTF1(_L("expm1_test():"));
   		err = expm1_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   	
   	else if(TestStepName() == Kfabs_test)  
   		{
   		INFO_PRINTF1(_L("fabs_test():"));
   		err = fabs_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kilogb_test)
   		{
   		INFO_PRINTF1(_L("ilogb_test():"));
   		err = ilogb_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kj0_test)
   		{
   		INFO_PRINTF1(_L("j0_test():"));
   		err = j0_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kj1_test)
   		{
   		INFO_PRINTF1(_L("j1_test():"));
   		err = j1_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klrint_test)
   		{
   		INFO_PRINTF1(_L("lrint_test():"));
   		err = lrint_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	   	
   	else if(TestStepName() == Kllrint_test)  
   		{
   		INFO_PRINTF1(_L("llrint_test():"));
   		err = llrint_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klog_test)
   		{
   		INFO_PRINTF1(_L("log_test():"));
   		err = log_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klog10_test)
   		{
   		INFO_PRINTF1(_L("log10_test():"));
   		err = log10_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klog1pf_test)
   		{
   		INFO_PRINTF1(_L("log1p_test():"));
   		err = log1p_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klogb_test)
   		{
   		INFO_PRINTF1(_L("logb_test():"));
   		err = logb_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Klroundf_test)
   		{
   		INFO_PRINTF1(_L("lround_test():"));
   		err = lround_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kllroundf_test)
   		{
   		INFO_PRINTF1(_L("llround_test():"));
   		err = llround_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Krintf_test)
   		{
   		INFO_PRINTF1(_L("rint_test():"));
   		err = rint_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ksqrtf_test)
   		{
   		INFO_PRINTF1(_L("sqrt_test():"));
   		err = sqrt_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ktruncf_test)
   		{
   		INFO_PRINTF1(_L("trunc_test():"));
   		err = trunc_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ky0f_test)
   		{
   		INFO_PRINTF1(_L("y0_test():"));
   		err = y0_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ky1f_test)
   		{
   		INFO_PRINTF1(_L("y1_test():"));
   		err = y1_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kfloorf_test)
   		{
   		INFO_PRINTF1(_L("floor_test():"));
   		err = floor_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Ksignificandf_test)
   		{
   		INFO_PRINTF1(_L("significand_test():"));
   		err = significand_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Knearbyintf_test)
   		{
   		INFO_PRINTF1(_L("nearbyint_test():"));
   		err = nearbyint_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kisinf_test)
   		{
   		INFO_PRINTF1(_L("isinf_test():"));
   		err = isinf_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Kfdimf_test)
   		{
   		INFO_PRINTF1(_L("fdim_test():"));
   		err = fdim_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
    else if(TestStepName() == Kfmax_test)
    	{
   		INFO_PRINTF1(_L("fmax_test():"));
   		err = fmax_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kfmin_test)
    	{
   		INFO_PRINTF1(_L("fmin_test():"));
   		err = fmin_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == Kfmod_test)
    	{
   		INFO_PRINTF1(_L("fmod_test():"));
   		err = fmod_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == Khypot_test)
   		{
   		INFO_PRINTF1(_L("hypot_test():"));
   		err = hypot_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
    else if(TestStepName() == Knexttoward_test)
   		{
   		INFO_PRINTF1(_L("nexttoward_test():"));
   		err = nexttoward_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Kremainder_test)
    	{
   		INFO_PRINTF1(_L("remainder_test():"));
   		err = remainder_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == Kcopysign_test)
    	{
   		INFO_PRINTF1(_L("copysign_test():"));
   		err = copysign_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == Kfjn_test)
   		{
   		INFO_PRINTF1(_L("jn_test():"));
   		err = jn_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
    else if(TestStepName() == Kfyn_test)
   		{
   		INFO_PRINTF1(_L("yn_test():"));
   		err = yn_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
    else if(TestStepName() == Kscalbf_test)
    	{
   		INFO_PRINTF1(_L("scalb_test():"));
   		err = scalb_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == Kscalbnf_test)
    	{
   		INFO_PRINTF1(_L("scalbn_test():"));
   		err = scalbn_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	 
   	else if(TestStepName() == Kpow_test)
    	{
   		INFO_PRINTF1(_L("pow_test():"));
   		err = pow_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == Kacosf_test)
   		{
   		INFO_PRINTF1(_L("acos_test():"));
   		err = acos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Kacoshf_test)
    	{
   		INFO_PRINTF1(_L("acosh_test():"));
   		err = acosh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == Kasinf_test)
    	{
   		INFO_PRINTF1(_L("asin_test():"));
   		err = asin_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	
   	else if(TestStepName() == Kasinh_test)
    	{
   		INFO_PRINTF1(_L("asinh_test():"));
   		err = asinh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Katan_test)
    	{
   		INFO_PRINTF1(_L("atan_test():"));
   		err = atan_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    
   	else if(TestStepName() == Katanh_test)
    	{
   		INFO_PRINTF1(_L("atanh_test():"));
   		err = atanh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KCosf_test)
    	{
   		INFO_PRINTF1(_L("Cos_test():"));
   		err = Cos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	 
   	else if(TestStepName() == Kfpclassify_test)
    	{
   		INFO_PRINTF1(_L("fpclassify_test():"));
   		err = fpclassify_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Kcosh_test)
    	{
   		INFO_PRINTF1(_L("cosh_test():"));
   		err = cosh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == Ksinf_test)
    	{
   		INFO_PRINTF1(_L("sin_test():"));
   		err = sin_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   	else if(TestStepName() == Ksinh_test)
    	{
   		INFO_PRINTF1(_L("sinh_test():"));
   		err = sinh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == Ktan_test)
    	{
   		INFO_PRINTF1(_L("tan_test():"));
   		err = tan_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	 	 
   	else if(TestStepName() == Ktanh_test)
    	{
   		INFO_PRINTF1(_L("tanh_test():"));
   		err = tanh_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == Katan2_test)
    	{
   		INFO_PRINTF1(_L("atan2_test:"));
   		err = atan2_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kfma_test)
    	{
   		INFO_PRINTF1(_L("fma_test():"));
   		err = fma_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kisfinitef_test)
    	{
   		INFO_PRINTF1(_L("isfinite_test:"));
   		err = isfinite_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   else if(TestStepName() == Kisnormal_test)
    	{
   		INFO_PRINTF1(_L("isnormal_test():"));
   		err = isnormal_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Ksignbit_test)
    	{
   		INFO_PRINTF1(_L("signbit_test:"));
   		err = signbit_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kldexpf_test)
    	{
   		INFO_PRINTF1(_L("ldexp_test():"));
   		err = ldexp_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	 
   else if(TestStepName() == Kroundf_test)
    	{
   		INFO_PRINTF1(_L("round_test:"));
   		err = round_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   else if(TestStepName() == Kscalbln_test)
    	{
   		INFO_PRINTF1(_L("scalbln_test():"));
   		err = scalbln_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kfinitef_test)
    	{
   		INFO_PRINTF1(_L("finite_test:"));
   		err = finite_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kmodf_test)
    	{
   		INFO_PRINTF1(_L("modf_test():"));
   		err = modf_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kfrexp_test)
    	{
   		INFO_PRINTF1(_L("frexp_test:"));
   		err = frexp_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kremquo_test)
    	{
   		INFO_PRINTF1(_L("remquo_test():"));
   		err = remquo_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   
   else if(TestStepName() == Kremquo_remcheckf_test)
    	{
   		INFO_PRINTF1(_L("remquo_remcheck_test:"));
   		err = remquo_remcheck_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Kgamma_test)
    	{
   		INFO_PRINTF1(_L("gamma_test():"));
   		err = gamma_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Klgamma_test)
    	{
   		INFO_PRINTF1(_L("lgamma_test():"));
   		err = lgamma_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   else if(TestStepName() == Knextafter_test)
    	{
   		INFO_PRINTF1(_L("nextafter_test():"));
   		err = nextafter_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	 		  	
	return TestStepResult(); 
	}
	
// -----------------------------------------------------------------------------
// Utility functions for testing.
// -----------------------------------------------------------------------------

void CTestFloat_blr::ReadIntParam(TInt &aInt)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(Kparam,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	}

void CTestFloat_blr::ReadStringParam(char* aString)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<100> pNameBuf;
  	TPtrC desc;
  	TInt i;
	
	pNameBuf.Format(Kparam,++iParamCnt);
	TBool ret = GetStringFromConfig(ConfigSection(), pNameBuf, desc);
	
	if ( desc == _L("\"\"") )
	  	{
	  	i = 0;
	  	}
	else
		{
		// If the string is quoted, take only the insides
		if ((desc[0] == '\"') && (desc[desc.Length()-1] == '\"'))
			{
			for ( i=0; i<desc.Length()-2; i++ )
    	  		{
        		aString[i]=desc[i+1];
        		}
      		}
    	// Otherwise, take the whole string
    	else
      		{
	    	for ( i=0; i<desc.Length(); i++ )
    	  		{
        		aString[i]=desc[i];
        		}
      		}
    	}
    aString[i]='\0';
   	}	
   	
void CTestFloat_blr::ReadLIntParam(TInt32 &aLInt)
	{
	char Str[MAX_SIZE];
	TInt32 d;
	ReadStringParam(Str);
	/* Strtol converts the initial part of the
	string into long integer value according to the 
	give base [Third arg]*/
	
	d = strtol(Str,NULL, 10);
	aLInt = d; 
	}
	
void CTestFloat_blr::ReadLLIntParam(TInt64 &aLInt)
	{
	char Str[MAX_SIZE];
	TInt64 d; 							/*long long int*/
	ReadStringParam(Str);
	/* Strtoll converts the initial part of the
	string into long long integer value according to the 
	give base [Third arg]*/
	
	d = strtoll(Str,NULL, 10);
	aLInt = d; 
	}		
	
void CTestFloat_blr::ReadFloatParam(FLOAT &aDbl)
	{
	char Str[MAX_SIZE];
	double d;
	ReadStringParam(Str);
	#ifdef TESTING_FLOAT
		d = strtod(Str,NULL);
	#else
	  	d = strtod(Str,NULL);
	#endif
	aDbl = d; 
	}	

int CTestFloat_blr::check_bool (int computed, int expected)
	{
	int ok = 0;
		
	if ((computed == 0) == (expected == 0))
		{
    	ok = 1;
		}
		
	return ok;
	}

int CTestFloat_blr::check_int (int computed, int expected,  int max_ulp)
	{
  	int diff = computed - expected;
  	int ok = 0;

  	if (abs (diff) <= max_ulp)
  		{
    	ok = 1;
		}
  	return ok;
	}

int CTestFloat_blr::check_long (TInt32 computed, TInt32 expected,  FLOAT max_ulp, FLOAT &gen_ulp)
 	{
	long int diff = computed - expected;
  	int ok = 0;

  	if ((gen_ulp =labs (diff)) <= max_ulp)
  		{
    	ok = 1;
		}
	
	return ok;
	}	
int CTestFloat_blr::check_longlong (TInt64 computed, TInt64 expected,  FLOAT max_ulp, FLOAT &gen_ulp)
	{
  	long long int diff = computed - expected;
  	int ok = 0;
  
  	if ( (gen_ulp = llabs (diff)) <= max_ulp)
    	{
    	ok = 1;
		}

  	return ok;
	}
	
 
  // -----------------------------------------------------------------------------
// Test methods
// -----------------------------------------------------------------------------
//                  

TInt CTestFloat_blr::cbrt_test()
	{
 	// Create temporary variables in stack
   	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
   	// Read parameters
  	
  	ReadStringParam(chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    ReadFloatParam (max_ulp);
    
    // Do some testing
    FLOAT res = FUNC(cbrt) (input);
    
    TBuf<MAX_SIZE> buf;
    
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
		{
    	buf.Append(chParam[j]);
		}
    
        
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

	
TInt CTestFloat_blr::ceil_test()
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
    FLOAT res = FUNC(ceil) (input);
        
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

TInt CTestFloat_blr::erf_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
		{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(erf) (input);
        
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


TInt CTestFloat_blr::erfc_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
     FLOAT res = FUNC(erfc) (input);
        
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

int  CTestFloat_blr::check_float (FLOAT computed, FLOAT expected,FLOAT max_ulp, FLOAT &gen_ulp)
	{
	int ok = 0;
	FLOAT diff = 0;
	FLOAT ulp = 0;
	  
	if (isnan (computed) && isnan (expected))
		{
	  	ok = 1;
		}
	else if (isinf (computed) && isinf (expected))
	    {
	    /* Test for sign of infinities.  */
	    if ((IGNORE_ZERO_INF_SIGN) == 0
		  && signbit (computed) != signbit (expected))
			{
			ok = 0;
			}
	    else
			{
			ok = 1;
			}
	    }
	  /* Don't calc ulp for NaNs or infinities.  */
	else if (isinf (computed) || isnan (computed) || isinf (expected) || isnan (expected))
		{
	    ok = 0;
		}
		else
			{
			diff = FUNC(fabs) (computed - expected);
			/* ilogb (0) isn't allowed.  */
			if (expected == 0.0)
				{
				ulp = diff / FUNC(ldexp) (1.0, - MANT_DIG);
				}
			else
				{
				ulp = diff / FUNC(ldexp) (1.0, FUNC(ilogb) (expected) - MANT_DIG);
				}
	      
			if ((IGNORE_ZERO_INF_SIGN) == 0
				&& computed == 0.0 && expected == 0.0
				&& signbit(computed) != signbit (expected))
				{
				ok = 0;
				}
			else if (ulp == 0.0 || (ulp <= max_ulp ))
				{
				ok = 1;
				}	
			else
				{
				ok = 0;
				}

			}
	gen_ulp = ulp ;  
	  
	return ok;
	}	
	
TInt CTestFloat_blr::exp_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    	
    // Do some testing
    FLOAT res = FUNC(exp) (input);
        
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
	
TInt CTestFloat_blr::exp2_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(exp2) (input);
        
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


TInt CTestFloat_blr::expm1_test()
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
    
    TBuf<MAX_SIZE> buf;
    
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(expm1) (input);
        
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

TInt CTestFloat_blr::fabs_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(fabs)(input);
        
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

TInt CTestFloat_blr::ilogb_test()
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
    FLOAT res = FUNC(ilogb) (input);
        
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

TInt CTestFloat_blr::j0_test()
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
    FLOAT res = FUNC(j0) (input);
        
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


TInt CTestFloat_blr::j1_test()
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
    FLOAT res = FUNC(j1) (input);
        
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

TInt CTestFloat_blr::lrint_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	long int expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
   	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadLIntParam   (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    TInt32 res = FUNC(lrint) (input);
        
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
    INFO_PRINTF2(_L("Expected Value : %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}
TInt CTestFloat_blr::llrint_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	TInt64 expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	// Read parameters
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadLLIntParam  (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    TInt64 res = FUNC(llrint) (input);
        
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


TInt CTestFloat_blr::log_test()
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
    
    // Do some testing
    FLOAT res = FUNC(log) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    TBuf<MAX_SIZE> buf;
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

TInt CTestFloat_blr::log10_test()
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
    FLOAT res = FUNC(log10) (input);
        
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


TInt CTestFloat_blr::log1p_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(log1p) (input);
        
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
	
TInt CTestFloat_blr::logb_test()
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
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    FLOAT res = FUNC(logb)(input);
        
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

	
	
TInt CTestFloat_blr::lround_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	TInt32 expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadLIntParam (expected);
    ReadFloatParam (max_ulp);
    
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
  	
    // Do some testing
    TInt32 res = FUNC(lround) (input);
        
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

TInt CTestFloat_blr::llround_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	TInt64 expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadLLIntParam  (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    TInt64 res = FUNC(llround) (input);
        
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
    INFO_PRINTF2(_L("Expected Value : %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
 
    return KErrNone;

	}

TInt CTestFloat_blr::rint_test()
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
    FLOAT res = FUNC(rint) (input);
        
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


TInt CTestFloat_blr::sqrt_test()
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
    FLOAT res = FUNC(sqrt)(input);
        
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


TInt CTestFloat_blr::trunc_test()
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
    FLOAT res = FUNC(trunc) (input);
        
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

TInt CTestFloat_blr::y0_test()
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
    
    // Do some testing
    FLOAT res = FUNC(y0) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    
    TBuf<MAX_SIZE> buf;
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		  : %S\n"),&buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::y1_test()
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
    FLOAT res = FUNC(y1) (input);
        
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

TInt CTestFloat_blr::significand_test()
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
    
    // Do some testing
    FLOAT res = FUNC(significand) (input);
        
    if(check_float(res, expected, max_ulp, gen_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    	
    TBuf<MAX_SIZE> buf;
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


TInt CTestFloat_blr::floor_test()
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
    FLOAT res = FUNC(floor) (input);
        
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

TInt CTestFloat_blr::nearbyint_test()
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
    FLOAT res = FUNC(nearbyint) (input);
        
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

TInt CTestFloat_blr::isinf_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  
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
    int res = __isinff(input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
  
    return KErrNone;
	}

	
TInt CTestFloat_blr::isnan_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  
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
    int res = isnan (input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}
	
TInt CTestFloat_blr::fpclassify_test()
	{
    
	// Create temporary variables in stack
	char chParam[MAX_SIZE];
	FLOAT input;
    int expected;
	FLOAT max_ulp;
	
    // Read parameters
    ReadStringParam (chParam ) ;
    ReadFloatParam (input );
    ReadIntParam    (expected );
    ReadFloatParam (max_ulp );
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    // Do some testing
    
    // Set test result
    int res = __fpclassifyf(input);
    INFO_PRINTF1(_L("_________________________________________\n"));
    INFO_PRINTF2(_L("TestCase		: %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value   : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value: %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		: %f\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
	
    if(check_int(res, expected, max_ulp))
    	{
    	INFO_PRINTF1(_L("Test passed."));
    	}
    else    
    	{
    	ERR_PRINTF1(_L("Test Failed."));
   		return KErrGeneral;
    	}
    return KErrNone;
    
    }  

TInt CTestFloat_blr::isfinite_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	
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
    int res = isfinite (input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}

TInt CTestFloat_blr::finite_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadFloatParam (expected);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
       
    // Do some testing
    int res = FUNC(finite) (input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTestFloat_blr::isnormal_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	
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
    int res = __isnormalf(input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    
    return KErrNone;
	}


TInt CTestFloat_blr::signbit_test()
	{
 	// Create temporary variables in stack
  	char chParam[MAX_SIZE];
  	FLOAT input;
  	FLOAT expected;
  	FLOAT max_ulp;
  	  	
  	
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
    int res = __signbitf(input);
        
    if(check_bool(res, expected))
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
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}
	
	
TInt CTestFloat_blr::round_test()
	{	
	char chParam[MAX_SIZE];
  	FLOAT input;
  	long int expected;
  	FLOAT max_ulp;
  	FLOAT gen_ulp;
  	
  	// Read parameters
  	
  	ReadStringParam (chParam);
    ReadFloatParam (input);
    ReadLIntParam   (expected);
    ReadFloatParam (max_ulp);
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    TInt32 res = FUNC(round) (input);
        
    if(check_long(res, expected, max_ulp, gen_ulp))
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
    INFO_PRINTF2(_L("Expected Value : %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;

	}

TInt CTestFloat_blr::modf_test()
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
    //
    TBuf<MAX_SIZE> buf;
    TInt len = strlen(chParam);
    
    for (TInt j =0; j<len;j++)
    	{
    	buf.Append(chParam[j]);
		}
    
    // Do some testing
    FLOAT res = FUNC(modf)(input1, &iPart);
        
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
	INFO_PRINTF2(_L("Result  iPart  : %f\n"), iPart );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    
    return KErrNone;
		
	}
