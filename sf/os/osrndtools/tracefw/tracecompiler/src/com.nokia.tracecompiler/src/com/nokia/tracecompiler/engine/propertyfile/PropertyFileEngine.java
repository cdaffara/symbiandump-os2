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
* Property file management engine
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import java.io.File;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import com.nokia.tracecompiler.engine.TraceCompilerEngineBase;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.project.ProjectUtils;
import com.nokia.tracecompiler.source.SourceConstants;

/**
 * Property file management engine
 * 
 */
public final class PropertyFileEngine extends TraceCompilerEngineBase {

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 */
	public PropertyFileEngine(TraceModel model) {
		this.model = model;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectOpened()
	 */
	@Override
	public void projectOpened() throws TraceCompilerException {
		TracePropertyFile propertyFile = model
				.getExtension(TracePropertyFile.class);
		if (propertyFile == null) {
			String fileName = null;
			try {
				fileName = ProjectUtils.getLocationForFile(model,
						ProjectEngine.traceFolderName,
						PropertyFileConstants.PROPERTY_FILE_NAME, false);
			} catch (TraceCompilerException e) {
				// Model should always be open when traceProjectOpened is
				// called
			}
			if (fileName != null) {
				propertyFile = parsePropertyFile(fileName);
			}
		}
		if (propertyFile == null) {
			String msg = Messages
					.getString("PropertyFileEngine.FailedToAttachFile"); //$NON-NLS-1$
			TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, null, true);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectClosing()
	 */
	@Override
	public void projectClosed() {
		model.removeExtensions(TracePropertyFile.class);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#exportProject()
	 */
	@Override
	public void exportProject() {
	}

	/**
	 * Parses the property file
	 * 
	 * @param fileName
	 *            the file path
	 * @return the property file
	 */
	private TracePropertyFile parsePropertyFile(String fileName) {
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		factory.setValidating(false);
		DocumentBuilder builder;
		TracePropertyFile propertyFile = null;
		try {
			builder = factory.newDocumentBuilder();
			File file = new File(fileName);
			if (file.exists()) {
				try {
					PropertyFileParser parser = new PropertyFileParser(model,
							fileName, builder);
					parser.parse();
					propertyFile = new TracePropertyFile(file.getParent(),
							parser.getDocument());
				} catch (TraceCompilerException e) {
					// Problem parsing document -> Backup and create new
					TraceCompilerEngineGlobals.getEvents().postError(e);
					createBackup(fileName);
				}
			}
			if (propertyFile == null) {
				propertyFile = new TracePropertyFile(file.getParent(), builder
						.newDocument());
			}
			model.addExtension(propertyFile);
		} catch (ParserConfigurationException e) {
		}
		return propertyFile;
	}

	/**
	 * Creates a backup of the property file
	 * 
	 * @param path
	 *            the file to be backed up
	 */
	private void createBackup(String path) {
		boolean backup = false;
		File file = new File(path);
		if (file.exists()) {
			boolean allexist = true;
			// Checks the existing backup files and renames the old file to
			// largest available number starting from 0
			File f = null;
			for (int i = 0; i < PropertyFileConstants.BACKUP_COUNT && allexist; i++) {
				f = new File(path + i + PropertyFileConstants.BACKUP_EXTENSION);
				if (!f.exists()) {
					backup = FileUtils.copyFile(file, f);
					allexist = false;
				}
			}
			if (allexist) {
				// If all backups from 0 to 9 exist, the old ones are moved
				// back 1 step. The new file is renamed to *9.h
				for (int i = 0; i < PropertyFileConstants.BACKUP_COUNT; i++) {
					f = new File(path + i
							+ PropertyFileConstants.BACKUP_EXTENSION);
					if (i == 0) {
						f.delete();
					} else {
						f.renameTo(new File(path + (i - 1)
								+ PropertyFileConstants.BACKUP_EXTENSION));
					}
				}
				f = new File(path + PropertyFileConstants.LAST_BACKUP);
				backup = FileUtils.copyFile(file, f);
			}
			if (f != null && backup) {
				String msg = Messages
						.getString("PropertyFileEngine.PropertyFileBackUpPrefix") //$NON-NLS-1$
						+ FileUtils.convertSeparators(
								SourceConstants.FORWARD_SLASH_CHAR, f
										.getAbsolutePath(), false);
				TraceCompilerEngineGlobals.getEvents().postInfoMessage(msg, null);
			}
		}
	}

}
