/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Defines the rights object class used by the reference agent. 
* Please note that the reference agent uses a binary format to externalize and
* internalize rights objects. In a real agent, a rights object will be an XML 
* document expressing permissions. The class defined here is not a reference 
* implementation for rights object.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRARIGHTSOBJECT_H_
#define SRARIGHTSOBJECT_H_

#include <e32base.h>
#include <s32strm.h>
#define deleteObject(a) {delete a; a = NULL;}

	
enum TSraAccessRights
	{
	EProgramProtected				= 0x01, ///< The specific program is protected
	EServiceProtected				= 0x02, ///< The whole service is protected
	ECanExportContent				= 0x04, ///< The content can be exported
	EMustProtectContentIfRecording	= 0x08, ///< The content must be protected if recorded. If ECanRecord is undefined, then this attribute is also undefined.
	ECanPlayContent					= 0x10  ///< The content can be played
	};
	
class CSraRightsObject : public CBase
	{
public:
	IMPORT_C static CSraRightsObject* NewL(RReadStream& aStream);
	IMPORT_C ~CSraRightsObject();
#ifdef _DEBUG
	// These are utility functions only and not used in regular flow of the component.
	// They are used to create RO files for the streaming test cases.
	static CSraRightsObject* NewL(TUint8 aAccessRights, const TDesC8& aCid, const TDesC8& aRiUri, const TDesC8& aKey, const TBool& aExpired=0);
	void ExternalizeL(RWriteStream& aStream) const;
#endif
	IMPORT_C HBufC8* ContentId() const;
	IMPORT_C HBufC8* RightsIssuer() const;
	IMPORT_C HBufC8* Key() const;
	IMPORT_C TUint8 AccessRights() const;
	IMPORT_C TBool IsExpired() const;
		
private:
#ifdef _DEBUG
	CSraRightsObject(const TBool& aExpired, TUint8 aAccessRights=0);
#endif
	CSraRightsObject();
	void ConstructL(const TDesC8& aCid, const TDesC8& aRiUri, const TDesC8& aKey);
	void InternalizeL(RReadStream& aStream);
		
private:
	TBool	iExpired;		///< Whether the RO is expired
	TUint8 iAccessRights;	///< Flag for stream access rights
	HBufC8* iCid;			///< Contendt ID
	HBufC8* iRiUri;			///< Right Issuer URI
	HBufC8* iKey;			///< Long term decryption key
	};

#endif /*SRARIGHTSOBJECT_H_*/
