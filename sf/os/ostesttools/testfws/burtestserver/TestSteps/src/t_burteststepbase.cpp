// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @released
*/

#include "t_burteststepbase.h"

namespace bur_ts
	{
	_LIT(KTabText, "\t|");
	_LIT(KFormatText2, "%S%S");
	_LIT(KFormatText3, "%S%S%S");
	_LIT(KFormatText4, "%S%S%d");
	_LIT(KFormatText5, "%S%S%X");
	_LIT(KFormatText6, "%S%S%s");
	
	const TUint8 KMaxHexLength = 8;
	
	CBURTestStepBase::CBURTestStepBase(CBURTestServer& aParent) 
		/**
		C++ Constructor
		
		@internalComponent
		@released
		
		@param aParent - The parent CBURTestServer of this child test step.
		*/
		:iParentTestServer(aParent),
		iBackupClient(NULL),
		iBackupIncType(EBackupBase),
	 	iIsPartial(EFalse),
		iFailures(KErrNone),
		iStorageManager(NULL),
		iMode(EBackupMode),
		iWaitedForActive(EFalse)
		{}
	
	CBURTestStepBase::~CBURTestStepBase()
		/**
		C++ Destructor
		@internalComponent
		@released
		*/
		{
		iDataOwners.ResetAndDestroy();
		iDataOwners.Close();
			
		iSidArray.Close();
		iJidArray.Close();
		
		iFs.Close();
		
		iTransferTypes.ResetAndDestroy();
		iTransferTypes.Close();
		
		delete iStorageManager;
		delete iActiveScheduler;
		
		delete iBackupClient;
		}
	
	// LOGGING METHODS
	void CBURTestStepBase::Log(TUint aIndentationLevel, const TDesC& aLog)
		/**
		Logs the data inside the string as text string at aIndentationLevel.
		
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF3(KFormatText2, &ptr, &aLog);
		delete indentation;
		}
	
	void CBURTestStepBase::LogWithText(TUint aIndentationLevel, const TDesC& aLog, const TDesC& aText)
		/**
		Writes the message with a text string.
		
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		@param aText - Textual data to be logged with the main message.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF4(KFormatText3, &ptr, &aLog, &aText);
		delete indentation;
		}
	
	void CBURTestStepBase::LogWithText8(TUint aIndentationLevel, const TDesC& aLog, const TDesC8& aText)
		/**
		8-bit variant of the above
		
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		@param aText - 8-bit textual data to be logged with the main message.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF4(KFormatText3, &ptr, &aLog, &aText);
		delete indentation;
		}
	
	void CBURTestStepBase::LogWithNum(TUint aIndentationLevel, const TDesC& aLog, TInt aNum)
		/**
		Writes to the report a number along with a message.
		
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		@param aNum - Number to be printed with the log.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF4(KFormatText4, &ptr, &aLog, aNum);
		delete indentation;
		}
	
	void CBURTestStepBase::LogWithSID(TUint aIndentationLevel, const TDesC& aLog, TSecureId aSecureId)
		/**
		Writes out a TSecureId as a hexadecimal number along with a message.
		
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		@param aSecureId - A TSecureId to be printed as a hexadecimal number.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF4(KFormatText5, &ptr, &aLog, aSecureId.iId);
		delete indentation;
		}
	
	void CBURTestStepBase::LogWithChar(TUint aIndentationLevel, const TDesC& aLog, TChar aChar)
		/**
		@internalComponent
		@released
		
		@param aIndentationLevel - The indentation level in the TestExecute test report.
		@param aLog - The message to be logged.
		@param aChar - A single char, appended at the end of the message.
		*/
		{
		HBufC* indentation = NULL;
		indentation = GetIndentation(aIndentationLevel);
		TPtr ptr = indentation->Des();
		INFO_PRINTF4(KFormatText6, &ptr, &aLog, &aChar);
		delete indentation;
		}
	
	HBufC* CBURTestStepBase::GetIndentation(TInt aLevel)
		/**
		Generates a string of indentation markers (based on KIndentationMarker) the 
		level of depth specified by aLevel
		
		@internalComponent
		@released
		
		@param aLevel - Level of depth required for indentation.
		*/
		{
		HBufC* hbuf = NULL;
		
		// Indentation level times the length of the indentation marker, 
		// plus room for a tab and a '|'
		TInt indentationLength = aLevel * KIndentationMarker().Length() + 10;
		hbuf = HBufC::NewL(indentationLength);
		
		hbuf->Des().Append(KTabText);
		for(TInt i = 0; i < aLevel; ++i)
			{
			hbuf->Des().Append(KIndentationMarker);
			}
		
		return hbuf;
		}
	
	void CBURTestStepBase::CreateStorageManagerL()
		/**
		Must only be called after the user input has been processed.
		@internalComponent
		@released
		*/
		{
		iStorageManager = CStorageManager::NewL(iBackupDirName, iDriveList, this);
		}
	
	void CBURTestStepBase::ProcessUserOptionsL()
		/**
		This function uses TestExecute config file parsers to read in the user preferences
		from ini files
		
		@internalComponent
		@released
		 */
		{
		TPtrC backupDir;
		TInt isPartial = 0;
		TInt isBase = 0;
		TPtrC driveListText;
		
		// directory for archiving backup data
		_LIT(KBackupDirText1, "BackupDir");
		GetStringFromConfig(ConfigSection(), KBackupDirText1, backupDir);
		
		_LIT(KBackupDirText2, "Backup directory: ");
		LogWithText(LOG_LEVEL3, KBackupDirText2, backupDir);
		iBackupDirName.Copy(backupDir);
		
		// affected drives
		_LIT(KDriveListText1, "DriveList");
		GetStringFromConfig(ConfigSection(), KDriveListText1, driveListText);
		_LIT(KDriveListText2, "Drives: ");
		
		LogWithText(LOG_LEVEL3, KDriveListText2, driveListText);
		// extract all drives from the user input
		iDriveList = CBURTestStepBase::StringToDriveListL(driveListText);
		
		// create the storage handler based on the drives and backup directory
		CreateStorageManagerL();
		
		// full/partial backup
		_LIT(KPartialText1, "IsPartial");
		GetIntFromConfig(ConfigSection(), KPartialText1, isPartial);
		if (isPartial == 0)
			{
			iIsPartial = EFalse;
			
			_LIT(KPartialText2, "Full/Partial Type: Full");
			Log(LOG_LEVEL3, KPartialText2);
			} //if
		else if (isPartial == 1)
			{
			iIsPartial = ETrue;
			TSecureId sid;
			
			TChar ch;
			TPtrC list;
			
			
			// PARSING ID LIST //
			
			_LIT(KSIDList, "IDs");
			GetStringFromConfig(ConfigSection(), KSIDList, list);
			TLex sidLex(list);
		
			while (!sidLex.Eos())
				{
        		sidLex.Mark();
	        	ch=sidLex.Peek();

    	    	while(!sidLex.Eos() && ( ch=sidLex.Peek() ) != TChar(','))
        			sidLex.Inc();
 
        		if(!sidLex.TokenLength())
        			continue;
        		
        		TPtrC pToken = sidLex.MarkedToken();
        		TLex token(pToken);
        		
        		if (pToken.Length() <= KMaxHexLength && token.Val(sid.iId, EHex) == KErrNone)
        			{
        			iSidArray.Append(sid);
					_LIT(KFound, "ID Found in ini file: ");
					LogWithSID(LOG_LEVEL4, KFound, sid);		
        			} //if
        		else // must be a Java suite hash
        			{
        			iJidArray.Append(pToken);
        			_LIT(KJFound, "Java ID Found in ini file: ");
        			LogWithText(LOG_LEVEL4, KJFound, pToken);
        			}
				
        		if(ch==TChar(','))
        			sidLex.Inc();
        		
        		sidLex.SkipSpace();
        
				} //while
				
			
			_LIT(KPartialText3, "Full/Partial Type: Partial");
			Log(LOG_LEVEL3, KPartialText3);
			} //if
		else
			{
			_LIT(KPartialText4, "Full/Partial Type: UNKNOWN");
			_LIT(KPartialText5, "** PLEASE USE 0 OR 1 ONLY **");
			_LIT(KPartialText6, "** Using FULL as default  **");
			Log(LOG_LEVEL4, KPartialText4);
			Log(LOG_LEVEL4, KPartialText5);
			Log(LOG_LEVEL4, KPartialText6);
			}
		
		// base/incremental
		_LIT(KIncrText1, "BaseOnly");
		GetIntFromConfig(ConfigSection(), KIncrText1, isBase);
		if (isBase == 1)
			{
			iBackupIncType = EBackupBase;
			
			_LIT(KIncrText2, "Base/Incremental Type: Base Only");
			Log(LOG_LEVEL3, KIncrText2);
			}
		else if (isBase == 0)
			{
			iBackupIncType = EBackupIncrement;
			
			_LIT(KIncrText3, "Base/Incremental Type: Incremental by Default");
			Log(LOG_LEVEL3, KIncrText3);
			}
		else
			{
			_LIT(KIncrText4, "Base/Incremental Type: UNKNOWN");
			_LIT(KIncrText5, "** PLEASE USE 0 OR 1 ONLY **");
			_LIT(KIncrText6, "** Using BASE as default  **");
			Log(LOG_LEVEL4, KIncrText4);
			Log(LOG_LEVEL4, KIncrText5);
			Log(LOG_LEVEL4, KIncrText6);
			}
		}
		
	
	void CBURTestStepBase::PopulateListOfDataOwnersL()
		/**
		Uses CSBEClient::ListOfDataOwnersL() to extract a list of data owners that have 
		registred for backup and restore. Populates iDataOwners.
		
		@internalComponent
		@released
		*/
		{
		_LIT(KTempText1, "Getting list of data owners from device");
		Log(LOG_LEVEL2,KTempText1);
		
		TRAPD(err, iBackupClient->ListOfDataOwnersL(iDataOwners));
		if (err != KErrNone)
			{
			_LIT(KTempText1, "Error getting list of data owners: ");
			LogWithNum(LOG_LEVEL2,KTempText1, err);
			iFailures++;
			}

		// Log
		if (iDataOwners.Count() == 0)
			{
			_LIT(KTempText3, "No data owners found.");
			Log(LOG_LEVEL3, KTempText3);
			}
		else
			{
			_LIT(KTempText4, "Number of data owners found on device: ");
			LogWithNum(LOG_LEVEL3, KTempText4, iDataOwners.Count());
			}
		}
		
	void CBURTestStepBase::PopulateListOfDataOwnersAsyncL()
		/**
		Uses CSBEClient::ListOfDataOwnersL() to extract a list of data owners that have 
		registred for backup and restore. Populates iDataOwners.
		
		@internalComponent
		@released
		*/
		{
		_LIT(KTempText1, "Getting list of data owners from device");
		Log(LOG_LEVEL2,KTempText1);
		CBURActiveObject* active = CBURActiveObject::NewL();
		CleanupStack::PushL(active);
		TRAPD(err, iBackupClient->ListOfDataOwnersL(iDataOwners, active->iStatus));
		if (err == KErrNone)
			{
			active->StartL();
			}
			
		if (err != KErrNone || active->Error() != KErrNone)
			{
			_LIT(KTempText1, "Error getting list of data owners: ");
			LogWithNum(LOG_LEVEL2,KTempText1, err);
			iFailures++;
			}
		CleanupStack::PopAndDestroy(active);
		
		// Log
		if (iDataOwners.Count() == 0)
			{
			_LIT(KTempText3, "No data owners found.");
			Log(LOG_LEVEL3, KTempText3);
			}
		else
			{
			_LIT(KTempText4, "Number of data owners found on device: ");
			LogWithNum(LOG_LEVEL3, KTempText4, iDataOwners.Count());
			}
		}
	
	void CBURTestStepBase::SetBURModeL(TBURPartType aBURPartType, TBackupIncType aBackupIncType)
		/**
		Uses CSBEClient::SetBURModeL() to set backup and restore settings
		
		@internalComponent
		@released
		
		@param aBURPartType - Full/Partial backup/restore.
		@param aBackupIncType - Base/Incremental backup/restore.
		*/
		{
		iBackupClient->SetBURModeL(iDriveList, aBURPartType, aBackupIncType);
		}
	
	void CBURTestStepBase::SetBURModeAsyncL(TBURPartType aBURPartType, TBackupIncType aBackupIncType)
		/**
		Uses CSBEClient::SetBURModeL() Asyncrhonous version to set backup and restore settings
		
		@internalComponent
		@released
		
		@param aBURPartType - Full/Partial backup/restore.
		@param aBackupIncType - Base/Incremental backup/restore.
		*/
		{
		CBURActiveObject* active = CBURActiveObject::NewL();
		CleanupStack::PushL(active);
		iBackupClient->SetBURModeL(iDriveList, aBURPartType, aBackupIncType, active->iStatus);
		active->StartL();
		User::LeaveIfError(active->Error());
		CleanupStack::PopAndDestroy(active);
		}
	
	void CBURTestStepBase::PrepareForPartialBURL()
		/**
		@internalComponent
		@released
		*/
		{
		TSecureId sid = NULL;
		
		// go through list of pids and sids
		for (TInt index = 0; index < iDataOwners.Count(); index++)
			{
			TBool notFound = ETrue;
			sid = ExtractIDL(*iDataOwners[index]);
			
			if (sid != NULL)
				{
				TInt position = iSidArray.Find(sid);
				if (position != KErrNotFound)
					{
					_LIT(KTempText2, "ID found in list of data owners: ");
					LogWithSID(LOG_LEVEL3, KTempText2, sid);
					notFound = EFalse;
					}
				else if (position == KErrNotFound)
					{
					notFound = ETrue;
					} // if else
				} // if
			else // possibly jid
				{
				HBufC* pSuiteHash = ExtractJavaIDL(*iDataOwners[index]);
				TPtrC suiteHash = pSuiteHash->Des();
				if (iJidArray.Find(suiteHash) != KErrNotFound)
					{
					_LIT(KTempText3, "Java ID found in list of data owners: ");
					LogWithText(LOG_LEVEL3, KTempText3, suiteHash);
					notFound = EFalse;
					}
				else // not found 
					{
					notFound = ETrue;
					}
				delete pSuiteHash;
				pSuiteHash = NULL;
				}
			if (notFound)
				{
				delete iDataOwners[index];
				iDataOwners[index] = NULL;
				iDataOwners.Remove(index);
				index--;
				}
			}
		}
		
	void CBURTestStepBase::SetSIDListForPartialL()
	/**
	Set the list of active data owners for partial bur
	
	@param aDataOwners List of data owners for partial bur
	*/
		{
		iSidArray.Reset();
		for (TInt i=0; i< iDataOwners.Count(); i++)
			{
			if (iDataOwners[i]->CommonSettings() & EActiveBUR)
				{
				TSBDerivedType type = iDataOwners[i]->Identifier().DerivedTypeL();
				if (type == ESIDDerivedType)
					{
					// means it is definately a sid
					CSBSecureId* secureId = CSBSecureId::NewL(&(iDataOwners[i]->Identifier()));
					CleanupStack::PushL(secureId);
						
					iSidArray.AppendL(secureId->SecureIdL());
					CleanupStack::PopAndDestroy(secureId);
					}
				else if (type == EPackageDerivedType)
					{
					CSBPackageId* packageId = CSBPackageId::NewL(&(iDataOwners[i]->Identifier()));
					CleanupStack::PushL(packageId);
					
					iSidArray.AppendL(packageId->SecureIdL());
					CleanupStack::PopAndDestroy(packageId);
					}
				} //if
			}// for
		
		// set active owners array	
		TRAPD(err,iBackupClient->SetSIDListForPartialBURL(iSidArray));
		if (err != KErrNone)
			{
			iFailures++;
			_LIT(KSidP, "Error Setting list for partial : ");
			LogWithNum(LOG_LEVEL3, KSidP, err);
			}
		}
		
	void CBURTestStepBase::GenerateSIDTransferTypesL(RTransferTypeArray& aTransferTypes, TTransferDataType aDataType)
		/**
		For each data owner in iDataOwners, this method creates CSBGenericTransferTypes
		based on the TTransferDataType passed. One transfer type is created per drive that
		the data owner in question has data on. Only creates transfer types supported by data owner
		
		@internalComponent
		@released
		
		@param aTransferTypes - Array of transfer types.
		@param aDataType - Trasnfer data type, passive base, active base, etc.
		*/
		{
		TTransferDataType tempDataType = aDataType;
		aTransferTypes.ResetAndDestroy();
		TSecureId sid = NULL;
		
		TInt length = iDriveList.Length();
		for (TInt driveCount = 0; driveCount < length; driveCount++)
			{
			if (iDriveList[driveCount])
				{
				for (TInt i = 0; i < iDataOwners.Count(); i++)
					{
					if (iDataOwners[i]->Identifier().DerivedTypeL() == ESIDDerivedType)
						{
						CSBSecureId* secureId = CSBSecureId::NewL(&(iDataOwners[i]->Identifier()));
						CleanupStack::PushL(secureId);
						
						sid = secureId->SecureIdL();
						
						CleanupStack::PopAndDestroy(secureId);
						}
					else if (iDataOwners[i]->Identifier().DerivedTypeL() == EPackageDerivedType)
						{						
						CSBPackageId* pkgId = CSBPackageId::NewL(&(iDataOwners[i]->Identifier()));
						CleanupStack::PushL(pkgId);
					
						sid = pkgId->SecureIdL();
						
						CleanupStack::PopAndDestroy(pkgId);	
						} // else if
					
					if (sid != NULL)
						{
						// decides if data owner supports that data type
						
						TBool checkIfHaveBase = EFalse;
						TBool baseOnly = EFalse;
						TBool supports = EFalse;
						switch (aDataType)
							{
							case ERegistrationData:
								supports = EFalse;
								break;
							case EPassiveBaseData:
								if (iDataOwners[i]->CommonSettings() & EPassiveBUR)
									{
									supports = ETrue;
									}
								break;
							case EPassiveSnapshotData:
								if (iDataOwners[i]->PassiveSettings() & EPassiveSupportsInc)
									{
									supports = ETrue;
									}
								break;
							case EPassiveIncrementalData:
								if (iDataOwners[i]->CommonSettings() & EPassiveBUR)
									{
									if (!(iDataOwners[i]->PassiveSettings() & EPassiveSupportsInc) && !iMode)
										{
										_LIT(KPsIncDt, "Base only for SID: ");
										LogWithSID(LOG_LEVEL3, KPsIncDt, sid);
										baseOnly = ETrue;
										supports = ETrue;
										aDataType = EPassiveBaseData;
										}
									else if (iDataOwners[i]->PassiveSettings() & EPassiveSupportsInc)
										{
										checkIfHaveBase = ETrue;
										}
									} // if
								break;
							case EActiveSnapshotData:
								if (iDataOwners[i]->ActiveSettings() & EActiveSupportsInc)
									{
									supports = ETrue;
									}
								break;
							case EActiveIncrementalData:
								if (iDataOwners[i]->CommonSettings() & EActiveBUR)
									{
									if (!(iDataOwners[i]->ActiveSettings() & EActiveSupportsInc) && !iMode)
										{
										_LIT(KAcIncDt, "Base only for SID: ");
										LogWithSID(LOG_LEVEL3, KAcIncDt, sid);
										baseOnly = ETrue;
										supports = ETrue;
										aDataType = EActiveBaseData;
										}
									else if (iDataOwners[i]->ActiveSettings() & EActiveSupportsInc)
										{
										checkIfHaveBase = ETrue;
										}	
									} // if
								break;
							case EActiveBaseData:
								if (iDataOwners[i]->CommonSettings() & EActiveBUR)
									{
									supports = ETrue;
									}
								break;
							} //switch
						
						
						if (checkIfHaveBase)
							{
							TTransferDataType dType;
							if (aDataType == EActiveIncrementalData)
								{
								dType = EActiveBaseData;
								}
							else
								{
								dType = EPassiveBaseData;
								} // if else
								
							CSBSIDTransferType* tType = CSBSIDTransferType::NewL(sid, TDriveNumber(driveCount), dType);
							CleanupStack::PushL(tType);
							TFileName fileName;
							iStorageManager->GetFileNameL(tType, fileName, EFalse);
							if(iStorageManager->IsFileExists(fileName))
								{
								supports = ETrue;
								}
							else 
								{
								_LIT(KTempText2, "No increments found use Base instead for SID: ");
								LogWithSID(LOG_LEVEL3, KTempText2, sid);
								supports = ETrue;
								aDataType = dType;
								baseOnly = ETrue;
								}
							CleanupStack::PopAndDestroy(tType);
							} // if check if have base
											
						
						if (supports)
							{
							CSBSIDTransferType* sidType = CSBSIDTransferType::NewL(sid, TDriveNumber(driveCount), aDataType);
							CleanupStack::PushL(sidType);
							aTransferTypes.AppendL(sidType);
							CleanupStack::Pop(sidType);
							} // if else
						
						// change to the proper dataType if base only true	
						if (baseOnly)
							{
							aDataType = tempDataType;
							}
						sid = NULL;	
						}// if
					} // for		
				} // if
			} //for
		}
		
	void CBURTestStepBase::GeneratePIDTransferTypesL(RTransferTypeArray& aTransferTypes, TPackageDataType aPkgDataType)
		/**
		For each data owner in iDataOwners, this method creates CSBGenericTransferTypes
		based on the TTransferDataType passed. One transfer type is created per drive that
		the data owner in question has data on.
		
		@internalComponent
		@released
		
		@param aTransferTypes - Array of transfer types.
		@param aDataType - Trasnfer data type, passive base, active base, etc.
		*/
		{
		aTransferTypes.ResetAndDestroy();
		TUid id;
		//need array to avoid duplications
		RArray<TUid> uidArray;
		CleanupClosePushL(uidArray);
		
		TInt length = iDriveList.Length();
		for (TInt driveCount = 0; driveCount < length; driveCount++)
			{
			if (iDriveList[driveCount])
				{
				TInt count = iDataOwners.Count();
				for (TInt i = 0; i < count; i++)
					{
					switch (iDataOwners[i]->Identifier().DerivedTypeL())
						{
						case EPackageDerivedType:
							if (iDataOwners[i]->CommonSettings() & EHasSystemFiles)
								{
								CSBPackageId* pkgId = CSBPackageId::NewL(&(iDataOwners[i]->Identifier()));
								CleanupStack::PushL(pkgId);
							
								id = pkgId->PackageIdL();
								if (uidArray.Find(id) == KErrNotFound)
									{
									CSBPackageTransferType* idType = CSBPackageTransferType::NewL(id, TDriveNumber(driveCount), aPkgDataType);
									CleanupStack::PushL(idType);
									aTransferTypes.AppendL(idType);
									CleanupStack::Pop(idType);
									//add to uid array
									uidArray.AppendL(id);
									}
								CleanupStack::PopAndDestroy(pkgId);	
								} //if
							break;
						} // switch
					} //for		
				} //if	
			} //for
		CleanupStack::PopAndDestroy(&uidArray);
		}
		
	void CBURTestStepBase::GenerateJavaTransferTypesL(RTransferTypeArray& aTransferTypes, TJavaTransferType aJavaTransferType)
		/**
		For each data owner in iDataOwners, this method creates CSBGenericTransferTypes
		based on the TTransferDataType passed. One transfer type is created per drive that
		the data owner in question has data on.
		
		@internalComponent
		@released
		
		@param aTransferTypes - Array of transfer types.
		@param aJavaTransferType - Trasnfer data type
		*/
		{
		aTransferTypes.ResetAndDestroy();
		
		TInt length = iDriveList.Length();
		for (TInt driveCount = 0; driveCount < length; driveCount++)
			{
			if (iDriveList[driveCount])
				{
				TInt count = iDataOwners.Count();
				for (TInt i=0; i < count; i++)
					{
					switch (iDataOwners[i]->Identifier().DerivedTypeL())
						{
						case EJavaDerivedType:
							{
							if ((aJavaTransferType == EJavaMIDlet && (iDataOwners[i]->CommonSettings() & EHasSystemFiles)) || aJavaTransferType == EJavaMIDletData)
								{
								CSBJavaId* javaId = CSBJavaId::NewL(&(iDataOwners[i]->Identifier()));
								CleanupStack::PushL(javaId);
								const TDesC& suiteHash = javaId->SuiteHashL();
								CSBJavaTransferType* javaType = CSBJavaTransferType::NewL(suiteHash, TDriveNumber(driveCount), aJavaTransferType);
								CleanupStack::PushL(javaType);
								aTransferTypes.AppendL(javaType);
								CleanupStack::Pop(javaType);
								CleanupStack::PopAndDestroy(javaId);
								}//if
							} 
							break;
						} // switch
					} //for
				} // if
			} // for
		}
		
	
	void CBURTestStepBase::GeneratePublicTransferTypesL(RPointerArray<CSBGenericDataType>& aTransferTypes)
		/**
		For each data owner in iDataOwners, this method creates CSBGenericTransferTypes
		based on the TTransferDataType passed. One transfer type is created per drive that
		the data owner in question has data on.
		
		@internalComponent
		@released
		
		@param aTransferTypes - Array of transfer types.
		@param aDataType - Trasnfer data type, passive base, active base, etc.
		*/
		{
		aTransferTypes.ResetAndDestroy();

		TInt count = iDataOwners.Count();
		for (TInt i = 0; i < count; i++)
			{
			switch (iDataOwners[i]->Identifier().DerivedTypeL())
				{
				case EPackageDerivedType:
				case ESIDDerivedType:
					if (iDataOwners[i]->PassiveSettings() & EHasPublicFiles)
						{
						const TDesC8& desc = iDataOwners[i]->Identifier().Externalise();
						CSBGenericDataType* idType = CSBGenericDataType::NewL(desc);
						CleanupStack::PushL(idType);
						aTransferTypes.AppendL(idType);
						CleanupStack::Pop(idType);
						} //if
					break;
				} // switch
			}
		}
	
	
	void CBURTestStepBase::SupplyDataL(RTransferTypeArray& aTransferTypes)
		/**
		Retrieves data from the storage handler and supplies to the backup client for 
		the given transfer type
		
		@internalComponent
		@released
		
		@param aTransferTypes - Array of transfer types.
		*/
		{
		TInt transferTypesCount = aTransferTypes.Count();
		for(TInt currentIndex = 0; currentIndex < transferTypesCount; ++currentIndex)
			{
			TInt error;		
			TInt increments = 0;
			TInt incrIndex = 1;
			
			TSBDerivedType dataType = aTransferTypes[currentIndex]->DerivedTypeL();
			
			// check for incremental data
			switch(dataType)
				{
				case ESIDTransferDerivedType:
					{
					CSBSIDTransferType* type = CSBSIDTransferType::NewL(aTransferTypes[currentIndex]);
					CleanupStack::PushL(type);
					
					_LIT(KSid, "Supplying data for ID: ");
					LogWithSID(LOG_LEVEL3, KSid, type->SecureIdL());
					
					if (type->DataTypeL() == EActiveIncrementalData || type->DataTypeL() == EPassiveIncrementalData)
						{
						increments = iStorageManager->TotalIncFiles(type);
						if (!increments)
							{
							_LIT(KErrorText2, "No increments found");
							Log(LOG_LEVEL4, KErrorText2);
							// cleanup
							CleanupStack::PopAndDestroy(type);
							continue;
							}
						else 
							{
							_LIT(KIncFound, "Number of increments found: ");
							LogWithNum(LOG_LEVEL4, KIncFound, increments);
							} // else if
						} // if
					CleanupStack::PopAndDestroy(type);
					}
					break;
				case EPackageTransferDerivedType:
					{
					// package stuff
					CSBPackageTransferType* pType = CSBPackageTransferType::NewL(aTransferTypes[currentIndex]);
					CleanupStack::PushL(pType);
					_LIT(KPid, "Supplying data for Package ID: ");
					LogWithSID(LOG_LEVEL3, KPid, pType->PackageIdL());
					CleanupStack::PopAndDestroy(pType);
					}
					break;
				case EJavaTransferDerivedType:
					{
					// java stuff
					CSBJavaTransferType* jType = CSBJavaTransferType::NewL(aTransferTypes[currentIndex]);
					CleanupStack::PushL(jType);
					_LIT(KJid, "Supplying data for Java SuiteHash: ");
					LogWithText(LOG_LEVEL3, KJid, jType->SuiteHashL());
					CleanupStack::PopAndDestroy(jType);
					}
					break;
				
				} // switch
			TBool runAgain = ETrue;		
			do 
				{
				TBool finished;				
				TPtr8* bufferPtr = NULL;
				TRAP(error, bufferPtr = &iBackupClient->TransferDataAddressL());
				if (error != KErrNone)
					{
					_LIT(KErrTDA, "Error getting TransferDataAddress: ");
					LogWithNum(LOG_LEVEL4, KErrTDA, error);
					break;
					}
				TRAP(error, iStorageManager->RetrieveDataL(aTransferTypes[currentIndex], *bufferPtr, finished, incrIndex));
				if (error != KErrNone)
					{
					iStorageManager->Reset();
					if (error == KErrPathNotFound || error == KErrNotFound)
						{
						_LIT(KErrorText2, "Data doesn't exists");
						Log(LOG_LEVEL4, KErrorText2);
						}
					else
						{
						_LIT(KErrorText1, "Error reading from archive: ");
						LogWithNum(LOG_LEVEL4, KErrorText1, error);
						iFailures++;
						}
					break;
					} 
				else
					{
					_LIT(KLogBytes, "Number of bytes to send: ");
					LogWithNum(LOG_LEVEL4, KLogBytes, bufferPtr->Length());	
			
					TRAP(error, iBackupClient->SupplyDataL(*aTransferTypes[currentIndex], finished));
					if (error != KErrNone)
						{
						_LIT(KErrorText2, "Error supplying data to backup server: ");
						LogWithNum(LOG_LEVEL4, KErrorText2, error);
						iFailures++;
						break;
						}
					else
						{
						_LIT(KSuccess, "Operation finished successfully");
						Log(LOG_LEVEL4, KSuccess);
						}
					
					} // else
					
				if ((finished && !increments) || (incrIndex == increments && finished)) // if finished 
					{
					runAgain = EFalse;
					}
				else if (finished && incrIndex < increments) // is incremental data move to next increment
					{
					incrIndex++;
					runAgain = ETrue;
					}
				else 
					{
					runAgain = ETrue;
					}
				} // do
				while (runAgain);
			} // for
			
			if (!iFailures)
				{
				_LIT(KComp, "No errors found");
				Log(LOG_LEVEL3, KComp);
				}
		// Do not delete bufferPtr, as it gets deleted by the iBackupClient
		}
	
	TDriveList CBURTestStepBase::StringToDriveListL(TPtrC apText)
		/**
		@internalComponent
		@released
		
		@param apText - String that needs to be converted into a TDriveList.
		@return TDriveList with all the bytes set to 1 where a corresponding drive letter 
				was found in apText
		*/
		{
		TInt error;
		RFs fsSession;
		CleanupClosePushL(fsSession);
		error = fsSession.Connect();
		if (error != KErrNone)
			{
			User::Leave(error);
			}
		
		TUint8 str[KMaxDrives];
		for (TInt i = 0; i < KMaxDrives; ++i)
			{ 
			str[i] = 0; // Initialise to zero
			}
		
		TInt length = apText.Length();
		for (TInt i = 0; i < length; ++i)
			{
			TInt pos;
			fsSession.CharToDrive(apText.Ptr()[i], pos);
			str[pos] = 1;
			}
		
		TDriveList driveList;
		//driveList.FillZ(); // initialise to zeros
		TPtr8 ptr(&str[0], KMaxDrives, KMaxDrives);
		driveList.Copy(ptr);
		
		CleanupStack::PopAndDestroy();
		return driveList;
		}
		
	
	void CBURTestStepBase::SaveDataOwners()
		/**
		Saves data owners to storage Manager
		*/
		{
		TInt count = iDataOwners.Count();
		for (TInt i =0; i < count; i++)
			{
			TRAPD(err, iStorageManager->SaveDataOwnerL(*iDataOwners[i]));
			if (err != KErrNone)
				{
				_LIT(KDataOwnerErr, "Error saving Data Owner: ");
				LogWithNum(LOG_LEVEL2, KDataOwnerErr, err);
				}
			}
		}
	
	void CBURTestStepBase::CheckSIDStatusL(RTransferTypeArray& aTransferTypes, RTransferTypeArray& aReadyArray)
		/**
		Checks the readyness status for Active Data Owner. The ones are ready put into ready array and removed from the original
		
		@param aTransferTypes - TransferTypes to check for status
		@param aReadyArray - array of transfer types where dataowners are ready
		*/
		{
		aReadyArray.ResetAndDestroy();
		if (aTransferTypes.Count())
			{
			// used for getting statuses
			RSIDStatusArray statusArray;
			CleanupClosePushL(statusArray);
			// create array of sid for status request
			for (TInt i =0; i< aTransferTypes.Count(); i++)
				{
				TSecureId id = NULL;
				TSBDerivedType type = aTransferTypes[i]->DerivedTypeL();
				if (type == ESIDTransferDerivedType)
					{
					CSBSIDTransferType* sidType = CSBSIDTransferType::NewL(aTransferTypes[i]);
					CleanupStack::PushL(sidType);
					id = sidType->SecureIdL();
					CleanupStack::PopAndDestroy(sidType);
					statusArray.Append(TDataOwnerAndStatus(id, EUnset, KErrNone));	
					}
				} //for
						
			// get status for data owners	
			iBackupClient->SIDStatusL(statusArray);
			
			// check which sids ready
			TInt total = statusArray.Count();
			for (TInt j=0; j < total; j++)
				{
				TDataOwnerAndStatus test = statusArray[j];
				if (statusArray[j].iStatus == EDataOwnerReady)
					{
					aReadyArray.Append(aTransferTypes[j]);
					statusArray.Remove(j);
					aTransferTypes.Remove(j);
					j--;
					total--;
					}
				else if (statusArray[j].iStatus == EDataOwnerReadyNoImpl || statusArray[j].iStatus == EDataOwnerFailed || statusArray[j].iStatus == EDataOwnerNotFound)
					{
					LogWithSID(LOG_LEVEL3, _L("Data Owner Failed, NotFound or Has No implementation : "),statusArray[j].iSID.iId);
					statusArray.Remove(j);
					delete aTransferTypes[j];
					aTransferTypes[j] = NULL;
					aTransferTypes.Remove(j);
					j--;
					total--;
					}
				} //for	
			statusArray.Reset();
			CleanupStack::PopAndDestroy(&statusArray);
			} // if
		}
		
	TSecureId CBURTestStepBase::ExtractIDL(CDataOwnerInfo& aDataOwner)
		/**
		Gets Secure ID or UID from Data Owner
		
		@param aDataOwner Data Owner to get ID from
		
		@return TSecureId Secure ID of the data owner
		*/
		{
		TSecureId id = NULL;
		if (aDataOwner.Identifier().DerivedTypeL() == ESIDDerivedType)
			{
			CSBSecureId* secureId = CSBSecureId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(secureId);
					
			id = secureId->SecureIdL();
												
			CleanupStack::PopAndDestroy(secureId);
			} // if				
		else if (aDataOwner.Identifier().DerivedTypeL() == EPackageDerivedType)
			{
			CSBPackageId* pkgId = CSBPackageId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(pkgId);
			
			// package id		
			id = pkgId->PackageIdL();
				
			CleanupStack::PopAndDestroy(pkgId);
			} // else if
		return id;
		}
		
	HBufC* CBURTestStepBase::ExtractJavaIDL(CDataOwnerInfo& aDataOwner)
		/**
		Gets Suite Hash ID from the Data Owner
		
		@param aDataOwner - Data Owner to get suite hash from
		
		@return HBufC* pointe to a suite hash
		*/
		{
		HBufC* jid = NULL;
		if (aDataOwner.Identifier().DerivedTypeL() == EJavaDerivedType)
			{
			CSBJavaId* javaId = CSBJavaId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(javaId);
			
			const TDesC& suiteHash = javaId->SuiteHashL();
			jid = HBufC::NewLC(suiteHash.Size());
			*jid = suiteHash;
			CleanupStack::Pop(jid);
			CleanupStack::PopAndDestroy(javaId);
			}
		return jid;
		}
		
	void CBURTestStepBase::CheckValidRegistrationL()
		/**
		Checks the list of data owners returned from the device for the validity of the registration file.
		If registration is invalid the data owner is removed from the list and not backed up or restored.
		*/
		{
		for (TInt i = 0; i < iDataOwners.Count(); i++)
			{
			switch (iDataOwners[i]->Identifier().DerivedTypeL())
				{
				case ESIDDerivedType:
					{
					_LIT(KFoundSid, "Sid in the list: ");
					LogWithSID(LOG_LEVEL3, KFoundSid, ExtractIDL(*iDataOwners[i]));
					}
					break;
		
				case EPackageDerivedType:
					{
					_LIT(KFoundPid, "Package in the list: ");
					LogWithSID(LOG_LEVEL3, KFoundPid, ExtractIDL(*iDataOwners[i]));
					}
					break;
				
				case EJavaDerivedType:
					{	
					HBufC* suiteHash = ExtractJavaIDL(*iDataOwners[i]);
					_LIT(KJava, "Java ID found: ");
					LogWithText(LOG_LEVEL3, KJava, *suiteHash);
					delete suiteHash;
					suiteHash = NULL;
					}
					break;
				default:
					_LIT(KLogErr, "Not supported type");
					Log(LOG_LEVEL3, KLogErr);
					break;
				}
			
			TCommonBURSettings settings = iDataOwners[i]->CommonSettings();
			// check if registration file was parsed correctly
			if (!(settings & EPassiveBUR))
				{
				if (!(settings & EActiveBUR))
					{
					if (!(settings & EHasSystemFiles))
						{
						delete iDataOwners[i];
						iDataOwners[i] = NULL;
						iDataOwners.Remove(i);
						i--;
						if (iIsPartial)
							{
							iFailures++;
							}
						_LIT(KBadReg, "*** Error: Bad Registration file for ID above ^^^");
						Log(LOG_LEVEL3, KBadReg);
						}
					
					}//if
				}//if
			}// for
		}
		
	void CBURTestStepBase::DoSupplyL(TTransferDataType aDataType)
		/**
		@internalComponent
		@released
		*/
		{
		GenerateSIDTransferTypesL(iTransferTypes, aDataType);
		if (iTransferTypes.Count())
			{
			// TODO: need to check weather we can supply snapshot without waiting
			if (aDataType == EActiveSnapshotData || aDataType == EActiveBaseData || aDataType == EActiveIncrementalData)
				{
			 	// used for sids which are ready
				RTransferTypeArray readyArray;
				CleanupClosePushL(readyArray);
					
				for (TInt i=0; i < KRetries;)
					{
					CheckSIDStatusL(iTransferTypes, readyArray);
											
					if (readyArray.Count()) // dataowners ready
						{
						// ========= Supply Data ================
						SupplyDataL(readyArray);		
						} 
					else if (iTransferTypes.Count()) // data owners not ready
						{
						User::After(KDelay);
						i++;
						}
					else // finished with all sids
						{
						break;
						}
					} // for
				if (iTransferTypes.Count())
					{
					iFailures++;
					_LIT(KLogNoTrans, "***Error: Some Data Owners were Not Ready or Failed to Connect");
					Log(LOG_LEVEL3, KLogNoTrans);
					}
				readyArray.ResetAndDestroy();
				CleanupStack::PopAndDestroy(&readyArray);
				} //if
			else 
				{
				SupplyDataL(iTransferTypes);
				}	
			} //if
		}
		
	// CBURActiveObject //
	
	/**
	Standard Symbian OS Constructor
	@return pointer to newly allocated CBURActiveObject object
	*/
	CBURActiveObject* CBURActiveObject::NewL()
		{
		CBURActiveObject* self = new(ELeave) CBURActiveObject();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}
		
	/**
	C++ Standard destructor
	*/
	CBURActiveObject::~CBURActiveObject()
		{
		Cancel();
		delete iActiveScheduler;
		}
	
	/**
	C++ Standard Constructor
	*/
	CBURActiveObject::CBURActiveObject()
		: CActive(EPriorityNormal), iError(KErrNone)
		{
		}
	
	/**
	Symbian OS 2nd phase constructor
	*/
	void CBURActiveObject::ConstructL()
		{
		iActiveScheduler = new (ELeave) CActiveSchedulerWait();
		CActiveScheduler::Add(this);		
		}
	
	/**
	CActive::DoCancel() implementation
	Stops ActiveSchedulerWait if it is Started
	*/
	void CBURActiveObject::DoCancel()
		{
		if (iActiveScheduler->IsStarted())
			{
			iActiveScheduler->AsyncStop();
			}
		}
	
	/**
	Waits for the Status to complete, remembers the iStatus code and calls AsyncStop
	*/
	void CBURActiveObject::RunL()
		{
		iError = iStatus.Int();
		iActiveScheduler->AsyncStop();
		}
	/**
	CActive::RunError() implementation
	@param aError error code
	@return KErrNone
	*/
	TInt CBURActiveObject::RunError(TInt aError)
		{
		iError = aError;
		Cancel();
		return KErrNone;
		}
	
	/**
	Starts and active object and Scheduler, the call will be complete when RunL is called
	*/
	void CBURActiveObject::StartL()
		{
		if (IsActive())
			{
			User::Leave(KErrInUse);
			}
		iStatus = KRequestPending;
		SetActive();
		iActiveScheduler->Start();
		}
	/**
	Returns an error/completing code of the iStatus
	*/
	TInt CBURActiveObject::Error()
		{
		return iError;
		}
		
	}	// end namespace

