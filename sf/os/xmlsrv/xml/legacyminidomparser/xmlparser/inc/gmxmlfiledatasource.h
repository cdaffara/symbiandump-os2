// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// GMXMLFILEDDATASOURCE.H
// This file contains the declaration of the specific CMDXMLParserFileDataSource class
// which is used by the XML Parser when the file based API is used.
// 
//

/**
 @file
*/


#ifndef __GMXMLFILEDATASOURCE_H__
#define __GMXMLFILEDATASOURCE_H__

class CMDXMLParserFileDataSource : public CBase, public MMDXMLParserDataProvider
	{
public:
	virtual void GetData(TPtrC8 &aPtr, TRequestStatus &aStatus);
	virtual void Disconnect();

	/** Creates and Initialises the data provider with a specified XML file.
	
	@param aRFs File server session
	@param aFileToParse The file name to parse
	*/
	static CMDXMLParserFileDataSource *NewL(RFs aRFs, const TDesC& aFileToParse);
	static CMDXMLParserFileDataSource *NewLC(RFs aRFs, const TDesC& aFileToParse);

	static CMDXMLParserFileDataSource* NewL(RFile& aFileHandleToParse);

	~CMDXMLParserFileDataSource();

private:
	CMDXMLParserFileDataSource();

	/** Second phase constructor.
	*/
	void ConstructL(RFs aRFs, const TDesC& aFileToParse);
	
	void ConstructL(RFile& aFileHandleToParse);

private:

	enum EInternalState
	{
		KInit = 0,
		KDataSent,
		KDone,
		KError
	};

	TInt  iState;
	RFile iXMLFile;
	HBufC8* iXMLFileBuffer;
	};

#endif
