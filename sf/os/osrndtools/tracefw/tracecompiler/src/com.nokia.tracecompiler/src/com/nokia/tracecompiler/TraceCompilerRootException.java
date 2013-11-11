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
 * Command-line based view implementation
 *
 */
package com.nokia.tracecompiler;


public class TraceCompilerRootException extends Exception {
	// Serial version UID, as required for all Exceptions
	private static final long serialVersionUID = 2126996710246333574L;
	

	/**
	 * Constructs a TraceCompilerRootException with the given detail message and
	 * cause.
	 * @param message the detail message
	 * @param cause the Exception that caused this Exception
	 */
	public TraceCompilerRootException(String message, Throwable cause)
	{
		super(message, cause);
	}
}
