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
* FIle element parser
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.w3c.dom.Element;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.engine.source.SourceEngine;
import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * File element parser
 * 
 */
final class FileElementParser implements PropertyFileElementParser {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.propertyfile.PropertyFileElementParser#
	 *      parse(java.lang.Object, org.w3c.dom.Element)
	 */
	public void parse(Object owner, Element element)
			throws TraceCompilerException {
		String filePath = element.getTextContent();
		SourceEngine sourceEngine = TraceCompilerEngineGlobals.getSourceEngine();
		ArrayList<String> list = sourceEngine.getNonSourceFiles();

		String projectPath = TraceCompilerEngineGlobals.getProjectPath();

		File file = new File(projectPath + File.separatorChar
				+ ProjectEngine.traceFolderName + File.separatorChar + filePath);

		try {
			filePath = file.getCanonicalPath();
		} catch (IOException e) {
			e.printStackTrace();
		}

		if (!list.contains(filePath)) {
			sourceEngine.addNonSourceFile(filePath);
		}
	}
}