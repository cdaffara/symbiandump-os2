/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Utility functions for property file
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Utility functions for property file
 * 
 */
final class PropertyFileUtils {

	/**
	 * Finds the root element from property file document
	 * 
	 * @param document
	 *            the document
	 * @return the root element
	 */
	static Element findRoot(Document document) {
		NodeList list = document.getChildNodes();
		Element rootElement = null;
		for (int i = 0; i < list.getLength(); i++) {
			Node root = list.item(i);
			if (root.getNodeType() == Node.ELEMENT_NODE) {
				if (root.getNodeName().equalsIgnoreCase(
						PropertyFileConstants.ROOT_ELEMENT)) {
					rootElement = (Element) root;
				}
			}
		}
		return rootElement;
	}

}
