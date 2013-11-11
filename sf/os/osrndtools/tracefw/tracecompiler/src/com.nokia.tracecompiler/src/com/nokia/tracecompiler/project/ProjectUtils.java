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
* Project file utilities
*
*/
package com.nokia.tracecompiler.project;

import java.io.File;
import java.util.ArrayList;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.source.SymbianConstants;

/**
 * Project file utilities
 *
 */
public final class ProjectUtils {


	/**
	 * Tries to locate existing file with given extension and if not found,
	 * proposes a new location for that file. The new location will be
	 * <i>createPath</i> or if that is null, the directory where the trace
	 * project file (.tbprj) resides. Trace project must be open before this can
	 * be called
	 *
	 * @param model
	 *            the trace model
	 * @param createPath
	 *            the path to create if existing file is not found
	 * @param fileName
	 *            the name for the new file
	 * @param appendProjectName
	 *            true if the name is appended to end of model name
	 * @return the project file path for the new file
	 * @throws TraceCompilerException
	 *             if the model does not have any project files
	 */
	public static String getLocationForFile(TraceModel model,
			String createPath, String fileName, boolean appendProjectName)
			throws TraceCompilerException {
		TraceProjectFile projectFile = model
				.getExtension(TraceProjectFile.class);
		if (projectFile == null) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.MODEL_NOT_READY);
		}
		String projectName = ""; //$NON-NLS-1$
		if (appendProjectName) {
			projectName = projectFile.getProjectName();
		}
		return getLocationForFile(projectFile.getPath(), projectName,
				createPath, fileName);
	}

	/**
	 * Tries to locate existing file with given extension and if not found,
	 * proposes a new location for that file. The new location will be
	 * <i>createPath</i> or if that is null, the directory where the trace
	 * project file resides. Trace project must be open before this can
	 * be called
	 *
	 * @param projectPath
	 *            path to the project
	 * @param projectName
	 *            name of the project
	 * @param createPath
	 *            the path to create if existing file is not found
	 * @param extension
	 *            the file extension for the new file
	 * @return the project file path for the new file
	 */
	public static String getLocationForFile(String projectPath,
			String projectName, String createPath, String extension) {
		ArrayList<String> searchPaths = new ArrayList<String>();
		if (createPath != null && createPath.length() > 0) {
			// If create path is explicitly specified, it is added to the search
			// path list
			searchPaths.add(createPath);
		}
		// Default directories are always included into search
		searchPaths.add(ProjectEngine.traceFolderName);
		searchPaths.add(SymbianConstants.GROUP_DIRECTORY);
		searchPaths.add(SymbianConstants.INCLUDE_DIRECTORY);
		String[] pathArr = new String[searchPaths.size()];
		searchPaths.toArray(pathArr);
		return getLocationForFile(projectPath, projectName, pathArr,
				createPath, extension);
	}

	/**
	 * Tries to locate existing file and if not found, proposes a new location
	 * for that file. The search starts from <i>projectPath</i>. If
	 * <i>checkParents</i> is true, the parent directories are also checked if
	 * the file is not found
	 *
	 * @param projectPath
	 *            path where to start the search
	 * @param projectName
	 *            the name of the project
	 * @param searchPaths
	 *            the sub-paths to be searched
	 * @param createPath
	 *            the sub-path to create if file is not found
	 * @param fileName
	 *            extension of the file to be located
	 * @return the project file path for the new file
	 */
	private static String getLocationForFile(String projectPath,
			String projectName, String[] searchPaths, String createPath,
			String fileName) {
		// If the project file is in one of the search paths, the path is
		// changed to its parent
		File projectPathFile = new File(projectPath);
		String projectPathName = projectPathFile.getName();
		for (int i = 0; i < searchPaths.length; i++) {
			if (searchPaths[i].equals(projectPathName)) {
				projectPath = projectPathFile.getParent();
				i = searchPaths.length;
			}
		}
		// Tries to find an existing file based on project path and project name
		String filePath = findProjectFile(projectPath, projectName,
				searchPaths, fileName, false);
		if (filePath == null) {
			// If file is not found, this returns a new path
			filePath = projectPath + File.separator + createPath
					+ File.separator + projectName + fileName;
		}
		return filePath;
	}

	/**
	 * Finds a project file or directory based on a file or directory
	 *
	 * @param startPath
	 *            the file name or path where to start search
	 * @param projectName
	 *            the name of the project file
	 * @param searchDirectories
	 *            the directories to search
	 * @param fileExtension
	 *            the file extension to be located or null if locating one of
	 *            the search directories
	 * @param checkParents
	 *            true if parent directories should be checked
	 * @return the file or directory path if found or null if not
	 */
	private static String findProjectFile(String startPath, String projectName,
			String[] searchDirectories, String fileExtension,
			boolean checkParents) {
		String foundFile = null;
		if (startPath != null) {
			File file = new File(startPath);
			if (!file.isDirectory()) {
				file = file.getParentFile();
			}
			if (file != null) {
				do {
					for (int i = 0; i < searchDirectories.length
							&& foundFile == null; i++) {
						foundFile = findProjectFile(searchDirectories[i],
								projectName, fileExtension, file);
					}
					file = file.getParentFile();
				} while (file != null && foundFile == null && checkParents);
			}
		}
		return foundFile;
	}

	/**
	 * Searches a single directory for a project file
	 *
	 * @param searchDirectory
	 *            the directory to search
	 * @param fileExtension
	 *            the file extension to be located or null if locating one of
	 *            the search directories
	 * @param projectName
	 *            the name of the project file
	 * @param file
	 *            the current file
	 * @return the file or directory path if found or null if not
	 */
	private static String findProjectFile(String searchDirectory,
			String projectName, String fileExtension, File file) {
		String foundFile = null;
		File projectPath = new File(file.getAbsolutePath() + File.separator
				+ searchDirectory + File.separator);
		if (projectPath.exists()) {
			if (fileExtension == null) {
				foundFile = projectPath.getAbsolutePath();
			} else {
				foundFile = findProjectFileFromDirectory(projectPath,
					projectName, fileExtension);
			}
		}
		return foundFile;
	}

	/**
	 * Gets the first file with given extension from given directory
	 *
	 * @param path
	 *            the path to search
	 * @param projectName
	 *            the name of the project file
	 * @param fileExtension
	 *            the file extension
	 * @return the file name if found or null if not
	 */
	private static String findProjectFileFromDirectory(File path,
			String projectName, String fileExtension) {
		String proposal = null;
		File[] files = path.listFiles();
		if (projectName != null) {
			// If project name is specified, only the file that matches the name
			// is returned
			String nameFromProjectFile = projectName + fileExtension;

			// Check does user have access rights to the traces folder
			if (path.canWrite() == false) {
				TraceCompilerEngineGlobals
						.getEvents()
						.postErrorMessage(
								Messages
										.getString("ProjectUtils.accesRightErrorText"), null, true); //$NON-NLS-1$
				System.exit(1);
			}
			
			for (int i = 0; i < files.length && proposal == null; i++) {
				String fname = files[i].getName();
				if (fname.equals(nameFromProjectFile)) {
					proposal = files[i].getAbsolutePath();
				}
			}
		} else {
			// If project name is not specified, this proposes the first file
			// which ends with the file extension
			for (int i = 0; i < files.length && proposal == null; i++) {
				String fname = files[i].getName();
				if (fname.length() > fileExtension.length()) {
					int extstart = fname.length() - fileExtension.length();
					if (fname.substring(extstart).equals(fileExtension)) {
						proposal = files[i].getAbsolutePath();
					}
				}
			}
		}
		return proposal;
	}
}
