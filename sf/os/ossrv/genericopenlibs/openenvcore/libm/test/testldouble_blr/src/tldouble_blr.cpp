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
#include "tldouble_blr.h"

CTLongDouble_blr::~CTLongDouble_blr() 
	{ 
	}  

CTLongDouble_blr::CTLongDouble_blr(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
    iParamCnt=0;
	SetTestStepName(aStepName);		
	}

TVerdict CTLongDouble_blr::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTLongDouble_blr::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTLongDouble_blr::doTestStepL()
	{
	int err;
	if(TestStepName() == Kcbrt_test)
		{
   		INFO_PRINTF1(_L("cbrtl_test():"));
   		err = cbrtl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kceil_test)
		{
   		INFO_PRINTF1(_L("ceill_test():"));
   		err = ceill_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kerf_test)
		{
   		INFO_PRINTF1(_L("erfl_test():"));
   		err = erfl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kerfc_test)
		{
   		INFO_PRINTF1(_L("erfcl_test():"));
   		err = erfcl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kexp_test)
		{
   		INFO_PRINTF1(_L("expl_test():"));
   		err = expl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kexp2_test)
		{
   		INFO_PRINTF1(_L("exp2l_test():"));
   		err = exp2l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kexpm1_test)
		{
   		INFO_PRINTF1(_L("expm1l_test():"));
   		err = expm1l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kfabs_test)
		{
   		INFO_PRINTF1(_L("fabsl_test():"));
   		err = fabsl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kilogb_test)
		{
   		INFO_PRINTF1(_L("ilogbl_test():"));
   		err = ilogbl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
  	else if(TestStepName() == Kj0_test)
		{
   		INFO_PRINTF1(_L("j0l_test():"));
   		err = j0l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kj1_test)
		{
   		INFO_PRINTF1(_L("j1l_test():"));
   		err = j1l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Klrint_test)
		{
   		INFO_PRINTF1(_L("lrintl_test():"));
   		err = lrintl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kllrint_test)
		{
   		INFO_PRINTF1(_L("llrintl_test():"));
   		err = llrintl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Klog_test)
		{
   		INFO_PRINTF1(_L("logl_test():"));
   		err = logl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Klog10_test)
		{
   		INFO_PRINTF1(_L("log10l_test():"));
   		err = log10l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Klog1p_test)
		{
   		INFO_PRINTF1(_L("log1pl_test():"));
   		err = log1pl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Klogb_test)
		{
   		INFO_PRINTF1(_L("logbl_test():"));
   		err = logbl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Klround_test)
		{
   		INFO_PRINTF1(_L("lroundl_test():"));
   		err = lroundl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kllround_test)
		{
   		INFO_PRINTF1(_L("llroundl_test():"));
   		err = llroundl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Krint_test)
		{
   		INFO_PRINTF1(_L("rintl_test():"));
   		err = rintl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Ksqrt_test)
		{
   		INFO_PRINTF1(_L("sqrtl_test():"));
   		err = sqrtl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Ktrunc_test)
		{
   		INFO_PRINTF1(_L("truncl_test():"));
   		err = truncl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Ky0_test)
		{
   		INFO_PRINTF1(_L("y0l_test():"));
   		err = y0l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	
   	else if(TestStepName() == Ky1_test)
		{
   		INFO_PRINTF1(_L("y1l_test():"));
   		err = y1l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfloor_test)
		{
   		INFO_PRINTF1(_L("floorl_test():"));
   		err = floorl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Ksignificand_test)
		{
   		INFO_PRINTF1(_L("significandl_test():"));
   		err = significandl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Knearbyint_test)
		{
   		INFO_PRINTF1(_L("nearbyintl_test():"));
   		err = nearbyintl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kisinf_test)
		{
   		INFO_PRINTF1(_L("isinfl_test():"));
   		err = isinfl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Kisnan_test)
		{
   		INFO_PRINTF1(_L("isnanl_test():"));
   		err = isnanl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfdim_test)
		{
   		INFO_PRINTF1(_L("fdiml_test():"));
   		err = fdiml_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kfmax_test)
		{
   		INFO_PRINTF1(_L("fmaxl_test():"));
   		err = fmaxl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kfmin_test)
		{
   		INFO_PRINTF1(_L("fminl_test():"));
   		err = fminl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfmod_test)
		{
   		INFO_PRINTF1(_L("fmodl_test():"));
   		err = fmodl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Khypot_test)
		{
   		INFO_PRINTF1(_L("hypotl_test():"));
   		err = hypotl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kremainder_test)
		{
   		INFO_PRINTF1(_L("remainderl_test():"));
   		err = remainderl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Knexttoward_test)
		{
   		INFO_PRINTF1(_L("nexttowardl_test():"));
   		err = nexttowardl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Knextafter_test)
		{
   		INFO_PRINTF1(_L("nextafterl_test():"));
   		err = nextafterl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kcopysign_test)
		{
   		INFO_PRINTF1(_L("copysignl_test():"));
   		err = copysignl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfjn_test)
		{
   		INFO_PRINTF1(_L("jnl_test():"));
   		err = jnl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Kfyn_test)
		{
   		INFO_PRINTF1(_L("ynl_test():"));
   		err = ynl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kscalb_test)
		{
   		INFO_PRINTF1(_L("scalbl_test():"));
   		err = scalbl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kscalbn_test)
		{
   		INFO_PRINTF1(_L("scalbnl_test():"));
   		err = scalbnl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kpow_test)
		{
   		INFO_PRINTF1(_L("powl_test():"));
   		err = powl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kacos_test)
		{
   		INFO_PRINTF1(_L("acosl_test():"));
   		err = acosl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kacosh_test)
		{
   		INFO_PRINTF1(_L("acoshl_test():"));
   		err = acoshl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
    else if(TestStepName() == Kasin_test)
		{
   		INFO_PRINTF1(_L("asinl_test():"));
   		err = asinl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kasinh_test)
		{
   		INFO_PRINTF1(_L("asinhl_test():"));
   		err = asinhl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Katan_test)
		{
   		INFO_PRINTF1(_L("atanl_test():"));
   		err = atanl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Katanh_test)
		{
   		INFO_PRINTF1(_L("atanhl_test():"));
   		err = atanhl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == KCos_test)
		{
   		INFO_PRINTF1(_L("Cosl_test():"));
   		err = Cosl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kcosh_test)
		{
   		INFO_PRINTF1(_L("coshl_test():"));
   		err = coshl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Ksin_test)
		{
   		INFO_PRINTF1(_L("sinl_test():"));
   		err = sinl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Ksinh_test)
		{
   		INFO_PRINTF1(_L("sinhl_test():"));
   		err = sinhl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Ktan_test)
		{
   		INFO_PRINTF1(_L("tanl_test():"));
   		err = tanl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Ktanh_test)
		{
   		INFO_PRINTF1(_L("tanhl_test():"));
   		err = tanhl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Katan2_test)
		{
   		INFO_PRINTF1(_L("atan2l_test():"));
   		err = atan2l_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kisnormal_test)
		{
   		INFO_PRINTF1(_L("isnormall_test():"));
   		err = isnormall_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Ksignbit_test)
		{
   		INFO_PRINTF1(_L("signbitl_test():"));
   		err = signbitl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kscalbln_test)
		{
   		INFO_PRINTF1(_L("scalblnl_test():"));
   		err = scalblnl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfinite_test)
		{
   		INFO_PRINTF1(_L("finitel_test():"));
   		err = finitel_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kmodf_test)
		{
   		INFO_PRINTF1(_L("modfl_test():"));
   		err = modfl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kldexp_test)
		{
   		INFO_PRINTF1(_L("ldexpl_test():"));
   		err = ldexpl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kdrem_test)
		{
   		INFO_PRINTF1(_L("dreml_test():"));
   		err = dreml_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfrexp_test)
		{
   		INFO_PRINTF1(_L("frexpl_test():"));
   		err = frexpl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}  
   	else if(TestStepName() == Kremquo_test)
		{
   		INFO_PRINTF1(_L("remquol_test():"));
   		err = remquol_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kremquo_remcheck_test)
		{
   		INFO_PRINTF1(_L("remquo_remcheckl_test():"));
   		err = remquo_remcheckl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kgamma_test)
		{
   		INFO_PRINTF1(_L("gammal_test():"));
   		err = gammal_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Klgamma_test)
		{
   		INFO_PRINTF1(_L("lgammal_test():"));
   		err = lgammal_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		} 
   	else if(TestStepName() == Kfpclassify_test)
		{
   		INFO_PRINTF1(_L("fpclassifyl_test():"));
   		err = fpclassifyl_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kisfinite_test)
		{
   		INFO_PRINTF1(_L("isfinitel_test():"));
   		err = isfinitel_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	return TestStepResult(); 
	}


// -----------------------------------------------------------------------------
// Utility functions for testing.
// -----------------------------------------------------------------------------

void CTLongDouble_blr::ReadIntParam(TInt &aInt)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(Kparam,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	}
	
void CTLongDouble_blr::ReadFloatParam(FLOAT &aDbl)
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
	
	
void CTLongDouble_blr::ReadStringParam(char* aString)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<100> pNameBuf;
  	TPtrC descriptor;
  	TInt i;
	
	pNameBuf.Format(Kparam,++iParamCnt);
	TBool ret = GetStringFromConfig(ConfigSection(), pNameBuf, descriptor);
	
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
      		aString[i] = '\0';
    	}
        
	}

void CTLongDouble_blr::ReadLIntParam(TInt32 &aLInt)
	{
	char Str[MAX_SIZE];
	TInt32 d;
	ReadStringParam( Str);
	/* Strtol converts the initial part of the
	string into long integer value according to the 
	give base [Third arg]*/
	 
	d = strtol(Str,NULL, 10);
	aLInt = d; 
	}
	
void CTLongDouble_blr::ReadLLIntParam(TInt64 &aLInt)
	{
	char Str[MAX_SIZE];
	TInt64 d; 							/*long long int*/
	ReadStringParam( Str);
	/* Strtoll converts the initial part of the
	string into long long integer value according to the 
	give base [Third arg]*/
	
	d = strtoll(Str,NULL, 10);
	aLInt = d; 
	}	
// -----------------------------------------------------------------------------
// Test methods
// -----------------------------------------------------------------------------
//                  
int CTLongDouble_blr::check_bool (int computed, int expected)
	{
	int ok = 0;
	
	if ((computed == 0) == (expected == 0))
		{
		ok = 1;
		}
	return ok;
	}

int CTLongDouble_blr::check_int (int computed, int expected,  int max_ulp)
	{
  	int diff = computed - expected;
  	int ok = 0;

  	if (abs (diff) <= max_ulp)
  		{
		ok = 1;
		}
  	return ok;
	}

int  CTLongDouble_blr::check_float (FLOAT computed, FLOAT expected,FLOAT max_ulp, FLOAT &gen_ulp)
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
		// Test for sign of infinities.  
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
	// Don't calc ulp for NaNs or infinities.  
	else if (isinf (computed) || isnan (computed) || isinf (expected) || isnan (expected))
  		{
  		ok = 0;
  		}
	else
		{
		diff = FUNC(fabs) (computed - expected);
		// ilogb (0) isn't allowed.  
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

int CTLongDouble_blr::check_longlong (TInt64 computed, TInt64 expected,  FLOAT max_ulp, FLOAT &gen_ulp)
	{
  	long long int diff = computed - expected;
  	int ok = 0;
  
  
  	if ( (gen_ulp = llabs (diff)) <= max_ulp)
		{
		ok = 1;
		}
  	return ok;
	}

int CTLongDouble_blr::check_long (TInt32 computed, TInt32 expected,  FLOAT max_ulp, FLOAT &gen_ulp)
	{
	long int diff = computed - expected;
  	int ok = 0;

  	if ((gen_ulp =labs (diff)) <= max_ulp)
  		{
		ok = 1;
		}
	return ok;
	}
	
TInt CTLongDouble_blr::cbrtl_test()
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
    FLOAT res = FUNC(cbrt) (input);
        
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



TInt CTLongDouble_blr::ceill_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}


TInt CTLongDouble_blr::erfl_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTLongDouble_blr::erfcl_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTLongDouble_blr::expl_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTLongDouble_blr::exp2l_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}

TInt CTLongDouble_blr::expm1l_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    
    return KErrNone;
	}

TInt CTLongDouble_blr::fabsl_test()
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
    FLOAT res = FUNC(fabs) (input);
        
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


TInt CTLongDouble_blr::ilogbl_test()
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


TInt CTLongDouble_blr::j0l_test()
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


TInt CTLongDouble_blr::j1l_test()
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

TInt CTLongDouble_blr::fpclassifyl_test()
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
    int res = fpclassify(input);
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

TInt CTLongDouble_blr::lrintl_test()
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
    buf.Append(chParam[0]) ; 
    buf.Append(chParam[1]) ;
    buf.Append(chParam[2]) ;
    buf.Append(chParam[3]) ;
    buf.Append(chParam[4]) ;
    buf.Append(chParam[5]) ;
    buf.Append(chParam[6]) ;
    // Do some testing
    TInt32 res = FUNC(lrint) (input);
        
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

TInt CTLongDouble_blr::llrintl_test()
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

TInt CTLongDouble_blr::logl_test()
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
    FLOAT res = FUNC(log) (input);
        
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


TInt CTLongDouble_blr::log10l_test()
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

TInt CTLongDouble_blr::log1pl_test()
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
    FLOAT res = FUNC(log1p) (input);
        
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

TInt CTLongDouble_blr::logbl_test()
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
    FLOAT res = FUNC(logb) (input);
        
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


TInt CTLongDouble_blr::roundl_test()
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

TInt CTLongDouble_blr::lroundl_test()
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
    TInt32 res = FUNC(lround) (input);
        
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
     
    return KErrNone;
	}


TInt CTLongDouble_blr::llroundl_test()
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
    INFO_PRINTF2(_L("Expected Value : %d\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}

TInt CTLongDouble_blr::rintl_test()
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

TInt CTLongDouble_blr::sqrtl_test()
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

TInt CTLongDouble_blr::truncl_test()
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

TInt CTLongDouble_blr::y0l_test()
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

TInt CTLongDouble_blr::y1l_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf);
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}

TInt CTLongDouble_blr::significandl_test()
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

TInt CTLongDouble_blr::floorl_test()
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

TInt CTLongDouble_blr::nearbyintl_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Max ULP value  : %f\n"), max_ulp );
	INFO_PRINTF2(_L("Result  		  : %f\n"), res );
	INFO_PRINTF2(_L("Generated Ulp  : %f\n"), gen_ulp );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}

TInt CTLongDouble_blr::isinfl_test()
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
    int res = __isinf(input);
    
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

TInt CTLongDouble_blr::isnanl_test()
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
    int res = isnan(input);
        
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

TInt CTLongDouble_blr::finitel_test()
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
    INFO_PRINTF2(_L("TestCase		  : %S\n"), &buf );
    INFO_PRINTF2(_L("Input Value    : %f\n"), input );
    INFO_PRINTF2(_L("Expected Value : %f\n"), expected );
	INFO_PRINTF2(_L("Result  		  : %d\n"), res );
	INFO_PRINTF1(_L("_________________________________________\n"));
    return KErrNone;
	}
	
TInt CTLongDouble_blr::isfinitel_test()
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


TInt CTLongDouble_blr::isnormall_test()
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
    int res = isnormal(input);
        
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


TInt CTLongDouble_blr::signbitl_test()
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
    int res = signbit (input);
        
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
