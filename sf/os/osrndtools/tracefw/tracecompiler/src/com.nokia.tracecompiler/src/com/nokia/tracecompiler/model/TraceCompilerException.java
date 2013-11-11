/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Exceptions thrown by TraceCompiler engine
*
*/
package com.nokia.tracecompiler.model;

/**
 * Exceptions thrown by TraceCompiler engine
 * 
 */
public class TraceCompilerException extends Exception {

	/**
	 * Error codes for TraceCompiler exceptions
	 * 
	 */
	public interface TraceCompilerExceptionCode {
	}

	/**
	 * UID
	 */
	private static final long serialVersionUID = -2991616409482985157L; // CodForChk_Dis_Magic

	/**
	 * Error code
	 */
	private TraceCompilerExceptionCode errorCode;

	/**
	 * Error parameters
	 */
	private TraceCompilerErrorParameters parameters;

	/**
	 * Source object
	 */
	private Object source;

	/**
	 * Flag that defines will event related to exception posted to trace event view
	 */
	private boolean postEvent = true;
	
	/**
	 * Constructor with error code
	 * 
	 * @param errorCode
	 *            the error code
	 */
	public TraceCompilerException(TraceCompilerExceptionCode errorCode) {
		this.errorCode = errorCode;
	}
	
	/**
	 * Constructor with error code and postEvent flag
	 * 
	 * @param errorCode
	 *            the error code
	 * @param postEvent
	 *            flag that defines will event related to exception posted to trace event view   
	 */
	public TraceCompilerException(TraceCompilerExceptionCode errorCode, boolean postEvent) {
		this.errorCode = errorCode;
		this.postEvent = postEvent;
	}

	/**
	 * Constructor with error code and parameters
	 * 
	 * @param errorCode
	 *            the error code
	 * @param parameters
	 *            the error parameters
	 */
	public TraceCompilerException(TraceCompilerExceptionCode errorCode,
			TraceCompilerErrorParameters parameters) {
		this.errorCode = errorCode;
		this.parameters = parameters;
	}

	/**
	 * Constructor with error code, parameters and source object
	 * 
	 * @param errorCode
	 *            the error code
	 * @param parameters
	 *            the error parameters
	 * @param source
	 *            the source object
	 */
	public TraceCompilerException(TraceCompilerExceptionCode errorCode,
			TraceCompilerErrorParameters parameters, Object source) {
		this.errorCode = errorCode;
		this.parameters = parameters;
		this.source = source;
	}
	
	/**
	 * Constructor with error code and root cause
	 * 
	 * @param errorCode
	 *            the error code
	 * @param cause
	 *            the reason for this exception
	 */
	public TraceCompilerException(TraceCompilerExceptionCode errorCode,
			Throwable cause) {
		super(cause);
		this.errorCode = errorCode;
	}

	/**
	 * Gets the error code
	 * 
	 * @return error code
	 */
	public TraceCompilerExceptionCode getErrorCode() {
		return errorCode;
	}

	/**
	 * Gets the parameters related to the error
	 * 
	 * @return the parameters
	 */
	public TraceCompilerErrorParameters getErrorParameters() {
		return parameters;
	}

	/**
	 * Gets the source of this error
	 * 
	 * @return the source
	 */
	public Object getErrorSource() {
		return source;
	}

	/**
	 * Is event related to exception wanted to post to trace event view
	 * 
	 * @return true is event is wanted to post trace event view
	 *         false is event is not wanted to post trace event view
	 */
	public boolean isEventWantedToPost() {
		return postEvent;
	}
	
}
