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



#ifndef __TLDOUBLE_BLR_H__
#define __TLDOUBLE_BLR_H__

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
#elif TESTING_LDOUBLE
#define FUNC(function) function##l
#define FLOAT long double
#define CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat) Clongdouble
#else 
#define FUNC(function) function
#define FLOAT double
#define CHOOSE(Clongdouble,Cdouble,Cfloat,Cinlinelongdouble,Cinlinedouble,Cinlinefloat) Cdouble
#endif //TESTING_FLOAT

#define IGNORE_ZERO_INF_SIGN		0x10

#define MANT_DIG CHOOSE ((LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1),  \
                         (LDBL_MANT_DIG-1), (DBL_MANT_DIG-1), (FLT_MANT_DIG-1))

_LIT(Kcbrt_test, "cbrtl_test");
_LIT(Kceil_test, "ceill_test");
_LIT(Kerf_test, "erfl_test");
_LIT(Kerfc_test, "erfcl_test");
_LIT(Kexp_test, "expl_test");
_LIT(Kexp2_test, "exp2l_test");
_LIT(Kexpm1_test, "expm1l_test");
_LIT(Kfabs_test, "fabsl_test");
_LIT(Kilogb_test, "ilogbl_test");
_LIT(Kj0_test, "j0l_test");
_LIT(Kj1_test, "j1l_test");
_LIT(Klrint_test, "lrintl_test");
_LIT(Kllrint_test, "llrintl_test");
_LIT(Kfpclassify_test, "fpclassifyl_test");
_LIT(Klog_test, "logl_test");
_LIT(Klog10_test, "log10l_test");
_LIT(Klog1p_test, "log1pl_test");
_LIT(Klogb_test, "logbl_test");
_LIT(Kround_test, "roundl_test");
_LIT(Klround_test, "lroundl_test");
_LIT(Kllround_test, "llroundl_test");
_LIT(Krint_test, "rintl_test");
_LIT(Ksqrt_test, "sqrtl_test");
_LIT(Ktrunc_test, "truncl_test");
_LIT(Ky0_test, "y0l_test");
_LIT(Ky1_test, "y1l_test");
_LIT(Kfloor_test, "floorl_test");
_LIT(Ksignificand_test, "significandl_test");
_LIT(Knearbyint_test, "nearbyintl_test");
_LIT(Kisinf_test, "isinfl_test");
_LIT(Kisnan_test, "isnanl_test");
_LIT(Kfdim_test, "fdiml_test");
_LIT(Kfmax_test, "fmaxl_test");
_LIT(Kfmin_test, "fminl_test");
_LIT(Kfmod_test, "fmodl_test");
_LIT(Khypot_test, "hypotl_test");
_LIT(Kremainder_test, "remainderl_test");
_LIT(Knexttoward_test, "nexttowardl_test");
_LIT(Knextafter_test, "nextafterl_test");
_LIT(Kcopysign_test, "copysignl_test");
_LIT(Kfjn_test, "jnl_test");
_LIT(Kfyn_test, "ynl_test");
_LIT(Kscalb_test, "scalbl_test");
_LIT(Kscalbn_test, "scalbnl_test");
_LIT(Kpow_test, "powl_test");
_LIT(Kacos_test, "acosl_test");
_LIT(Kacosh_test, "acoshl_test");
_LIT(Kasin_test, "asinl_test");
_LIT(Kasinh_test, "asinhl_test");
_LIT(Katan_test, "atanl_test");
_LIT(Katanh_test, "atanhl_test");
_LIT(KCos_test, "Cosl_test");
_LIT(Kcosh_test, "coshl_test");
_LIT(Ksin_test, "sinl_test");
_LIT(Ksinh_test, "sinhl_test");
_LIT(Ktan_test, "tanl_test");
_LIT(Ktanh_test, "tanhl_test");
_LIT(Katan2_test, "atan2l_test");
_LIT(Kfma_test, "fmal_test");
_LIT(Kisfinite_test, "isfinitel_test");
_LIT(Kisnormal_test, "isnormall_test");
_LIT(Ksignbit_test, "signbitl_test");
_LIT(Kscalbln_test, "scalblnl_test");
_LIT(Kfinite_test, "finitel_test");
_LIT(Kmodf_test, "modfl_test");
_LIT(Kldexp_test, "ldexpl_test");
_LIT(Kdrem_test, "dreml_test");
_LIT(Kfrexp_test, "frexpl_test");
_LIT(Kremquo_test, "remquol_test");
_LIT(Kremquo_remcheck_test, "remquo_remcheckl_test");
_LIT(Kgamma_test, "gammal_test");
_LIT(Klgamma_test, "lgammal_test");

class CTLongDouble_blr : public CTestStep
{
public:
	~CTLongDouble_blr (); 
	CTLongDouble_blr (const TDesC& aStepName);
	TInt iParamCnt;
	TInt iStrCnt;
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:

    /**
    * C++ default constructor.
    */
    CTLongDouble_blr();
        
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
    
    TInt cbrtl_test();
    TInt ceill_test();
    TInt erfl_test();
	TInt erfcl_test();
	TInt expl_test();
	TInt exp2l_test();
	TInt expm1l_test();
	TInt fabsl_test();
	TInt ilogbl_test();
	TInt j0l_test();
	TInt j1l_test();
	TInt lrintl_test();
	TInt llrintl_test();
	TInt fpclassifyl_test();
    TInt logl_test();
	TInt log10l_test();
	TInt log1pl_test();
	TInt logbl_test();
	TInt roundl_test();
	TInt lroundl_test();
	TInt llroundl_test();
	TInt rintl_test();
	TInt sqrtl_test();
	TInt truncl_test();
	TInt y0l_test();
	TInt y1l_test();
	TInt floorl_test();
	TInt significandl_test();
	TInt nearbyintl_test();
	TInt isinfl_test();
	TInt isnanl_test();
	TInt acosl_test();
    TInt acoshl_test();
    TInt asinl_test();
	TInt asinhl_test();
	TInt Cosl_test();
    TInt atanl_test();
	TInt atanhl_test();
	TInt coshl_test();
	TInt sinl_test();
	TInt sinhl_test();
	TInt tanl_test();
	TInt tanhl_test();
	TInt isfinitel_test();
	TInt isnormall_test();
	TInt signbitl_test();
	TInt finitel_test();
	TInt modfl_test();
	// Apis which take two arguments
	TInt fdiml_test();
	TInt fmaxl_test();
	TInt fminl_test();
	TInt fmodl_test();
	TInt hypotl_test();
	TInt remainderl_test();
	TInt nexttowardl_test();
	TInt nextafterl_test();
	TInt copysignl_test();
	TInt jnl_test();
	TInt ynl_test();
	TInt scalbl_test();
	TInt scalbnl_test();
	TInt scalblnl_test();
	TInt powl_test();
	TInt atan2l_test();
	TInt ldexpl_test();
	TInt dreml_test();
	TInt frexpl_test();
	TInt remquol_test();
	TInt remquo_remcheckl_test();
	TInt fmal_test();
	TInt gammal_test();
	TInt lgammal_test();
       

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TDOUBLE_BLR_H
            
// End of File