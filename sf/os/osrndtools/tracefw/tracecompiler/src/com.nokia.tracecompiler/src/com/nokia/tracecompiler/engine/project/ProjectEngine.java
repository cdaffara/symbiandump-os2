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
* Trace project file manager
*
*/
package com.nokia.tracecompiler.engine.project;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Enumeration;

import com.nokia.tracecompiler.engine.TraceCompilerEngineBase;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.plugin.TraceAPIPluginManager;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.project.GroupNames;
import com.nokia.tracecompiler.project.ProjectUtils;

/**
 * Trace project engine
 * 
 */
public final class ProjectEngine extends TraceCompilerEngineBase {

	/**
	 * Parameters for create project
	 */
	private class CreateProjectParameters {

		/**
		 * Trace project path
		 */
		String traceProjectPath;

		/**
		 * Trace project name
		 */
		String traceProjectName;

		/**
		 * Trace project ID
		 */
		int traceProjectID;

	}

	/**
	 * Default project ID
	 */
	private static final int DEFAULT_PROJECT_ID = 0x0; // CodForChk_Dis_Magic

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Trace directory name
	 */
	public static String traceFolderName;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 */
	public ProjectEngine(TraceModel model) {
		this.model = model;
	}

	/**
	 * Opens trace project
	 * 
	 * @param projectPath
	 *            the path to the project to be opened
	 * @param modelName
	 *            the name for the model or null to use directory name
	 * @throws TraceCompilerException
	 *             if opening fails
	 */
	public void openTraceProject(String projectPath, String modelName)
			throws TraceCompilerException {
		if (projectPath != null) {
			CreateProjectParameters parameters = createParameters(projectPath,
					modelName);

			// Create empty project
			if (model.getExtension(TraceCompilerProject.class) == null) {
				createEmptyProjectFile(parameters);
			}
		} else {
			
			// If fileName is null, there's no open source files. In
			// that the project cannot be created
			throw new TraceCompilerException(
					TraceCompilerErrorCode.SOURCE_NOT_OPEN);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectOpened()
	 */
	@Override
	public void projectOpened() {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#projectClosed()
	 */
	@Override
	public void projectClosed() {
		model.removeExtensions(TraceCompilerProject.class);
		model.removeExtensions(TraceIDCache.class);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerEngine#exportProject()
	 */
	@Override
	public void exportProject() {
		try {

			// The fixed id definitions file is updated even there are no traces
			// in case that old fixed id definitions file exist
			syncFixedIdDefinitionsFile();
		} catch (TraceCompilerException e) {
		}
	}

	/**
	 * Updated fixed id definitions file
	 * 
	 * @throws TraceCompilerException
	 *             if update fails
	 */
	private void syncFixedIdDefinitionsFile() throws TraceCompilerException {
		TraceIDCache cache = model.getExtension(TraceIDCache.class);
		// Create trace Id cache if it does not exist
		if (cache == null) {
			String path = ProjectUtils.getLocationForFile(model,
					ProjectEngine.traceFolderName,
					ProjectConstants.FIXED_ID_DEFINITIONS_FILE_NAME, false);
			if (path != null) {
				cache = new TraceIDCache(new File(path).getParent());
				model.addExtension(cache);
			}
		}
		if (cache != null) {
			File cacheFile = new File(cache.getAbsolutePath());

			// The fixed id definitions file is updated in case that there are
			// traces in model
			if (model.hasTraces()) {
				// Try to save Ids to fixed Id definition file
				try {
					SortedProperties ids = new SortedProperties();
					// Save Ids from model to fixed Id properties
					model.saveIDs(ids);

					// If there are some fixed Ids in that are not used anymore,
					// keep also those Ids in defininiton file, but mark those
					// ids as obsolete
					ids = handleObsoleteIds(ids);

					// Rewrites the trace Id cache file
					OutputStream fos = FileUtils.createOutputStream(cacheFile);
					ids.store(fos,
							ProjectConstants.FIXED_ID_DEFINITION_FILE_TITLE);
					fos.close();
					cache.postFileWrittenEvent(cache.getAbsolutePath());
				} catch (IOException e) {
					cacheFile.delete();
				}
			}
		}
	}

	/**
	 * Handle obsolete Ids
	 * 
	 * @param ids
	 *            Ids
	 */
	private SortedProperties handleObsoleteIds(SortedProperties ids) {
		SortedProperties fixedIds = model.getFixedIds();
		if (fixedIds != null) {
			Enumeration<Object> fixedIdKeys = fixedIds.keys();
			while (fixedIdKeys.hasMoreElements()) {
				String fixedIdKey = (String) fixedIdKeys.nextElement();
				if (!ids.containsKey(fixedIdKey)) {
					String value = fixedIds.getProperty(fixedIdKey);
					boolean markAsObsolete = true;
					// In case of groups we only mark user defined
					// groups as obsolete
					if (fixedIdKey.startsWith(model.GROUP_PROPERTY_PREFIX)) {
						int valueAsInt = 0;
						try {
							valueAsInt = Integer.decode(value).intValue();
						} catch (NumberFormatException e) {
							// Corrupted.
							valueAsInt = 0;
						}
						if (valueAsInt < GroupNames.USER_GROUP_ID_FIRST) {
							markAsObsolete = false;
						}
					}
					if (markAsObsolete) {
						// If OBSOLETE tag text already exit, do not
						// add that again
						if (fixedIdKey
								.startsWith(model.OBSOLETE_PROPERTY_PREFIX)) {
							ids.setProperty(fixedIdKey, value);
						} else {
							ids.setProperty(model.OBSOLETE_PROPERTY_PREFIX
									+ fixedIdKey, value);
						}
					}
				}
			}
		}
		return ids;
	}

	/**
	 * Creates the parameters for new project
	 * 
	 * @param projectPath
	 *            the project path
	 * @param projectName
	 *            the name for the project
	 * @return the parameters
	 */
	private CreateProjectParameters createParameters(String projectPath,
			String projectName) {
		CreateProjectParameters queryData = new CreateProjectParameters();
		queryData.traceProjectPath = projectPath + File.separator
				+ ProjectEngine.traceFolderName;
		queryData.traceProjectName = projectName;
		queryData.traceProjectID = DEFAULT_PROJECT_ID;
		return queryData;
	}

	/**
	 * Creates the project file from query results
	 * 
	 * @param queryData
	 *            the query result
	 * @throws TraceCompilerException 
	 */
	private void createEmptyProjectFile(CreateProjectParameters queryData) throws TraceCompilerException {
		model.setName(queryData.traceProjectName);
		model.setID(queryData.traceProjectID);
		String componentName = model.getName();
		TraceCompilerProject file = new TraceCompilerProject(
				queryData.traceProjectPath, componentName);
		createAPI(file);
	}

	/**
	 * Creates the project API
	 * 
	 * @param file
	 *            the project file
	 */
	private void createAPI(TraceCompilerProject file) {
		model.addExtension(file);
		TraceAPIPluginManager plugin = model
				.getExtension(TraceAPIPluginManager.class);
		plugin.createDefaultAPI();
	}
}
