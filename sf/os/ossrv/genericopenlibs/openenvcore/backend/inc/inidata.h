/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : inidata.h
* Part of     : stdio server
* To read inifile.
* 
*
*/



#ifndef __INIDATA_H__
#define __INIDATA_H__

 #include <e32base.h>

 class CIniData : public CBase
 /*
  *
  * @publishedPartner
  * @test
  *
  * Defines the interface to acess to ini data file
  *
  * The basic functions, FindVar(), SetValue(), AddValue() and WriteToFileL()
  * Compulsory to call WriteToFileL() after calling any SetValue() or AddValue()
  */
     {
 public:
     // Constructor, pass in name of ini file to open
     // Default search path is 'c:\system\data' on target filesystem
     // ie. 'NewL(_L("c:\\system\\data\\ttools.ini"))' is equivalent
     // to 'NewL(_L("ttools.ini"))'
     static CIniData* NewL(const TDesC& aName);
     virtual ~CIniData();


     TBool FindVar(const TDesC &aSection,   // Section to look under
                         const TDesC &aKeyName,      // Key to look for
                         TPtrC &aResult);            // Buffer to store result

     TBool FindVar(const TDesC &aSection,   // Section to look under
                         const TDesC &aKeyName,      // Key to look for
                         TInt &aResult);             // Int ref to store result


 protected:
   		  CIniData();
   		  void ConstructL(const TDesC& aName);
 private:
     HBufC* iName;
     HBufC* iToken;
     TPtr iPtr;
     };

#endif /*__INIDATA_H__*/
