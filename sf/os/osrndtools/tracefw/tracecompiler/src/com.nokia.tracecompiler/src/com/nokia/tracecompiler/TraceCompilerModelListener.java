/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Model extension listener for console UI
 *
 */
package com.nokia.tracecompiler;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceLocation;
import com.nokia.tracecompiler.engine.TraceLocationList;
import com.nokia.tracecompiler.engine.TraceLocationListListener;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceModelExtensionListener;
import com.nokia.tracecompiler.model.TraceModelListener;
import com.nokia.tracecompiler.model.TraceObject;
import com.nokia.tracecompiler.model.TraceProcessingListener;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceLocationListener;

/**
 * Model extension listener for TraceCompiler UI
 * 
 */
final class TraceCompilerModelListener implements TraceModelListener,
		TraceModelExtensionListener, TraceLocationListListener,
		SourceLocationListener, TraceProcessingListener {

	/**
	 * List of errors for a source
	 */
	private ArrayList<TraceLocation> errorList = new ArrayList<TraceLocation>();

	/**
	 * Name of the file being processed
	 */
	private String currentFileName;

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectAdded(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectAdded(TraceObject owner, TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectCreationComplete(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectCreationComplete(TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectRemoved(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectRemoved(TraceObject owner, TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * propertyUpdated(com.nokia.tracecompiler.model.TraceObject, int)
	 */
	public void propertyUpdated(TraceObject object, int property) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtensionListener#
	 * extensionAdded(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceModelExtension)
	 */
	public void extensionAdded(TraceObject object, TraceModelExtension extension) {
		if (extension instanceof TraceLocationList) {
			((TraceLocationList) extension).addLocationListListener(this);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtensionListener#
	 * extensionRemoved(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceModelExtension)
	 */
	public void extensionRemoved(TraceObject object,
			TraceModelExtension extension) {
		if (extension instanceof TraceLocationList) {
			((TraceLocationList) extension).removeLocationListListener(this);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.TraceLocationListListener#
	 * locationAdded(com.nokia.tracecompiler.engine.TraceLocation)
	 */
	public void locationAdded(TraceLocation location) {
		location.addLocationListener(this);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.TraceLocationListListener#
	 * locationRemoved(com.nokia.tracecompiler.engine.TraceLocation)
	 */
	public void locationRemoved(TraceLocation location) {
		location.removeLocationListener(this);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationListener#
	 * locationValidityChanged(com.nokia.tracecompiler.engine.TraceLocation)
	 */
	public void locationValidityChanged(TraceLocation location) {
		String fileName = location.getFileName();
		if (currentFileName != null) {
			if (!fileName.equals(currentFileName)) {
				currentFileName = fileName;
				printLocationErrors();
			}
		} else {
			currentFileName = fileName;
		}
		TraceCompilerErrorCode code = location.getValidityCode();
		if (code != TraceCompilerErrorCode.OK
				&& code != TraceCompilerErrorCode.TRACE_DOES_NOT_EXIST) {
			if (!errorList.contains(location)) {
				errorList.add(location);
				location.reference();
			}
		} else {
			if (errorList.remove(location)) {
				location.dereference();
			}
		}
	}

	/**
	 * Prints the errors from the error list
	 */
	void printLocationErrors() {
		Collections.sort(errorList, new Comparator<TraceLocation>() {

			/**
			 * Sorts the array by line number
			 * 
			 * @param o1
			 *            location 1
			 * @param o2
			 *            location 2
			 * @return the result
			 */
			public int compare(TraceLocation o1, TraceLocation o2) {
				int line1 = o1.getLineNumber();
				int line2 = o2.getLineNumber();
				return line1 > line2 ? 1 : line1 == line2 ? 0 : -1;
			}

		});
		for (TraceLocation location : errorList) {
			printLocationError(location);
		}
		errorList.clear();
	}

	/**
	 * Prints a location error
	 * 
	 * @param location
	 *            the location
	 */
	private void printLocationError(TraceLocation location) {
		TraceCompilerEngineGlobals.getEvents().postErrorMessage(
				location.getFileName()
						+ Messages.getString("TraceCompilerModelListener.lineNumberPrefix") //$NON-NLS-1$
						+ location.getLineNumber()
						+ SourceConstants.COLON
						+ SourceConstants.SPACE
						+ TraceCompilerEngineErrorMessages.getErrorMessage(
								location.getValidityCode(), location
										.getValidityParameters()), null, true);
	}

	/**
	 * get list of cumulated erros.
	 * 
	 * @return list of TaceLocaion
	 */
	public ArrayList<TraceLocation> getErrors() {
		return errorList;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.nokia.tracecompiler.model.TraceProcessingListener#processingComplete
	 * (boolean)
	 */
	public void processingComplete(boolean changed) {
		printLocationErrors();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.nokia.tracecompiler.model.TraceProcessingListener#processingStarted()
	 */
	public void processingStarted() {
	}
}