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
* Constants related to Symbian sources
*
*/
package com.nokia.tracecompiler.source;

import com.nokia.tracecompiler.model.TraceParameter;

/**
 * Constants related to Symbian sources
 * 
 */
public interface SymbianConstants {

	/**
	 * Include directory
	 */
	final String INCLUDE_DIRECTORY = "inc"; //$NON-NLS-1$

	/**
	 * Source directory
	 */
	final String SOURCE_DIRECTORY = "src"; //$NON-NLS-1$

	/**
	 * Group directory
	 */
	final String GROUP_DIRECTORY = "group"; //$NON-NLS-1$

	/**
	 * Mmpfiles directory
	 */
	final String MMPFILES_DIRECTORY = "mmpfiles"; //$NON-NLS-1$
	
	/**
	 * ETrue
	 */
	final String ETRUE = "ETrue"; //$NON-NLS-1$

	/**
	 * EFalse
	 */
	final String EFALSE = "EFalse"; //$NON-NLS-1$

	/**
	 * TBool
	 */
	final String TBOOL = "TBool"; //$NON-NLS-1$

	/**
	 * TInt
	 */
	final String TINT = "TInt"; //$NON-NLS-1$

	/**
	 * TUint
	 */
	final String TUINT = "TUint"; //$NON-NLS-1$

	/**
	 * TInt64
	 */
	final String TINT64 = "TInt64"; //$NON-NLS-1$

	/**
	 * TUint64
	 */
	final String TUINT64 = "TUint64"; //$NON-NLS-1$

	/**
	 * TTime
	 */
	final String TTIME = "TTime"; //$NON-NLS-1$

	/**
	 * TInt32
	 */
	final String TINT32 = "TInt32"; //$NON-NLS-1$

	/**
	 * TUint32
	 */
	final String TUINT32 = "TUint32"; //$NON-NLS-1$

	/**
	 * TInt16
	 */
	final String TINT16 = "TInt16"; //$NON-NLS-1$

	/**
	 * TUint16
	 */
	final String TUINT16 = "TUint16"; //$NON-NLS-1$

	/**
	 * TInt8
	 */
	final String TINT8 = "TInt8"; //$NON-NLS-1$

	/**
	 * TUint8
	 */
	final String TUINT8 = "TUint8"; //$NON-NLS-1$

	/**
	 * TAny
	 */
	final String TANY = "TAny"; //$NON-NLS-1$

	/**
	 * TAny*
	 */
	final String TANY_PTR = "TAny*"; //$NON-NLS-1$
	
	/**
	 * const TAny*
	 */
	final String CONST_TANY_PTR = "const TAny*"; //$NON-NLS-1$

	/**
	 * TDes8
	 */
	final String TDES8 = "TDes8"; //$NON-NLS-1$

	/**
	 * TDesC8
	 */
	final String TDESC8 = "TDesC8"; //$NON-NLS-1$

	/**
	 * TDes16
	 */
	final String TDES16 = "TDes16"; //$NON-NLS-1$

	/**
	 * TDesC16
	 */
	final String TDESC16 = "TDesC16"; //$NON-NLS-1$

	/**
	 * TDes
	 */
	final String TDES = "TDes"; //$NON-NLS-1$

	/**
	 * TDesC
	 */
	final String TDESC = "TDesC"; //$NON-NLS-1$

	/**
	 * TPtr8
	 */
	final String TPTR8 = "TPtr8"; //$NON-NLS-1$

	/**
	 * TPtrC8
	 */
	final String TPTRC8 = "TPtrC8"; //$NON-NLS-1$

	/**
	 * TPtr16
	 */
	final String TPTR16 = "TPtr16"; //$NON-NLS-1$

	/**
	 * TPtrC16
	 */
	final String TPTRC16 = "TPtrC16"; //$NON-NLS-1$

	/**
	 * TPtr
	 */
	final String TPTR = "TPtr"; //$NON-NLS-1$

	/**
	 * TPtrC
	 */
	final String TPTRC = "TPtrC"; //$NON-NLS-1$

	/**
	 * TReal
	 */
	final String TREAL = "TReal"; //$NON-NLS-1$

	/**
	 * const TDesC8&
	 */
	final String CONST_TDESC8_REF = "const TDesC8&"; //$NON-NLS-1$

	/**
	 * const TDesC8&
	 */
	final String CONST_TDESC16_REF = "const TDesC16&"; //$NON-NLS-1$

	/**
	 * Prefix for all parameters generated into header files
	 */
	final String PARAMETER_DECLARATION_PREFIX = "aParam"; //$NON-NLS-1$

	/**
	 * __KERNEL_MODE__
	 */
	final String KERNEL_MODE = "__KERNEL_MODE__"; //$NON-NLS-1$

	/**
	 * Symbian parameter types mapped to TraceParameter types
	 */
	final String PARAMETER_TYPE_MAP[][] = { { TANY, null },
			{ SourceConstants.VOID, null }, { TINT, TraceParameter.SDEC32 },
			{ TINT32, TraceParameter.SDEC32 },
			{ TBOOL, TraceParameter.SDEC32 },
			{ SourceConstants.INT, TraceParameter.SDEC32 },
			{ SourceConstants.LONG, TraceParameter.SDEC32 },
			{ TUINT, TraceParameter.UDEC32 },
			{ TUINT32, TraceParameter.UDEC32 },
			{ TINT16, TraceParameter.SDEC16 },
			{ SourceConstants.SHORT, TraceParameter.SDEC16 },
			{ TUINT16, TraceParameter.UDEC16 },
			{ TINT8, TraceParameter.SDEC8 },
			{ SourceConstants.CHAR, TraceParameter.SDEC8 },
			{ TUINT8, TraceParameter.UDEC8 },
			{ TINT64, TraceParameter.SDEC64 },
			{ TTIME, TraceParameter.TIME },
			{ TUINT64, TraceParameter.UDEC64 },
			{ TREAL, TraceParameter.FLOAT_FIX } };

}
