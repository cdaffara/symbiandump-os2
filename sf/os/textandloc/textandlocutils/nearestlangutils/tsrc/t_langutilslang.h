#ifndef __T_LANGUTILSLANG_H__
#define __T_LANGUTILSLANG_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_LANGUTILSLANG : public CTestStep
    {
public:
    CT_LANGUTILSLANG();
protected:  
    TVerdict doTestStepL();
private:
    void TestLANGUTILSLANG();
    void TestNearestLang();
    void TestIdeaLang();
    void CreateTestFiles();
    };

_LIT(KTestStep_T_LANGUTILSLANG, "T_LANGUTILSLANG");

#endif
