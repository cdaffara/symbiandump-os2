/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

/***
 * @author - Symbian Ltd 2005
 * 
 * This file is a plugin file which takes XML and XSL input files and 
 * requests the DocumentLoaderFactory class to create a data model out of it
 * 
 */

import com.symbian.et.viewer.loader.*;
import java.io.File;

public class TefDocumentLoader implements LoaderFactory
{
    //Constructor  	
    public TefDocumentLoader()
    {
    }
    //To call the transformation function with the specified xml and xsl file	
    public void loadDocument() throws PluginLoaderException
    {
    	 DocumentLoaderFactory lFactory = DocumentLoaderFactory.getInstance();
         lFactory.transform(fXml, fXsl);
         lFactory.setModel();

    }

    //Set the editable to false	
    public boolean isEditable()
    {
        return false;
    }

    //Set the filterable to true
    public boolean isFilterable()
    {
        return true;
    }

    //Set the sortable to true
    public boolean isSortable()
    {
        return true;
    }

    //Assign xml file to fxml	
    public void setXmlFile(String aFile)
    {
        fXml = new File(aFile);
    }

    //Assign xsl file to fxsl	    
    public void setXslFile(String aFile)
    {
        fXsl = new File(aFile);
    }


    public String getPluginComment()
    {
        return "TEF report plugin for general purpose viewer";
    }

    private File fXml;
    private File fXsl;
}
