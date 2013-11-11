/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* \page pt Test Harness for the certificate store manager
* <H2>Testing</H2>
* <H3>Test goals</H3>
* <P>We want to the entire API for CCertStore and CCertStoreManager. Some other classes
* will tested indirectly by these tests i.e. the other classes in certstore.h and the file
* certstore implementation.</P>
* <P>Test cases</P>
* IMPORT_C const RArray<TCertManClientInfo>& Clients() const;
* IMPORT_C void GetCACerts(RPointerArray<CCACertStoreEntry>& aEntries,
* const TCACertStoreFilter& aFilter, TRequestStatus& aStatus);
* IMPORT_C void GetUserCerts(RPointerArray<CUserCertStoreEntry>& aEntries,
* const TUserCertStoreFilter& aFilter, TRequestStatus& aStatus);
* IMPORT_C TInt StoreCount() const;
* IMPORT_C CCertStore& Store(TInt aIndex) const;
* <TABLE BORDER1>
* <TR>
* <TD>1. Opening, initializing and deleting the certificate store manager</TD>
* <TD>This series of tests open, initialize and delete the certificate store manager.</TD>
* <TD>IMPORT_C static CCertStoreManager* NewL(RFs& aFs, TBool aOpenForWrite);<BR>
* IMPORT_C static CCertStoreManager* NewLC(RFs& aFs, TBool aOpenForWrite);<BR>
* IMPORT_C ~CCertStoreManager();<BR>
* IMPORT_C void Initialise(TRequestStatus& aStatus);<BR></TD>
* </TR>
* <TR>
* <TD>1.1. Opening, initializing and deleting, usual steps.</TD>
* </TR>
* <TR>
* <TD>1.1.1. Creating and initializing the certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.1.2. Deleting the certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.2. Opening, initializing and trying to open another certificate manager</TD>
* </TR>
* <TR>
* <TD>1.2.1. Creating and initializing the certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.2.2. Creating and initializing another certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.2.3. Deleting the certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.2.4. Opening a certificate store manager</TD>
* </TR>
* <TR>
* <TD>1.2.5. Deleting certificate store manager</TD>
* </TR>
* <TR>
* <TD>2. Adding and removing clients</TD>
* <TD>IMPORT_C void AddClientL(const TCertManClientInfo& aClient);<BR>
* IMPORT_C void RemoveClientL(const TUid& aUid);</TD>
* </TD>
* </TR>
* <TR>
* <TD>2.1. Adding and retrieving a client</TD>
* </TR>
* <TR>
* <TD>2.1.1. Initializing the manager</TD>
* </TR>
* <TR>
* <TD>2.1.2. Adding a client</TD>
* </TR>
* <TR>
* <TD>2.1.3. Retrieving the clients</TD>
* </TR>
* <TR>
* <TD>2.1.4. Deleting the client</TD>
* </TR>
* <TR>
* <TD>2.1.5. Deleting the manager</TD>
* </TR>
* <TR>
* <TD>2.2. Adding and retrieving a client, closing the store between the adding and the
* retrieving</TD>
* </TR>
* <TR>
* <TD>2.2.1. Initializing the manager</TD>
* </TR>
* <TR>
* <TD>2.2.2. Adding a client</TD>
* </TR>
* <TR>
* <TD>2.2.3. Deleting the manager</TD>
* </TR>
* <TR>
* <TD>2.2.4. Initializing the manager</TD>
* </TR>
* <TR>
* <TD>2.2.5. Retrieving the clients</TD>
* </TR>
* <TR>
* <TD>2.2.6. Deleting the client</TD>
* </TR>
* <TR>
* <TD>2.2.7. Deleting the manager</TD>
* </TR>
* <TR>
* <TD>2.3. Adding and removing 2 clients</TD>
* </TR>
* <TR>
* <TD>2.3.1. Initializing the manager</TD>
* </TR>
* <TR>
* <TD>2.3.2. Adding a client (1001)</TD>
* </TR>
* <TR>
* <TD>2.3.3. Adding a client (1002)</TD>
* </TR>
* <TR>
* <TD>2.3.4. Retrieving the clients</TD>
* </TR>
* <TR>
* <TD>2.3.5. Deleting one of the client (1001)</TD>
* </TR>
* <TR>
* <TD>2.3.6. Retrieving the clients</TD>
* </TR>
* <TR>
* <TD>2.3.7. Deleting one of the client (1002)</TD>
* </TR>
* <TR>
* <TD>2.3.8. Retrieving the clients</TD>
* </TR>
* <TR>
* <TD>2.3.9. Deleting the manager</TD>
* </TR>
* <TR>
* <TD>2.4.1. Initializing the manager</TD>
* </TR>
* <TR>
* <TD>2.4.2. Deleting a client that doesn't exist</TD>
* </TR>
* <TR>
* <TD>2.4.3. Deleting the manager</TD>
* </TR>
* </TABLE>
* <H2>Testing with failures</H2>
* <H3>Introduction</H3>
* <P>The following test script is meant to test the correct behaviour when errors occur.</P>
* <H3>Test goals</H3>
* This is a list of specific error conditions we have to test for.
* <TABLE BORDER"1">
* <TR>
* <TD>Addition of a client</TD><TD>Addition to memory successfull but writing to the store
* fails</TD>
* </TR>
* </TABLE>
* <H3>Test cases</H3>
* <TABLE BORDER1>
* <TR>
* <TD>1</TD>
* </TR>
* </TABLE>
* 1) We initialize the store.
* 2) We get the clients and make sure there are none. This is to ensure that the test
* are meaningfull.
* 3) We add a client to the store but we make this operation fail after the memory cache
* is updated but before the stream is updated.
* 4) We get the clients and make sure that it is still empty.
* 5) We close the store manager.
* 6) We reopen it.
* 7) We get the clients and make sure that it is still empty.
* 8) We close the store.
*
*/


/**
 @file
*/
