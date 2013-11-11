/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Header engine manages the trace header file that is included into source code
*
*/
package com.nokia.tracecompiler.engine.header;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.engine.LocationListBase;
import com.nokia.tracecompiler.engine.LocationProperties;
import com.nokia.tracecompiler.engine.TraceCompilerEngineBase;
import com.nokia.tracecompiler.engine.TraceCompilerEngineEvents;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.file.FileCompareOutputStream;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution.TraceHeaderContributionType;
import com.nokia.tracecompiler.project.ProjectUtils;
import com.nokia.tracecompiler.project.TraceProjectAPI;
import com.nokia.tracecompiler.source.SourceUtils;

/**
 * Header engine manages the trace header file that is included into source
 * code. This implements the plug-in interface to get notifications about
 * project file opening and closing
 * 
 */
public final class HeaderEngine extends TraceCompilerEngineBase {

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            trace model
	 */
	public HeaderEngine(TraceModel model) {
		this.model = model;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectOpened()
	 */
	@Override
	public void projectOpened() {
		TraceHeader header = model.getExtension(TraceHeader.class);
		if (header == null) {
			String fileName = null;
			try {
				fileName = ProjectUtils.getLocationForFile(model,
						ProjectEngine.traceFolderName,
						HeaderConstants.TRACE_HEADER_NAME, false);
			} catch (TraceCompilerException e) {
				// Model should always be open when traceProjectOpened is called
			}
			if (fileName != null) {
				header = new TraceHeader(fileName, false);
				model.addExtension(header);
			} else {
				String msg = Messages
						.getString("HeaderEngine.FailedToAttachHeader"); //$NON-NLS-1$
				TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, null, true);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engin.TraceCompilererEngine#projectClosing()
	 */
	@Override
	public void projectClosed() {
		model.removeExtensions(TraceHeader.class);
	}

	/**
	 * Gets the name for the trace header file based on given source
	 * 
	 * @param sourceFile
	 *            the source file name
	 * @return the header file name
	 */
	public String getHeaderFileName(String sourceFile) {
		String retval = null;
		if (model != null) {
			// The header file name is the source file name with extension
			// replaced by Traces.h
			File f = new File(sourceFile);
			retval = removeFileExtension(f.getName())
					+ HeaderConstants.TRACE_HEADER_EXTENSION;
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engi.TraceCompilerlerEngine#projectExporting()
	 */
	@Override
	public void exportProject() throws TraceCompilerException {
		if (model.isValid() && model.hasTraces()) {
			TraceHeader header = model.getExtension(TraceHeader.class);
			if (header != null) {
				// Headers for each source are written first
				String path = header.getPath();
				ArrayList<String> fileNames = new ArrayList<String>();
				collectSourceFilesFromTraces(fileNames);
				for (String fileName : fileNames) {
					String target = path + File.separator + fileName;
					writeHeaderFile(target);
				}
				// The main header is written after everything else succeeds
				writeMainHeaderFile(header);
			}
		}
	}

	/**
	 * Collects the source file names from traces
	 * 
	 * @param fileNames
	 *            the file names
	 */
	private void collectSourceFilesFromTraces(ArrayList<String> fileNames) {
		for (TraceGroup group : model) {
			for (Trace trace : group) {
				Iterator<LocationListBase> itr = trace
						.getExtensions(LocationListBase.class);
				while (itr.hasNext()) {
					LocationListBase base = itr.next();
					for (LocationProperties loc : base) {
						String locFileName = loc.getFileName();
						if (locFileName != null) {
							addFileToList(fileNames, locFileName);
						}
					}
				}
			}
		}
	}

	/**
	 * Adds a file to list if it does not exist
	 * 
	 * @param fileNames
	 *            the list
	 * @param locFileName
	 *            the file
	 */
	private void addFileToList(ArrayList<String> fileNames, String locFileName) {
		locFileName = removeFileExtension(locFileName);
		if (!fileNames.contains(locFileName)) {
			fileNames.add(locFileName);
		}
	}

	/**
	 * Removes the file extension from file name
	 * 
	 * @param fileName
	 *            the file name
	 * @return name without extension
	 */
	private String removeFileExtension(String fileName) {
		int index = fileName.lastIndexOf('.');
		int sep1 = fileName.lastIndexOf('/');
		int sep2 = fileName.lastIndexOf('\\');
		if (index > sep1 && index > sep2) {
			fileName = fileName.substring(0, index);
		}
		return fileName;
	}

	/**
	 * Writes the header to given target
	 * 
	 * @param target
	 *            the target file
	 * @throws TraceCompilerException
	 *             if writing fails
	 */
	private void writeHeaderFile(String target) throws TraceCompilerException {
		// The TraceHeader stored into the model is not used when writing
		// headers, since a separate header is written for each source file
		TraceHeader header = new TraceHeader(target, true);
		try {
			header.setOwner(model);
			TraceHeaderWriter writer = new TraceHeaderWriter(header);
			TraceCompilerEngineEvents events = TraceCompilerEngineGlobals.getEvents();
			if (writer.write()) {
				header.postFileWrittenEvent(header.getAbsolutePath());
			} else {
				String msg = Messages
						.getString("HeaderEngine.TraceHeaderNotChangedPrefix"); //$NON-NLS-1$
				events.postInfoMessage(msg + header.getAbsolutePath(), null);
			}
		} finally {
			// The header owner must be reset to null, since that makes
			// unregisters it from the model
			header.setOwner(null);
		}
	}

	/**
	 * Writes the main header file to given target
	 * 
	 * @param header
	 *            the header
	 * @throws TraceCompilerException
	 *             if writing fails
	 */
	private void writeMainHeaderFile(TraceHeader header)
			throws TraceCompilerException {
		// The TraceHeader stored into the model is not used when writing
		// headers, since a separate header is written for each source file
		TraceCompilerEngineEvents events = TraceCompilerEngineGlobals.getEvents();
		String path = header.getAbsolutePath();
		try {
			if (writeMainHeader(path)) {
				header.postFileWrittenEvent(path);
			} else {
				String msg = Messages
						.getString("HeaderEngine.TraceHeaderNotChangedPrefix"); //$NON-NLS-1$
				events.postInfoMessage(msg + path, null);
			}
		} catch (IOException e) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.CANNOT_WRITE_PROJECT_FILE, e);
		}
	}

	/**
	 * Writes the main header to given target
	 * 
	 * @param target
	 *            the target
	 * @return true if written, false if the file already had the same content
	 * @throws IOException
	 *             if writing fails
	 */
	private boolean writeMainHeader(String target) throws IOException {
		File f = new File(target);
		FileCompareOutputStream stream = new FileCompareOutputStream(f);
		String licence = TraceCompilerEngineGlobals.getDefaultLicence(true);
		if (licence != null) {
			stream.write(licence.getBytes());
		}
		stream.write(SourceUtils.createHeaderGuard(f.getName()).getBytes());
		TraceProjectAPI api = model.getExtension(TraceProjectAPI.class);
		if (api instanceof TraceHeaderContribution) {
			String[] content = ((TraceHeaderContribution) api)
					.getContribution(TraceHeaderContributionType.MAIN_HEADER_CONTENT);
			if (content != null && content.length == 1) {
				stream.write(content[0].getBytes());
			}
		}
		return stream.writeFile();
	}
}
