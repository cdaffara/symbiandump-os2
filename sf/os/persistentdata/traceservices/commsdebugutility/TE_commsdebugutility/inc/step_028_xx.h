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

#include <comms-infras/commsdebugutility.h>



#if (!defined __STEP_028_XX_H__)
#define __STEP_028_XX_H__

class CFloggerTest028_Sync_Setup : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Setup();
	~CFloggerTest028_Sync_Setup();


	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};



class CFloggerTest028_Sync_ConWriteUni : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteUni();
	~CFloggerTest028_Sync_ConWriteUni();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};

class CFloggerTest028_Sync_ConWriteUniBound : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteUniBound();
	~CFloggerTest028_Sync_ConWriteUniBound();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
	
	
class CFloggerTest028_Sync_ConWriteFormatEUni : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteFormatEUni();
	~CFloggerTest028_Sync_ConWriteFormatEUni();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};

class CFloggerTest028_Sync_ConWriteFormatEUniBound : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteFormatEUniBound();
	~CFloggerTest028_Sync_ConWriteFormatEUniBound();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	};
	
	
class CFloggerTest028_Sync_ConWriteFormatV8Bit : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteFormatV8Bit();
	~CFloggerTest028_Sync_ConWriteFormatV8Bit();

    void DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};

class CFloggerTest028_Sync_ConWriteFormatV8BitBound : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConWriteFormatV8BitBound();
	~CFloggerTest028_Sync_ConWriteFormatV8BitBound();

    void DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	};
	
class CFloggerTest028_Sync_Static_WriteUni : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_WriteUni();
	~CFloggerTest028_Sync_Static_WriteUni();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};

class CFloggerTest028_Sync_Static_WriteUniBound : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_WriteUniBound();
	~CFloggerTest028_Sync_Static_WriteUniBound();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	};
	
	
class CFloggerTest028_Sync_Static_WriteFormatVUni : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_WriteFormatVUni();
	~CFloggerTest028_Sync_Static_WriteFormatVUni();

    void DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};

class CFloggerTest028_Sync_Static_WriteFormatVUniBound : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_WriteFormatVUniBound();
	~CFloggerTest028_Sync_Static_WriteFormatVUniBound();

    void DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	};
	
	
	
class CFloggerTest028_Sync_Static_HexDump : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_HexDump();
	~CFloggerTest028_Sync_Static_HexDump();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};
	
	
class CFloggerTest028_Sync_ClearLog : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ClearLog();
	~CFloggerTest028_Sync_ClearLog();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};
	

class CFloggerTest028_Sync_Binary : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Binary();
	~CFloggerTest028_Sync_Binary();

	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;
	
	};
	
	
class CFloggerTest028_Sync_ConMultiple : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConMultiple();
	~CFloggerTest028_Sync_ConMultiple();

    void DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
   	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;   // a member so we don't have to pass to "DoTestWriteFormat"
	
	};
	

class CFloggerTest028_Sync_Static_Multiple : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_Multiple();
	~CFloggerTest028_Sync_Static_Multiple();

    void DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
   	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	
	};
	
class CFloggerTest028_Sync_ConMultiple2 : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_ConMultiple2();
	~CFloggerTest028_Sync_ConMultiple2();

    void DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...);
   	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	private:
	RFileLogger flogger;   // a member so we don't have to pass to "DoTestWriteFormat"
	
	};
	
class CFloggerTest028_Sync_Static_Multiple2 : public CTestStepFlogger
	{
	public:
	CFloggerTest028_Sync_Static_Multiple2();
	~CFloggerTest028_Sync_Static_Multiple2();

    void DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...);
   	TInt DoTestCheckWriteL();

	virtual enum TVerdict doTestStepL( void );	
	TInt executeStepL();
	TInt executeStepL(TBool);	
	
	};
#endif //(__STEP_028_XX_H__)
