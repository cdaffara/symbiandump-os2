Contents
--------

1.  How do I internationalize a Word template?
    1.1  For Word with a single "normal" template?
    1.2  For Word with multiple templates?

------------------------------------------------------------------------

1.  How do I internationalize a Word template?
    1.1 For Word with a single "normal" template?

It depends on the version of Word, if it's word with templates

You write a tool to do this (please!):

Get WORD and WPENG, and associated components.

WPENG:

Remove the comments from CWordModel::ConstructL() around this code:
	iStyleList=NULL;
	iStyleShortCutList=NULL;
	iStyleList=CStyleList::NewL();
	iStyleShortCutList=new(ELeave) CStyleShortCutList(3);
	for (TInt ii=0;ii<3;ii++)
		{
		CParagraphStyle* style=CParagraphStyle::NewL(*iParaFormatLayer,*iCharFormatLayer);
		RParagraphStyleInfo info(style);
		iStyleList->AppendL(&info);
		}
	iStyleList->At(0).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(0).iStyle->iName=_L("Heading 1");
	iStyleList->At(1).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(1).iStyle->iName=_L("Heading 2");
	iStyleList->At(2).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(2).iStyle->iName=_L("Heading 3");
	iStyleShortCutList->AppendL('1');
	iStyleShortCutList->AppendL('2');
	iStyleShortCutList->AppendL('3');
	iNormalStyleShortCut='N';

Rename "Heading" and shortcuts above to your desired heading and
shortcut text.

WORD:

In CWordDocument::NewDocumentL()

comment out these two lines:

	LocateTemplateL(templateFileName);
	//
	DoNewDocumentL(templateFileName);

Start Word in the emulator.  When Word has opened, close it straight
away.  Open the document created (probably C:\Documents\Word) in word.
You are now fine to edit the style information, the text should be
OK.

Don't forget all the other Word settings, such as Object view (set to
ICONIC by default - should be glass door) and printer settings (set to
Printer via PC for normal font information)

------------------------------------------------------------------------

    1.2 For Word with a single "normal" template?

This hasn't been documented yet, though it shouldn't be radically
different from the above.  Use your initiative.

