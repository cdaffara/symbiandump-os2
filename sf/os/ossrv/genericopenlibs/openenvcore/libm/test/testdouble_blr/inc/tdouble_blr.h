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



#ifndef __TDOUBLE_BLR_H__
#define __TDOUBLE_BLR_H__

// INCLUDES
#include <e32math.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <test/testexecutestepbase.h>



#define MAX_SIZE 50

#ifdef TESTING_FLOAT
#define FUNC(function) function##f
#define FLOAT float
#define CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat) Cfloat
#else
#define FUNC(function) function
#define FLOAT double
#define CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat) Cdouble
#endif //TESTING_FLOAT

#define IGNORE_ZERO_INF_SIGN		0x10

#define MANT_DIG CHOOSE ((LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1),  \
                         (LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1))

_LIT(Kcbrt_test, "cbrt_test");
_LIT(Kceil_test, "ceil_test");
_LIT(Kerf_test, "erf_test");
_LIT(Kerfc_test, "erfc_test");
_LIT(Kexp_test, "exp_test");
_LIT(Kexp2_test, "exp2_test");
_LIT(Kexpm1_test, "expm1_test");
_LIT(Kfabs_test, "fabs_test");
_LIT(Kilogb_test, "ilogb_test");
_LIT(Kj0_test, "j0_test");
_LIT(Kj1_test, "j1_test");
_LIT(Klrint_test, "lrint_test");
_LIT(Kllrint_test, "llrint_test");
_LIT(Kfpclassify_test, "fpclassify_test");
_LIT(Klog_test, "log_test");
_LIT(Klog10_test, "log10_test");
_LIT(Klog1p_test, "log1p_test");
_LIT(Klogb_test, "logb_test");
_LIT(Kround_test, "round_test");
_LIT(Klround_test, "lround_test");
_LIT(Kllround_test, "llround_test");
_LIT(Krint_test, "rint_test");
_LIT(Ksqrt_test, "sqrt_test");
_LIT(Ktrunc_test, "trunc_test");
_LIT(Ky0_test, "y0_test");
_LIT(Ky1_test, "y1_test");
_LIT(Kfloor_test, "floor_test");
_LIT(Ksignificand_test, "significand_test");
_LIT(Knearbyint_test, "nearbyint_test");
_LIT(Kisinf_test, "isinf_test");
_LIT(Kisnan_test, "isnan_test");
_LIT(Kfdim_test, "fdim_test");
_LIT(Kfmax_test, "fmax_test");
_LIT(Kfmin_test, "fmin_test");
_LIT(Kfmod_test, "fmod_test");
_LIT(Khypot_test, "hypot_test");
_LIT(Kremainder_test, "remainder_test");
_LIT(Knexttoward_test, "nexttoward_test");
_LIT(Knextafter_test, "nextafter_test");
_LIT(Kcopysign_test, "copysign_test");
_LIT(Kfjn_test, "jn_test");
_LIT(Kfyn_test, "yn_test");
_LIT(Kscalb_test, "scalb_test");
_LIT(Kscalbn_test, "scalbn_test");
_LIT(Kpow_test, "pow_test");
_LIT(Kacos_test, "acos_test");
_LIT(Kacosh_test, "acosh_test");
_LIT(Kasin_test, "asin_test");
_LIT(Kasinh_test, "asinh_test");
_LIT(Katan_test, "atan_test");
_LIT(Katanh_test, "atanh_test");
_LIT(KCos_test, "Cos_test");
_LIT(Kcosh_test, "cosh_test");
_LIT(Ksin_test, "sin_test");
_LIT(Ksinh_test, "sinh_test");
_LIT(Ktan_test, "tan_test");
_LIT(Ktanh_test, "tanh_test");
_LIT(Katan2_test, "atan2_test");
_LIT(Kfma_test, "fma_test");
_LIT(Kisfinite_test, "isfinite_test");
_LIT(Kisnormal_test, "isnormal_test");
_LIT(Ksignbit_test, "signbit_test");
_LIT(Kscalbln_test, "scalbln_test");
_LIT(Kfinite_test, "finite_test");
_LIT(Kmodf_test, "modf_test");
_LIT(Kldexp_test, "ldexp_test");
_LIT(Kdrem_test, "drem_test");
_LIT(Kfrexp_test, "frexp_test");
_LIT(Kremquo_test, "remquo_test");
_LIT(Kremquo_remcheck_test, "remquo_remcheck_test");
_LIT(Kgamma_test, "gamma_test");
_LIT(Klgamma_test, "lgamma_test");

class CTestDouble_blr : public CTestStep
{
public:
	~CTestDouble_blr (); 
	CTestDouble_blr (const TDesC& aStepName);
	TInt iParamCnt;
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:

    /**
    * C++ default constructor.
    */
    CTestDouble_blr();
        
    // Utility functions for handling parameters etc.
    void ReadIntParam(TInt &aInt);
    void ReadStringParam(char* aString);
    void ReadFloatParam(FLOAT &aDbl);
	void ReadLIntParam(TInt32 &aLInt);
	void ReadLLIntParam(TInt64 &aLInt);
	
    //Test Functions
    int check_float (FLOAT computed, FLOAT expected, FLOAT max_ulp, FLOAT &gen_ulp); 
    int check_longlong (TInt64 computed, TInt64 expected,  FLOAT max_ulp, FLOAT &gen_ulp);
    int check_int (int computed, int expected,  int max_ulp);
    int check_long (TInt32 computed, TInt32 expected,  FLOAT max_ulp, FLOAT &gen_ulp);
    int check_bool (int computed, int expected);
    
    TInt cbrt_test();
    TInt ceil_test();
    TInt erf_test();
	TInt erfc_test();
	TInt exp_test();
	TInt exp2_test();
	TInt expm1_test();
	TInt fabs_test();
	TInt ilogb_test();
	TInt j0_test();
	TInt j1_test();
	TInt lrint_test();
	TInt llrint_test();
	TInt fpclassify_test();
    TInt log_test();
	TInt log10_test();
	TInt log1p_test();
	TInt logb_test();
	TInt round_test();
	TInt lround_test();
	TInt llround_test();
	TInt rint_test();
	TInt sqrt_test();
	TInt trunc_test();
	TInt y0_test();
	TInt y1_test();
	TInt floor_test();
	TInt significand_test();
	TInt nearbyint_test();
	TInt isinf_test();
	TInt isnan_test();
	TInt acos_test();
    TInt acosh_test();
    TInt asin_test();
	TInt asinh_test();
	TInt Cos_test();
    TInt atan_test();
	TInt atanh_test();
	TInt cosh_test();
	TInt sin_test();
	TInt sinh_test();
	TInt tan_test();
	TInt tanh_test();
	TInt isfinite_test();
	TInt isnormal_test();
	TInt signbit_test();
	TInt finite_test();
	TInt modf_test();
	// Apis which take two arguments
	TInt fdim_test();
	TInt fmax_test();
	TInt fmin_test();
	TInt fmod_test();
	TInt hypot_test();
	TInt remainder_test();
	TInt nexttoward_test();
	TInt nextafter_test();
	TInt copysign_test();
	TInt jn_test();
	TInt yn_test();
	TInt scalb_test();
	TInt scalbn_test();
	TInt scalbln_test();
	TInt pow_test();
	TInt atan2_test();
	TInt ldexp_test();
	TInt drem_test();
	TInt frexp_test();
	TInt remquo_test();
	TInt remquo_remcheck_test();
	TInt fma_test();
	TInt gamma_test();
	TInt lgamma_test();
       

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TDOUBLE_BLR_H
            
// End of File