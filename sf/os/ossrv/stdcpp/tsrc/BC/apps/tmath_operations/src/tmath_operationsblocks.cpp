/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <numeric>
#include <math.h>
#include <stdlib.h>
#include <fenv.h>
//#include <stdio.h>
#include "tmath_operations.h"

using namespace std;

#define PI 3.14159265
int flags_result(int flags);


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctmath_operations::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctmath_operations::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctmath_operations::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        
        ENTRY( "Accumulate", Ctmath_operations::Accumulate ),
        ENTRY( "Inner_Product", Ctmath_operations::Inner_Product ),
        ENTRY( "Partial_Sum", Ctmath_operations::Partial_Sum ),
        ENTRY( "Adjacent_Difference", Ctmath_operations::Adjacent_Difference ),
        ENTRY( "Power", Ctmath_operations::Power ),
        ENTRY( "Iota", Ctmath_operations::Iota ),
        
        ENTRY( "Cos_float", Ctmath_operations::Cos_float ),
        ENTRY( "Cos_double", Ctmath_operations::Cos_double ),
        ENTRY( "Cos_longdouble", Ctmath_operations::Cos_longdouble ),
        ENTRY( "Sin_float", Ctmath_operations::Sin_float ),
        ENTRY( "Sin_double", Ctmath_operations::Sin_double ),
        ENTRY( "Sin_longdouble", Ctmath_operations::Sin_longdouble ),
        ENTRY( "Tan_float", Ctmath_operations::Tan_float ),
        ENTRY( "Tan_double", Ctmath_operations::Tan_double ),
        ENTRY( "Tan_longdouble", Ctmath_operations::Tan_longdouble ),
        ENTRY( "Pow_float", Ctmath_operations::Pow_float ),
        ENTRY( "Pow_double", Ctmath_operations::Pow_double ),
        ENTRY( "Pow_longdouble", Ctmath_operations::Pow_longdouble ),
        ENTRY( "Cosh_float", Ctmath_operations::Cosh_float ),
        ENTRY( "Cosh_double", Ctmath_operations::Cosh_double ),
        ENTRY( "Cosh_longdouble", Ctmath_operations::Cosh_longdouble ),
        ENTRY( "Sinh_float", Ctmath_operations::Sinh_float ),
        ENTRY( "Sinh_double", Ctmath_operations::Sinh_double ),
        ENTRY( "Sinh_longdouble", Ctmath_operations::Sinh_longdouble ),
        ENTRY( "Tanh_float", Ctmath_operations::Tanh_float ),
        ENTRY( "Tanh_double", Ctmath_operations::Tanh_double ),
        ENTRY( "Tanh_longdouble", Ctmath_operations::Tanh_longdouble ),
        ENTRY( "Sqrt_float", Ctmath_operations::Sqrt_float ),
        ENTRY( "Sqrt_double", Ctmath_operations::Sqrt_double ),
        ENTRY( "Sqrt_longdouble", Ctmath_operations::Sqrt_longdouble ),
        ENTRY( "Log_float", Ctmath_operations::Log_float ),
        ENTRY( "Log_double", Ctmath_operations::Log_double ),
        ENTRY( "Log_longdouble", Ctmath_operations::Log_longdouble ),
        ENTRY( "Log10_float", Ctmath_operations::Log10_float ),
        ENTRY( "Log10_double", Ctmath_operations::Log10_double ),
        ENTRY( "Log10_longdouble", Ctmath_operations::Log10_longdouble ),
        ENTRY( "Exp_float", Ctmath_operations::Exp_float ),
        ENTRY( "Exp_double", Ctmath_operations::Exp_double ),
        ENTRY( "Exp_longdouble", Ctmath_operations::Exp_longdouble ),
        ENTRY( "Fabs_float", Ctmath_operations::Fabs_float ),
        ENTRY( "Fabs_double", Ctmath_operations::Fabs_double ),
        ENTRY( "Fabs_longdouble", Ctmath_operations::Fabs_longdouble ),
        ENTRY( "Acos_float", Ctmath_operations::Acos_float ),
        ENTRY( "Acos_double", Ctmath_operations::Acos_double ),
        ENTRY( "Acos_longdouble", Ctmath_operations::Acos_longdouble ),
        ENTRY( "Asin_float", Ctmath_operations::Asin_float ),
        ENTRY( "Asin_double", Ctmath_operations::Asin_double ),
        ENTRY( "Asin_longdouble", Ctmath_operations::Asin_longdouble ),
        ENTRY( "Atan_float", Ctmath_operations::Atan_float ),
        ENTRY( "Atan_double", Ctmath_operations::Atan_double ),
        ENTRY( "Atan_longdouble", Ctmath_operations::Atan_longdouble ),
        ENTRY( "Atan2_float", Ctmath_operations::Atan2_float ),
        ENTRY( "Atan2_double", Ctmath_operations::Atan2_double ),
        ENTRY( "Atan2_longdouble", Ctmath_operations::Atan2_longdouble ),
        ENTRY( "Ceil_float", Ctmath_operations::Ceil_float ),
        ENTRY( "Ceil_double", Ctmath_operations::Ceil_double ),
        ENTRY( "Ceil_longdouble", Ctmath_operations::Ceil_longdouble ),
        ENTRY( "Floor_float", Ctmath_operations::Floor_float ),
        ENTRY( "Floor_double", Ctmath_operations::Floor_double ),
        ENTRY( "Floor_longdouble", Ctmath_operations::Floor_longdouble ),
        ENTRY( "Fmod_float", Ctmath_operations::Fmod_float ),
        ENTRY( "Fmod_double", Ctmath_operations::Fmod_double ),
        ENTRY( "Fmod_longdouble", Ctmath_operations::Fmod_longdouble ),        
        ENTRY( "Modf_double", Ctmath_operations::Modf_double ),
        ENTRY( "Frexp_float", Ctmath_operations::Frexp_float ),
        ENTRY( "Frexp_double", Ctmath_operations::Frexp_double ),
        ENTRY( "Frexp_longdouble", Ctmath_operations::Frexp_longdouble ),
        ENTRY( "Ldexp_float", Ctmath_operations::Ldexp_float ),
        ENTRY( "Ldexp_double", Ctmath_operations::Ldexp_double ),
        ENTRY( "Ldexp_longdouble", Ctmath_operations::Ldexp_longdouble ),
        
        ENTRY( "Cbrt", Ctmath_operations::Cbrt ),
        ENTRY( "Acosh", Ctmath_operations::Acosh ),
        ENTRY( "Asinh", Ctmath_operations::Asinh ),
        ENTRY( "Atanh", Ctmath_operations::Atanh ),
        ENTRY( "Erf", Ctmath_operations::Erf ),
        ENTRY( "Lgamma", Ctmath_operations::Lgamma ), 
		ENTRY( "Acosf", Ctmath_operations::Acosf ),
        ENTRY( "Asinf", Ctmath_operations::Asinf ),
        ENTRY( "Atanf", Ctmath_operations::Atanf ),
        ENTRY( "Atan2f", Ctmath_operations::Atan2f ),
        ENTRY( "Cosf", Ctmath_operations::Cosf ),
        ENTRY( "Sinf", Ctmath_operations::Sinf ),
        ENTRY( "Tanf", Ctmath_operations::Tanf ),
        ENTRY( "Coshf", Ctmath_operations::Coshf ),
        ENTRY( "Sinhf", Ctmath_operations::Sinhf ),
        ENTRY( "Tanhf", Ctmath_operations::Tanhf ),        
        ENTRY( "Ldexpf", Ctmath_operations::Ldexpf ),
        ENTRY( "Log10f", Ctmath_operations::Log10f ),
        ENTRY( "Logf", Ctmath_operations::Logf ),
        ENTRY( "Modff", Ctmath_operations::Modff ),
        ENTRY( "Powf", Ctmath_operations::Powf ),
        ENTRY( "Sqrtf", Ctmath_operations::Sqrtf ),
        ENTRY( "Ceilf", Ctmath_operations::Ceilf ),
        ENTRY( "Fabsf", Ctmath_operations::Fabsf ),
        ENTRY( "Frexpf", Ctmath_operations::Frexpf ),
        ENTRY( "Expf", Ctmath_operations::Expf ),               
        ENTRY( "Fmodf", Ctmath_operations::Fmodf ),
        ENTRY( "Roundf", Ctmath_operations::Roundf ),
        ENTRY( "LRound", Ctmath_operations::LRound ),
        ENTRY( "LLRound", Ctmath_operations::LLRound ),
        ENTRY( "Round", Ctmath_operations::Round ),
        ENTRY( "Erff", Ctmath_operations::Erff ),
        ENTRY( "Erfcf", Ctmath_operations::Erfcf ),
        ENTRY( "Hypotf", Ctmath_operations::Hypotf ),
        ENTRY( "Hypot", Ctmath_operations::Hypot ),
        ENTRY( "Lgammaf", Ctmath_operations::Lgammaf ),
        ENTRY( "Acoshf", Ctmath_operations::Acoshf ),
        ENTRY( "Asinhf", Ctmath_operations::Asinhf ),
        ENTRY( "Atanhf", Ctmath_operations::Atanhf ),
        ENTRY( "Cbrtf", Ctmath_operations::Cbrtf ),
        ENTRY( "Logbf", Ctmath_operations::Logbf ),
        ENTRY( "Copysignf", Ctmath_operations::Copysignf ),           
        ENTRY( "Nextafterf", Ctmath_operations::Nextafterf ),
        ENTRY( "Remainderf", Ctmath_operations::Remainderf ),
        ENTRY( "Rintf", Ctmath_operations::Rintf ),
        ENTRY( "Fmaxf", Ctmath_operations::Fmaxf ),
        ENTRY( "Fminf", Ctmath_operations::Fminf ),   
        ENTRY( "Nearbyintf", Ctmath_operations::Nearbyintf ),
        ENTRY( "Truncf", Ctmath_operations::Truncf ),        
        ENTRY( "Fmaf", Ctmath_operations::Fmaf ),
        ENTRY( "Fma", Ctmath_operations::Fma ),
        ENTRY( "Exp2_float", Ctmath_operations::Exp2_float ),
        ENTRY( "Exp2_double", Ctmath_operations::Exp2_double ),
        ENTRY( "Expm1f_float", Ctmath_operations::Expm1f_float ),
        ENTRY( "Expm1_double", Ctmath_operations::Expm1_double ),
        ENTRY( "Fdim_double", Ctmath_operations::Fdim_double ),
        ENTRY( "Fdimf_float", Ctmath_operations::Fdimf_float ),
        ENTRY( "Fdiml_longdouble", Ctmath_operations::Fdiml_longdouble ),
        ENTRY( "Abs_float", Ctmath_operations::Abs_float ),
        ENTRY( "Abs_double", Ctmath_operations::Abs_double ),
        ENTRY( "Abs_longdouble", Ctmath_operations::Abs_longdouble ),
        ENTRY( "Scalbf", Ctmath_operations::Scalbf ),
        ENTRY( "Significandf", Ctmath_operations::Significandf ),
        ENTRY( "Fegetexceptflag", Ctmath_operations::Fegetexceptflag ),
        ENTRY( "Feraiseexcept", Ctmath_operations::Feraiseexcept ),
        ENTRY( "Feclearexcept", Ctmath_operations::Feclearexcept),
        ENTRY( "Fesetexceptflag", Ctmath_operations::Fesetexceptflag),
        
        ENTRY( "Isnan", Ctmath_operations::Isnan ), 
        ENTRY( "Signbit", Ctmath_operations::Signbit ),
        ENTRY( "Isnormal", Ctmath_operations::Isnormal ),  
        ENTRY( "Isinf", Ctmath_operations::Isinf ),
        ENTRY( "Isfinite", Ctmath_operations::Isfinite ),        
        ENTRY( "Fpclassifyd", Ctmath_operations::Fpclassifyd ),
        ENTRY( "Nanval", Ctmath_operations::Nanval ),
        ENTRY( "Infinite", Ctmath_operations::Infinite ),

        
        
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctmath_operations::Accumulate
// Accumulate test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctmath_operations::Accumulate( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	
	int ac1 , ac2 ;
	int A1[] = {1, 2, 3, 4, 5};
    const int N1 = sizeof(A1) / sizeof(int);

    ac1 = accumulate(A1, A1 + N1, 0);
    ac2 = accumulate(A1, A1 + N1,1, multiplies<int>());
    
    __UHEAP_MARKEND;
    
    if( (ac1 == 15) && (ac2 == 120) )
      return KErrNone;
    else
      return KErrGeneral;  

    }

// -----------------------------------------------------------------------------
// Ctmath_operations::Inner_Product
// Inner_Product test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::Inner_Product( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	
	int ip1, ip2;
	int A1[] = {1, 2, 3, 4, 5};
    int A2[] = {1, 4, 9, 16, 25};
    const int N1 = sizeof(A1) / sizeof(int);

    ip1 = inner_product(A1, A1 + N1, A2, 0);
    ip2 = inner_product(A1, A1 + N1, A2,1,multiplies<int>(),plus<int>());
    
    __UHEAP_MARKEND;
    
    if( (ip1 == 225) && (ip2 == 86400) )
      return KErrNone;
    else
      return KErrGeneral;  

    }


// -----------------------------------------------------------------------------
// Ctmath_operations::Partial_Sum
// Partial_Sum test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::Partial_Sum( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	
	int A1[] = {1, 2, 3, 4, 5};
	int A2[6] , p1 ,p2 ;

    const int N1 = sizeof(A1) / sizeof(int);

    partial_sum(A1, A1 + N1, A2);
    
    for( int i = 0; i < N1; i++ )
	  {
	   p1 = A2[i];
	  }

    partial_sum(A1, A1 + N1, A2, multiplies<int>());
	
	for( int i = 0; i < N1; i++ )
	  {
	   p2 = A2[i];
	  }
	
    __UHEAP_MARKEND;
    
    if( (p1 == 15) && (p2 == 120) )
      return KErrNone;
    else
      return KErrGeneral;  

    }


// -----------------------------------------------------------------------------
// Ctmath_operations::Adjacent_Difference
// Adjacent_Difference test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::Adjacent_Difference( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	
	int A1[] = {1, 2, 3, 4, 5};
	int A2[6] , p1 ,p2 ;

    const int N1 = sizeof(A1) / sizeof(int);

    adjacent_difference(A1, A1 + N1, A2);
    
    for( int i = 0; i < N1; i++ )
	  {
	   p1 = A2[i];
	  }

    adjacent_difference(A1, A1 + N1, A2, multiplies<int>());
	
	for( int i = 0; i < N1; i++ )
	  {
	   p2 = A2[i];
	  }
	
    __UHEAP_MARKEND;
    
    if( (p1 == 1) && (p2 == 20) )
      return KErrNone;
    else
      return KErrGeneral;  

    }
    
    
// -----------------------------------------------------------------------------
// Ctmath_operations::Power
// Power test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::Power( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	int p1, p2;

    p1 = power(2,5);  
	
    __UHEAP_MARKEND;
    
    if( p1 == 32 )
      return KErrNone;
    else
      return KErrGeneral;  

    }


// -----------------------------------------------------------------------------
// Ctmath_operations::Iota
// Iota test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctmath_operations::Iota( CStifItemParser& aItem )
    {
	
	__UHEAP_MARK;
	int p1, p2;
	int A2[6];

    iota(A2,A2+5,10); 
    
    for( int i = 0; i < 5; i++ )
	  {
	   p1 = A2[i];
	  }   
	
    __UHEAP_MARKEND;
    
    if( p1 == 14 )
      return KErrNone;
    else
      return KErrGeneral;  

    }
    
// -----------------------------------------------------------------------------
// Ctmath_operations::Basic_math_apis
// Basic_math_apis test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctmath_operations::Cos_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	param = 60.0;
  	expected = 0.500000 ;
  	result = cos(param*PI/180);
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Cos_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	param = 60.0;
  	expected = 0.500000 ;
  	result = cos(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Cos_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	param = 60.0;
  	expected = 0.500000 ;
  	result = cos(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Sin_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	param = 30.0;
  	expected = 0.500000 ;
  	result = sin(param*PI/180);
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Sin_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	param = 30.0;
  	expected = 0.500000 ;
  	result = sin(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Sin_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	param = 30.0;
  	expected = 0.500000 ;
  	result = sin(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Tan_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	param = 45.0;
  	expected = 1.000000 ;
  	result = tan(param*PI/180);
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Tan_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	param = 45.0;
  	expected = 1.000000 ;
  	result = tan(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Tan_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	param = 45.0;
  	expected = 1.000000 ;
  	result = tan(param*PI/180);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Pow_float( CStifItemParser& aItem )
{	
    float param1, param2, result1 ,result2 ,expected;
    int param3 = 7;
    
  	param1 = 3;
  	param2 = 7;
  	expected = 2187.000000;

  	result1 = pow( param1 , param2 );
  	result2 = pow( param1 , param3 );
  	
  	if( result1 == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Pow_double( CStifItemParser& aItem )
{	
    double param1, param2, result1 ,result2 ;
    float expected;
    int param3 = 7;
      
  	param1 = 3;
  	param2 = 7;
  	expected = 2187.000000;

  	result1 = pow( param1 , param2 );
  	result2 = pow( param1 , param3 );
  	
  	if( (float)result1 == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Pow_longdouble( CStifItemParser& aItem )
{	
    long double param1, param2 ,result1 ,result2 ;
    float expected;
    int param3 = 7;
      
  	param1 = 3;
  	param2 = 7;
  	expected = 2187.000000;

  	result1 = pow( param1 , param2 );
  	result2 = pow( param1 , param3 );
  	
  	if( (float)result1 == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Cosh_float( CStifItemParser& aItem )
{	
	float param, result ,expected;
	param = log(2.0);
	result = cosh (param);
	expected = 1.250000 ;
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Cosh_double( CStifItemParser& aItem )
{	
	double param, result;
	float expected;
	
	param = log(2.0);
	result = cosh (param);
	expected = 1.250000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Cosh_longdouble( CStifItemParser& aItem )
{	
	long double param, result;
	float expected;
	
	param = log(2.0);
	result = cosh (param);
	expected = 1.250000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sinh_float( CStifItemParser& aItem )
{	
	float param, result,expected;
	param = log(2.0);
	result = sinh (param);
	expected = 0.750000 ;
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sinh_double( CStifItemParser& aItem )
{	
	double param, result;
	float expected;
	
	param = log(2.0);
	result = sinh (param);
	expected = 0.750000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sinh_longdouble( CStifItemParser& aItem )
{	
	long double param, result;
	float expected;
	
	param = log(2.0);
	result = sinh (param);
	expected = 0.750000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Tanh_float( CStifItemParser& aItem )
{	
	float param, result,expected;
	param = log(2.0);
	result = tanh (param);
	expected = 0.600000 ;
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Tanh_double( CStifItemParser& aItem )
{	
	double param, result;
	float expected;
	
	param = log(2.0);
	result = tanh (param);
	expected = 0.600000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Tanh_longdouble( CStifItemParser& aItem )
{	
	long double param, result;
	float expected;
	
	param = log(2.0);
	result = tanh (param);
	expected = 0.600000 ;
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sqrt_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = 1024.0;
  	expected = 32.000000 ; 
  	result = sqrt (param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sqrt_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = 1024.0;
  	expected = 32.000000 ; 
  	result = sqrt (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Sqrt_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = 1024.0;
  	expected = 32.000000 ; 
  	result = sqrt (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = 5.5;
  	expected = 1.704748 ;
  	result = log (param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = 5.5;
  	expected = 1.704748 ; 
  	result = log (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = 5.5;
  	expected = 1.704748 ; 
  	result = log (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log10_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = 1000.0;
  	expected = 3.000000 ;
  	result = log10 (param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log10_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = 1000.0;
  	expected = 3.000000 ; 
  	result = log10 (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Log10_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = 1000.0;
  	expected = 3.000000 ; 
  	result = log10 (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Exp_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = 5.0;
  	expected = 148.413159 ; 
  	result = exp (param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Exp_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = 5.0;
  	expected = 148.413159 ; 
  	result = exp (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Exp_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = 5.0;
  	expected = 148.413159 ; 
  	result = exp (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Fabs_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = fabs (param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Fabs_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = fabs (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Fabs_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = fabs (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Acos_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	expected = 60.000000;
  	param = 0.5 ;
  	result = acos(param)* 180.0 / PI;
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Acos_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	expected = 60.000000;
  	param = 0.5 ;
  	result = acos(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Acos_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	expected = 60.000000;
  	param = 0.5 ;
  	result = acos(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Asin_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	expected = 30.000000;
  	param = 0.5 ;
  	result = asin(param)* 180.0 / PI;
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Asin_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	expected = 30.000000;
  	param = 0.5 ;
  	result = asin(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Asin_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	expected = 30.000000;
  	param = 0.5 ;
  	result = asin(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan_float( CStifItemParser& aItem )
{	
    float param, result ,expected;
    
  	expected = 45.000000;
  	param = 1.0 ;
  	result = atan(param)* 180.0 / PI;
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan_double( CStifItemParser& aItem )
{	
    double param, result ;
    float expected;
      
  	expected = 45.000000;
  	param = 1.0 ;
  	result = atan(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan_longdouble( CStifItemParser& aItem )
{	
    long double param, result ;
    float expected;
      
  	expected = 45.000000;
  	param = 1.0 ;
  	result = atan(param)* 180.0 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan2_float( CStifItemParser& aItem )
{	
    float x,y, result ,expected;
    
  	expected = 135.000000;
  	x = -10.0 ;
  	y = 10.0 ;
  	result = atan2 (y,x) * 180 / PI;
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan2_double( CStifItemParser& aItem )
{	
    double x,y, result ;
    float expected;
    
  	expected = 135.000000;
  	x = -10.0 ;
  	y = 10.0 ;
  	result = atan2 (y,x) * 180 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan2_longdouble( CStifItemParser& aItem )
{	
    long double x,y, result ;
    float expected;
      
  	expected = 135.000000;
  	x = -10.0 ;
  	y = 10.0 ;
  	result = atan2 (y,x) * 180 / PI;
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ceil_float( CStifItemParser& aItem )
{	
    float x, result ,expected;
      
  	expected = -2.000000;
  	x = -2.3 ;
  	result = ceil(x);
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ceil_double( CStifItemParser& aItem )
{	
    double x, result ;
    float expected;
      
  	expected = -2.000000;
  	x = -2.3 ;
  	result = ceil(x);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ceil_longdouble( CStifItemParser& aItem )
{	
    long double x, result ;
    float expected;
      
  	expected = -2.000000;
  	x = -2.3 ;
  	result = ceil(x);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Floor_float( CStifItemParser& aItem )
{	
    float x, result ,expected;
      
  	expected = -3.000000;
  	x = -2.3 ;
  	result = floor(x);
  	
  	if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Floor_double( CStifItemParser& aItem )
{	
    double x, result ;
    float expected;
      
  	expected = -3.000000;
  	x = -2.3 ;
  	result = floor(x);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Floor_longdouble( CStifItemParser& aItem )
{	
    long double x, result ;
    float expected;
      
  	expected = -3.000000;
  	x = -2.3 ;
  	result = floor(x);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Fmod_double( CStifItemParser& aItem )
{	
    double x, y, result ;
    float expected;
      
  	expected = 1.300000;
  	x = 5.3 ;
  	y = 2.0 ;
  	result = fmod(x,y);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Fmod_longdouble( CStifItemParser& aItem )
{	
    long double x, y, result ;
    float expected;
      
  	expected = 1.300000;
  	x = 5.3 ;
  	y = 2.0 ;
  	result = fmod(x,y);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Modf_double( CStifItemParser& aItem )
{	
  	double param, result, intpart;
  	float expected;

  	param = 3.141592 ;
  	expected = 0.141592 ;
  	result = modf (param , &intpart);
  	
  	if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Frexp_float( CStifItemParser& aItem )
{
  	float param, result, expected ;
  	int n;

  	param = 15.2;
  	expected = 0.950000 ;
  	result = frexp (param , &n);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Frexp_double( CStifItemParser& aItem )
{
  	double param, result;
  	float expected ;
  	int n;

  	param = 15.2;
  	expected = 0.950000 ;
  	result = frexp (param , &n);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Frexp_longdouble( CStifItemParser& aItem )
{
  	long double param, result;
  	float expected ;
  	int n;

  	param = 15.2;
  	expected = 0.950000 ;
  	result = frexp (param , &n);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ldexp_float( CStifItemParser& aItem )
{
  	float param, result , expected ;
  	int n;

  	param = 0.95;
  	expected = 15.200000 ;
  	n = 4;
  	result = ldexp (param , n);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ldexp_double( CStifItemParser& aItem )
{
  	double param, result;
  	float expected ;
  	int n;

  	param = 0.95;
  	expected = 15.200000 ;
  	n = 4;
  	result = ldexp (param , n);
 
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ldexp_longdouble( CStifItemParser& aItem )
{
  	double param, result;
  	float expected ;
  	int n;

  	param = 0.95;
  	expected = 15.200000 ;
  	n = 4;
  	result = ldexp (param , n);
 
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Cbrt( CStifItemParser& aItem)
{
    float a, b ,expected;    
    a = 27.0; 
    expected = 3.000000;   
    b = cbrt(a);

   if( b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}        

TInt Ctmath_operations::Acosh( CStifItemParser& aItem)
{
    float a, b;
    float expected;
    
    a = 1.5;
	expected = 0.9624236 ;    
    b = acosh(a);

   if( b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 

} 



TInt Ctmath_operations::Erf( CStifItemParser& aItem)
{	
   double smallx = 0.1, value;
   float expected = 0.1124627224223869;
 
   value = erf(smallx);

   return KErrNone;

}  

TInt Ctmath_operations::Lgamma( CStifItemParser& aItem)
{
    double a, b;
    float expected = 0.5723649 ; 
    a = 0.5;    
    b = lgammaf(a);
    
   if( (float)b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}  

TInt Ctmath_operations::Acosf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	expected = 60.000000;
  	param = 0.5 ;
  	result = acosf(param)* 180.0 / PI;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Asinf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	expected = 30.000000;
  	param = 0.5 ;
  	result = asinf(param)* 180.0 / PI;
 
  	return KErrNone;
}

TInt Ctmath_operations::Atanf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	expected = 45.000000;
  	param = 1.0 ;
  	result = atanf(param)* 180.0 / PI;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Atan2f( CStifItemParser& aItem )
{
    float x,y, result ,expected;
    
  	expected = 135.000000;
  	x = -10.0 ;
  	y = 10.0 ;
  	result = atan2f(y,x) * 180 / PI;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}



TInt Ctmath_operations::Sinf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	param = 30.0;
  	expected = 0.500000 ;
  	result = sinf(param*PI/180);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Tanf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	param = 45.0;
  	expected = 1.000000 ;
  	result = tanf(param*PI/180);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Coshf( CStifItemParser& aItem )
{
	float param, result ,expected;
	
	param = log(2.0);
	result = coshf(param);
	expected = 1.250000 ;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Sinhf( CStifItemParser& aItem )
{
	float param, result,expected;
	
	param = log(2.0);
	result = sinhf(param);
	expected = 0.750000 ;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Tanhf( CStifItemParser& aItem )
{
	float param, result,expected;
	
	param = log(2.0);
	result = tanhf(param);
	expected = 0.600000 ;
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ldexpf( CStifItemParser& aItem )
{
  	float param, result , expected ;
  	int n;

  	param = 0.95;
  	expected = 15.200000 ;
  	n = 4;
  	result = ldexpf(param , n);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Log10f( CStifItemParser& aItem )
{
  	float param, result , expected;
  	
  	param = 1000.0;
  	expected = 3.000000 ;
  	result = log10f(param);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Logf( CStifItemParser& aItem )
{
  	float param, result , expected;
  	
  	param = 5.5;
  	expected = 1.704748 ;
  	result = logf (param);
 
  	return KErrNone;
}



TInt Ctmath_operations::Powf( CStifItemParser& aItem )
{
    float param1, param2, result1 ,expected;
    
  	param1 = 3;
  	param2 = 7;
  	expected = 2187.000000;

  	result1 = powf( param1 , param2 );
 
    if( result1 == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Sqrtf( CStifItemParser& aItem )
{
  	float param, result , expected;
  	
  	param = 1024.0;
  	expected = 32.000000 ; 
  	result = sqrtf (param);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Ceilf( CStifItemParser& aItem )
{
    float x, result ,expected;
      
  	expected = -2.000000;
  	x = -2.3 ;
  	result = ceilf(x);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Fabsf( CStifItemParser& aItem )
{
  	float param, result , expected;
  	
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = fabsf (param);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Frexpf( CStifItemParser& aItem )
{
  	float param, result, expected ;
  	int n;

  	param = 15.2;
  	expected = 0.950000 ;
  	result = frexpf (param , &n);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Expf( CStifItemParser& aItem )
{
  	float param, result , expected;
  	
  	param = 5.0;
  	expected = 148.413159 ; 
  	result = expf (param);
 
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral; 
}
    
    
TInt Ctmath_operations::Fmodf( CStifItemParser& aItem)
{
   float w = -10.0, x = 3.0, z;
   float expected = -1.000000;

   z = fmodf( w, x );

   if( z == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}


TInt Ctmath_operations::Roundf( CStifItemParser& aItem)
{

  float a = 12;
  float expected =12.0;
  a+=0.1;
  a = roundf(a);

   if( a == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::LRound( CStifItemParser& aItem)
{

  double a = 12;
  long expected =12.0;
  a+=0.1;
  a = lround(a);

   if( (double)a == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::LLRound( CStifItemParser& aItem)
{

  double a = 12;
  long long expected =12.0;
  a+=0.1;
  a = llround(a);

   if( (double)a == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Round( CStifItemParser& aItem)
{

  double a = 12;
  double expected =12.0;
  a+=0.1;
  a = round(a);

   if( a == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}





TInt Ctmath_operations::Erfcf( CStifItemParser& aItem)
{

   float largex = 10.0;
   float expecterfcf = 0.0;
   float valueerfcf;     
 
   valueerfcf = erfcf(largex);        

   return KErrNone;
}

TInt Ctmath_operations::Hypotf( CStifItemParser& aItem)
{
   float x = 3.0, y = 4.0, result;
   float expected = 5.0;
   
   result = hypotf( x, y );

   if( result == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Hypot( CStifItemParser& aItem)
{
   double x = 3.0, y = 4.0, result;
   double expected = 5.0;
   
   result = hypot( x, y );

   if( result == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Lgammaf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.5723649; 
    a = 0.5;
    
    b = lgammaf(a);

   if( b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Acoshf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.9624236;
    
    a = 1.5;    
    b = acoshf(a);

  	return KErrNone;

}





TInt Ctmath_operations::Cbrtf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 3.0;
    a = 27.0;
    
    b = cbrtf(a);

   if( b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Logbf( CStifItemParser& aItem)
{
    float a, b;
    float expected=-1.0;
    a = 0.5;
    
    b = logbf(a);

   if( b == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Copysignf( CStifItemParser& aItem)
{
   float a, b, c;
   float expected=-27.0;

   a = 27.0;
   b = -5;
   c = copysignf(a, b);

   if( c == expected )
  	 return KErrNone;
   else
  	 return KErrGeneral; 
}                                              



TInt Ctmath_operations::Remainderf( CStifItemParser& aItem)
{
    float a, b , c;
    float expected = 0.500000 ; 
    a = 0.5; 
    b = 1.0;   
    c = remainderf(a , b);
    
    if( c == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}  

TInt Ctmath_operations::Rintf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.0 ; 
    a = 0.5;    
    b = rintf(a);
    
    if( b == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
} 

TInt Ctmath_operations::Fmaxf( CStifItemParser& aItem)
{
    float a, b , c;
    float expected = 1.0 ; 
    a = 0.5; 
    b = 1.0;   
    c = fmaxf(a , b);
    
    if( c == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
} 

TInt Ctmath_operations::Fminf( CStifItemParser& aItem)
{
    float a, b , c;
    float expected = 0.5 ; 
    a = 0.5; 
    b = 1.0;   
    c = fminf(a , b);
    
    if( c == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Nearbyintf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.0 ; 
    a = 0.5;    
    b = nearbyintf(a);
    
    if( b == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral;  
}

TInt Ctmath_operations::Truncf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.0 ; 
    a = 0.5;    
    b = truncf(a);
    
    if( b == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral;  
}

TInt Ctmath_operations::Fmaf( CStifItemParser& aItem)
{
    float a, b , c , res;
    float expected = 2.500000 ; 
    a = 0.5; 
    b = 1.0;
    c = 2.0;   
    res = fmaf(a , b , c);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Fma( CStifItemParser& aItem)
{
    double a, b , c , res;
    double expected = 2.500000 ; 
    a = 0.5; 
    b = 1.0;
    c = 2.0;   
    res = fma(a , b , c);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Exp2_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = 5.0;
  	expected = 32.00000; 
  	result = exp2 (param);
  
    if( result == expected || fabs(result-expected) < 0.5)
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Exp2_double( CStifItemParser& aItem )
{	
  	double param, result , expected;
  	param = 5.0;
  	expected = 32.00000000000000; 
  	result = exp2(param);
  
    if( result == expected || fabs(result-expected) < 0.5)
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}


TInt Ctmath_operations::Expm1f_float( CStifItemParser& aItem )
{	
  	float param,result;
  	double  expected;
  	param = 0.1;
  	expected = 0.1051709199466134; 
  	result = expm1f (param);
  
    if( result == (float)expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Expm1_double( CStifItemParser& aItem )
{	
  	double param, result , expected;
  	param = 0.01;
  	expected = 0.01005016708416806; 
  	result = expm1 (param);
  
    if( (float)result == (float)expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}


TInt Ctmath_operations::Fdim_double( CStifItemParser& aItem )
{	
  	double param1, param2, result , expected;
  	param1 = 1;
  	param2 = 2;
  	expected = 0.000000000000000; 
  	result = fdim (param1, param2);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Fdimf_float( CStifItemParser& aItem )
{	
  	float param1, param2, result , expected;
  	param1 = 1;
  	param2 = 2;
  	expected = 0.0; 
  	result = fdimf (param1, param2);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Fdiml_longdouble( CStifItemParser& aItem )
{	
  	long double param1, param2, result , expected;
  	param1 = 1;
  	param2 = 2;
  	expected = 0.000000000000000; 
  	result = fdiml (param1, param2);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Abs_float( CStifItemParser& aItem )
{	
  	float param, result , expected;
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = abs(param);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Abs_double( CStifItemParser& aItem )
{	
  	double param, result ;
  	float expected;
  	
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = abs (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Abs_longdouble( CStifItemParser& aItem )
{	
  	long double param, result ;
  	float expected;
  	
  	param = -5.0;
  	expected = 5.000000 ; 
  	result = abs (param);
  
    if( (float)result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}




TInt Ctmath_operations::Scalbf( CStifItemParser& aItem )
{	
  	float param1 =10.0;
  	float param2 = 2;
  	float expected = 40.000000;
  	float result; 
  	
  	result = scalbf(param1, param2);
  
    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;   
}

TInt Ctmath_operations::Significandf( CStifItemParser& aItem)
{
	float param, result;
	float expected = 1.250000;

    param = 5;
    result = significandf(param);



    if( result == expected )
  	 return KErrNone;
  	else
  	 return KErrGeneral;
}

TInt Ctmath_operations::Fegetexceptflag( CStifItemParser& aItem)
{
	fexcept_t flags;
  	int excepts, result;
  	int expect = 0;
  	fegetexceptflag(&flags, FE_ALL_EXCEPT);
  	result = flags_result(flags);
  	if (result == expect)
  	  return KErrNone;
  	else
  	 return KErrGeneral;
  
}

TInt Ctmath_operations::Feraiseexcept( CStifItemParser& aItem)
{
	fexcept_t flags;
  	int excepts, result;
  	int expect = 0;
  	
  	fegetexceptflag(&flags, FE_ALL_EXCEPT);
  	
  	feraiseexcept(FE_DIVBYZERO);
  	
  	excepts = fetestexcept(FE_DIVBYZERO | FE_INEXACT);
  	
  	result = flags_result(excepts);
  
  	if (result == expect)
  	  return KErrNone;
  	else
  	 return KErrGeneral;
  
}


TInt Ctmath_operations::Feclearexcept( CStifItemParser& aItem)
{
	fexcept_t flags;
  	int excepts, result;
  	int expect = 0;
  	
  	fegetexceptflag(&flags, FE_ALL_EXCEPT);
  	
  	feraiseexcept(FE_DIVBYZERO);
  	  	
  	feclearexcept(FE_ALL_EXCEPT);
  
    excepts = fetestexcept(FE_ALL_EXCEPT);
  	
  	result = flags_result(excepts);
  
  	if (result == expect)
  	  return KErrNone;
  	else
  	 return KErrGeneral;
  
}

TInt Ctmath_operations::Fesetexceptflag( CStifItemParser& aItem)
{
	fexcept_t flags;
  	int excepts, result;
  	int expect = 0;
  	
  	fegetexceptflag(&flags, FE_ALL_EXCEPT);
  	
  	feraiseexcept(FE_DIVBYZERO);
  	
  	/* restore original state of flags */
  	fesetexceptflag(&flags, FE_ALL_EXCEPT);
  
  	excepts = fetestexcept(FE_ALL_EXCEPT);
  
  	result = flags_result(excepts);
  
  	if (result == expect)
  	  return KErrNone;
  	else
  	 return KErrGeneral;
  
}



int flags_result(int flags)
{
  if (flags & FE_INEXACT)
    return 1;
  if (flags & FE_UNDERFLOW)
    return 1;
  if (flags & FE_OVERFLOW)
    return 1;
  if (flags & FE_DIVBYZERO)
    return 1;
  if (flags & FE_INVALID)
    return 1;
 
  if (!(flags & FE_ALL_EXCEPT))
    return 0;
}

TInt Ctmath_operations::Fmod_float( CStifItemParser& aItem )
{	
    float x, y, result ,expected;
      
  	expected = 1.300000;
  	x = 5.3 ;
  	y = 2.0 ;
  	result = fmod(x,y);
  	
    return KErrNone;
}

    
TInt Ctmath_operations::Asinh( CStifItemParser& aItem)
{
    float a, b;
    float expected;
    
    a = 1.5;
	expected =  1.194763 ;   
    b = asinh(a);

    return KErrNone;
}  

TInt Ctmath_operations::Atanh( CStifItemParser& aItem)
{
    float a, b;
    float expected;
    
    a = .1;
	expected = 0.1003354;    
    b = atanh(a);

    return KErrNone;

} 


TInt Ctmath_operations::Cosf( CStifItemParser& aItem )
{
    float param, result ,expected;
    
  	param = 60.0;
  	expected = 0.500000 ;
  	result = cosf(param*PI/180);
 
    return KErrNone;

}


TInt Ctmath_operations::Modff( CStifItemParser& aItem )
{
  	float param, result, intpart, expected;

  	param = 3.141592 ;
  	expected = 0.141592 ;
  	result = modff (param , &intpart);
 
    return KErrNone;

}

TInt Ctmath_operations::Erff( CStifItemParser& aItem)
{	
   float smallx = 0.1;
   float expecterff = 0.1124627;

   float valueerff;
 
   valueerff = erff(smallx);        

    return KErrNone;

}


TInt Ctmath_operations::Asinhf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 1.194763;
    a = 1.5;
    
    b = asinhf(a);

    return KErrNone;

}

TInt Ctmath_operations::Atanhf( CStifItemParser& aItem)
{
    float a, b;
    float expected = 0.5493062;
    a = .5;
    
    b = atanhf(a);

    return KErrNone;

}


TInt Ctmath_operations::Nextafterf( CStifItemParser& aItem)
{
    float a, b , c;
    float expected = 1.000000 ; 
    a = 1.0; 
    b = 10.0;   
    c = nextafterf(a , b);
    
    return KErrNone;

} 

TInt Ctmath_operations::Isnan( CStifItemParser& aItem)
{
    double a;
    bool res;
    bool expected = TRUE ; 
    
    a = 0.0/0.0;   
    res = __isnan(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Isfinite( CStifItemParser& aItem)
{
    double a = 3.0 ;
    bool res;
    bool expected = TRUE ; 
       
    res = __isfinite(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Isnormal( CStifItemParser& aItem)
{
    double a = 3.0;
    bool res;
    bool expected = TRUE ; 
       
    res = __isnormal(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Signbit( CStifItemParser& aItem)
{
    double a = -3.0 ;
    bool res;
    bool expected = TRUE ; 
     
    res = __signbit(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Isinf( CStifItemParser& aItem)
{
    double a = 3.0/0.0 ;
    bool res;
    bool expected = TRUE ; 
    
    res = __isinf(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Fpclassifyd( CStifItemParser& aItem)
{
    double a = 3.0 ;
    bool res;
    bool expected = TRUE ; 
    
    res = __fpclassifyd(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

TInt Ctmath_operations::Nanval( CStifItemParser& aItem)
{
    double res;      
    res = nanval();

  	return KErrNone;
} 

TInt Ctmath_operations::Infinite( CStifItemParser& aItem)
{
    double a = 3.0/0.0 ;
    bool res;
    bool expected = TRUE ; 
       
    res = __infinite(a);
    
    if( res == expected )
  	 return KErrNone;
    else
  	 return KErrGeneral; 
}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
