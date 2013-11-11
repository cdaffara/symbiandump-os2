#ifndef __T_LANGUTILS_H__
#define __T_LANGUTILS_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>
#include <baclipb.h>
#include <f32file.h>
#include <s32strm.h>
#include <s32file.h>
#include <e32lang.h>
#include <langutil.h>
#include "LangUtilImpl.h"
#include "T_LangUtilsTestShareFunc.h"

class CT_LANGUTILS : public CTestStep
    {
public:
    CT_LANGUTILS();
protected:  
    TVerdict doTestStepL();
private:
    void TestLANGUTILS();
    void TestGetDowngradePathL();
    void TestGetEquivalentLanguageList();
    void TestNearestLanguageFileV2(RFs& aFs);
    void CopyTestFiles();
    void CreateTestFiles2(RFs &aFs);
    void DeleteTestFiles2(RFs &aFs);
    void DeleteTestFiles();
    void DeleteTestFile(const TDesC& aFullName);
    void DeleteTestDir(const TDesC& aFullName);
    };

_LIT(KTestStep_T_LANGUTILS, "T_LANGUTILS");

#endif
