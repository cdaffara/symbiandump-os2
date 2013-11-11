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
 * TraceCompilerIllegalArgumentsException thrown when tracecompiler can not process its arguments
 *
 */
package com.nokia.tracecompiler;

/**
 * TraceCompilerIllegalArgumentsException thrown when tracecompiler can not process its arguments
 *
 */
public class TraceCompilerIllegalArgumentsException extends TraceCompilerRootException {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1387782275931306398L;

	public TraceCompilerIllegalArgumentsException(String message, Throwable cause) {
		super(message, cause);
	}

}
