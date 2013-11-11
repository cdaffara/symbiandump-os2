// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// LMATH.CPP - Wrappers for Epoc32 Math functions
// 
//


#include <e32math.h>
#include "FDLIBM.H"
#include <errno.h>

extern "C" {

/**
 * Map the Symbian error value to the standard error number and put it into errno.
 * @internalComponent 
 * @param aSymbianReturnValue the standard return value returned from the Symbian function.
 * @param aNan the result of Math::Nan
 */ 
void MapSymbianErrorCodeToErrno(int aSymbianReturnValue, int aNan)
{

	switch(aSymbianReturnValue)
	{
	case (KErrNone):
		{
		//errno does not get set to 0 according to the standard.  
		//The user must initialize it before calling the function.
		break;
		}
	case (KErrArgument):
		{
		if (aNan)
			errno = EDOM;
		else
			errno = EINVAL;
		break;
		}
	case (KErrOverflow):
	case (KErrUnderflow):
	case (KErrTotalLossOfPrecision):
		{
		errno = ERANGE;
		break;
		}
	}

}

/**
Calculate arctangent.
Performs the trigonometric arctangent operation on x and returns an angle in the range from -PI/2 to PI/2 expressed in radians.  
@return Arctangent of arg1.
@param arg1 Value whose arctangent has to be calculated.
*/
EXPORT_C double atan (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::ATan(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate cosine.
Performs the trigonometric cosine operation on x returning a value between -1 and 1.  
@return Cosine of arg1.
@param arg1 Angle expressed in radians (180 degrees = PI radians).
*/
EXPORT_C double cos (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Cos(result, arg1);
	MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate sine.
Performs the trigonometric sine operation on x returning a value between -1 and 1.  
@return Sine of arg1.
@param arg1 Angle expressed in radians (180 degrees = PI radians).
*/
EXPORT_C double sin (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Sin(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate tangent.
Performs the trigonometric tangent operation on arg1.  
@return Tangent of arg1.
@param arg1 Angle expressed in radians (180 degrees = PI radians).
*/
EXPORT_C double tan (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Tan(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate arccosine.
Performs the trigonometric arc cosine operation on x and returns an angle in the range from 0 to PI expressed in radians.  
@return Arc cosine of arg1.
@param arg1 Value between -1 and 1 whose arc cosine has to be calculated.
*/
EXPORT_C double acos (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::ACos(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate arcsine.
Performs the trigonometric arc sine operation on x and returns an angle in the range from -PI/2 to PI/2 expressed in radians.  
@return Arc sine of arg1
@param arg1 Value between -1 and 1 whose arc sine has to be calculated.
*/
EXPORT_C double asin (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::ASin(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate exponential.
Returns the exponential value of parameter arg1.  
@return Exponential of arg1
@param arg1 Floating point value.
*/
EXPORT_C double exp (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Exp(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate natural logarithm.  
Returns the natural logarithm of parameter arg1
@return Logarithm of arg1.
@param arg1 Floating point value.
*/
EXPORT_C double log (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Ln(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate logarithm base 10.  
Returns the logarithm base 10 of parameter arg1 
@return Logarithm base 10 of arg1
@param arg1 Floating point value.
*/
EXPORT_C double log10 (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Log(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate square root.  
Returns the square root of parameter arg1.
@return Square root of arg1
@param arg1 Non-negative floating point value.
*/
EXPORT_C double sqrt (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Sqrt(result, arg1);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate arctangent, 2 parameters.
Performs the trigonometric arctangent operation on y/x and returns an angle in the range from -PI to PI expressed in radians, using the signs of the parameters to determine the quadrant.
The result is valid even if arg2 is 0 (angle is PI/2 or -PI/2).
In fact this function returns the angle of bidimensional vector (arg2,arg1).
@return Arctangent of arg1/arg2.
@param arg1 and arg2.Values from whose division has to be calculated the arctangent.i.e.: Coordinates for the vector whose angle is to be calculated.
*/
EXPORT_C double atan2 (double arg1, double arg2) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::ATan(result, arg1, arg2);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Calculate numeric power.
Returns arg1 raised to the power of arg2.   
@return   arg1 raised to the power of arg2.
@param arg1 - Base value. 
@param arg2 - Exponent value.
*/
EXPORT_C double pow (double arg1, double arg2) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Pow(result, arg1, arg2);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Return remainder of floating point division.
Performs division arg1/arg2 and returns the remainder of the operation.  
@return Remainder of arg1/arg2.
@param arg1 and arg2 - Floating point values
*/
EXPORT_C double fmod (double arg1, double arg2) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Mod(result, arg1, arg2);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
Round to integral value in floating-point format
@return the integral value (represented as a double precision number) nearest to arg1 according to the prevailing rounding mode.
@param arg1 floating point value to round.
*/
EXPORT_C double rint (double arg1) __SOFTFP
    {
    double result;
    int    returnValue;

    returnValue = Math::Round(result, arg1, 0);
    MapSymbianErrorCodeToErrno(returnValue, Math::IsNaN(result));
    
    return result;
    }

/**
tests whether d is NaN
@return non-zero if d is NaN. Otherwise, 0 is returned.
@param d floating point value.
*/
EXPORT_C int isnan (double d) __SOFTFP
	{
	return Math::IsNaN(d);
	}

/**
test for infinity or not-a-number
@return 1 if the number d is Infinity, otherwise 0.
@param d floating point value to test.
*/
EXPORT_C int isinf (double d) __SOFTFP
	{
	return Math::IsInfinite(d);
	}

/**
Test for finity.
@return a nonzero value if the x parameter is a finite number;
that is, if d is not +-, INF, NaNQ, or NaNS.
@param d floating point value to test.
*/
EXPORT_C int finite (double d) __SOFTFP
	{
	return Math::IsFinite(d);
	}

}	// end of extern "C"



