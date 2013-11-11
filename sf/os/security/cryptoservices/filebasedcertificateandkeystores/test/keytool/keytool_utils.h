/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef _Keytool_utils_h_
#define _Keytool_utils_h_
#include <e32std.h>

#include <mctkeystore.h>
#include <signed.h>
#include <cctcertinfo.h>


_LIT(KSWInstall, "SWInstall");
_LIT(KSWInstallOCSP, "SWInstallOCSP");
_LIT(KMidletInstall, "MidletInstall");
_LIT(KTls, "SSL/TLS");
_LIT(KTOcsp, "OCSP Test");
_LIT(KUnknown, "Unknown Applicability!");

const TInt swinstalluid = 0x100042AB;
const TInt swinstallocspuid = 0x1000A8B6;
const TInt midletinstalluid = 0x101F9B28;
const TInt tlsuid = 0x1000183D;
const TInt tocspuid = 0x1000A405;

const TInt KUidSecurityKeytool= 0x10281c32;

/**
 * A bunch of assorted useful functions for the keytool.
 */
class KeyToolUtils 
	{
public:

	/**
	 * Given a <code>CCTKeyInfo</code> object is prints its
	 * contents in a human readable format.
	 *
	 * @param aKey The <code>CCTKeyInfo</code> hgolding the information
	 *				we are interested in.
	 */
	static void PrintKeyInfoL(const CCTKeyInfo& aKey, TBool aIsDetailed = EFalse, TBool aPageWise = EFalse);	

	/**
	 * Prints a human readable translation of the given key usage code.
	 *
	 * @param aUsage The key usage we want to print.
	 * @param aConsole The console where the output goes.
	 */
	static void PrintUsageL(TUint aUsage);

	/**
	 * Given a key access encoded as an integer (we are talking
	 * <code>CKeyInfoBase::EKeyAccess</code> anyway) it returns
	 * an equivalent human understandable descriptor.
	 */
	static HBufC* KeyAccessDesLC(TInt aAccess);

	/**
	 * Given an algorithm identifier returns an equivalent human 
	 * readable descriptor.
	 */
	static HBufC* AlgorithmDesLC(CCTKeyInfo::EKeyAlgorithm aAlgorithm);

	/**
	 * Given a list of keys as returned from keystore->list() method
	 * and a label of a key we are interested in, it returns the corresponding
	 * key info.
	 */	
	static CCTKeyInfo* findKey(RMPointerArray<CCTKeyInfo>& aKeyList, TDesC& aLabel);
	
	/**
	 * Pretty prints an octet string.
	 *
	 * @param aString The octet string to be printed.
	 * @param aConsole The console where the output goes.
	 */
	static void WriteOctetStringL(const TDesC8& aString);

	/**
	 * Pretty prints the human readable description of a given error code.
	 *
	 * @param aError The error code we want to print.
	 * @param aConsole The console where the output goes.
	 */
	static void WriteErrorL(TInt aError); 
	
	/**
	 * Parses the human understandable description of a key usage 
	 * and returns the corresponding numeric code.
	 *
	 * @param aUsage The string containing the key usage.
	 * @return The corresponding code for the usage.
	 */
	static TKeyUsagePKCS15 ParseKeyUsage(TPtrC aUsage);
	
	static CCTKeyInfo::EKeyAccess ParseKeyAccess(TPtrC aAccess);
	
	static void PrintCertInfoL(CCTCertInfo& aCertInfo, CCertificate& aCertificate, RArray<TUid> aApps, TBool aTrusted, TBool aIsDetailed = EFalse,TBool aPageWise = EFalse);

	static const TDesC& Uid2Des(TUid aUid);

	static RArray<CCTKeyInfo*> MatchKey(RMPointerArray<CCTKeyInfo>& aKeyList, TDesC& aLabel);

  	static void FilterCertsL(RMPointerArray<CCTCertInfo>& aCertList, TDesC& aLabel);
  	
  	static void FilterCertsL(RMPointerArray<CCTCertInfo>& aCertList, TCertificateOwnerType& aOwnerType);
  	
  	static void PrintInfoL(const TDesC& aValue,TBool aPageWise = EFalse);
  	
  	static void SetConsole(CConsoleBase* aConsole);

	static void SetFile(RFile* aFile);
	
	static TBool DoesFileExistsL(const RFs& aFs, const TDesC& aFileName);
	
	static CConsoleBase *iConsole;

	static RFile* iFile;
	};
#endif
