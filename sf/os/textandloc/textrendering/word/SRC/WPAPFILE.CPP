/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <s32file.h>

#include <f32file.h>

#include <txtrich.h>

#include <frmprint.h>

#include <coeutils.h>

#include <clock.h>  // RMessageWindow::EMaxTextLength

#include <barsread.h>

#include <prnsetup.h>

#include <eikenv.h>
#include <techview/eikrted.h>
#include <eikfutil.h>
#include <techview/eikcfdlg.h>
#include <techview/eikon.rsg>

#include "WPAPPUI.H"
#include "WPDIALGS.H"
#include "WPDOC.H"
#include "WPPANIC.H"
#include "wpresources.h"
#include <word.rsg>
#include "WORD.HRH"

const TInt KExportTextWrapWidth=78;
const TInt KMaxTemplateMsgLen = KMaxFileName+32*2; // file name and a pre + post string


void CWordAppUi::CreateFileL(const TDesC& aFileName)
	{
	TFileName templateFileName=NormalTemplateFileNameL();
	CreateFileL(aFileName, templateFileName);
	}

void CWordAppUi::CreateFileL(const TDesC& aFileName, const TDesC& aTemplateFileName)
// Support a *switch file* create message from the shell
//
	{
	DoFileSaveL(EFalse);  // do not echo any file save info message
	Document()->DoFileNewL(aFileName, aTemplateFileName);
	iEikonEnv->UpdateTaskNameL();
	HandleModelChangeL();
	iRichEd->ResetUndo();
	iRichEd->SetBookmark();
	}


void CWordAppUi::CmdFileNewL()
// Get a filename and create that file
//
	{
	TFileName fileName;
	SetInitialPathL(fileName);
	TFileName templateFileName;
	CEikDialog* dialog=new(ELeave) CWordTemplateNewFileDialog(&fileName, templateFileName, TUidType(KNullUid, KUidAppDllDoc, KUidWordAppValue));
	if (dialog->ExecuteLD(R_WORD_TEMPLATE_FILE_NEW))  // confirms action if file already exists
		CreateFileL(fileName, templateFileName);
	}


void CWordAppUi::OpenFileL(const TDesC& aFileName)
// Supporta a *switch file* open message from the shell
//
	{
	DoFileSaveL(EFalse);  // do not echo any file save info message
	DoFileOpenL(aFileName);
	Document()->SetChanged(EFalse);
	iRichEd->ResetUndo();
	iRichEd->SetBookmark();
	}


void CWordAppUi::CmdFileOpenL()
// Get a filename from and open that file
//
	{
	TFileName filename;
	SetInitialPathL(filename);
	CEikFileOpenDialog* dialog=new(ELeave) CEikFileOpenDialog(&filename);
	dialog->RestrictToNativeDocumentFiles();
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		OpenFileL(filename);
	}


TFileName CWordAppUi::CurrentFilePath()const
	{
	TParsePtrC parser(Document()->Process()->MainDocFileName());
	return parser.DriveAndPath();
	}


void CWordAppUi::SetInitialPathL(TFileName& aFileName)const
// checks that the initial path exists, creating it if not
// if the path was read-only a default is used instead
	{
	aFileName=CurrentFilePath();
	TRAPD(ret,ConeUtils::EnsurePathExistsL(aFileName) );
	if (ret!=KErrNone)
		{// if the path is eg read-only default to C:
		TWordFilePath::GetWordDefaultFilePath(aFileName);		
		ConeUtils::EnsurePathExistsL(aFileName);
		}
	}

TFileName CWordAppUi::NormalTemplateFileNameL()const
// Returns the name of the normal template
//
	{
	TFindFile ff(iEikonEnv->FsSession());
	TFileName templateFileName;
	TWordFilePath::GetNormalTemplateFolderName(templateFileName);
	TFileName tempFileName;
	iEikonEnv->ReadResource(tempFileName, R_WORD_NORMAL_TEMPLATE_FILE_NAME);
	templateFileName.Append(tempFileName);
	TParsePtr parse(templateFileName);
	templateFileName[0] = 'Y';
	TInt err = ff.FindByDir(parse.NameAndExt(),parse.DriveAndPath());
	if(err)
		{
		// no normal template, search ROM for blank
		TWordFilePath::GetNormalTemplateFolderName(templateFileName);
		iEikonEnv->ReadResource(tempFileName, R_WORD_BLANK_TEMPLATE_FILE_NAME);
		templateFileName.Append(tempFileName);
		templateFileName[0] = 'Z';
		TParsePtr blankParse(templateFileName);
		User::LeaveIfError(ff.FindByDir(blankParse.NameAndExt(),blankParse.DriveAndPath()));
		}
	return ff.File();
	}

void CWordAppUi::DoFileOpenL(const TFileName& aFileName)
	{
	Document()->DoFileOpenL(aFileName,EFileWrite);
	iRichEd->ResetUndo();
	iRichEd->SetBookmark();
	iEikonEnv->UpdateTaskNameL();
	HandleModelChangeL();
	}


void CWordAppUi::CmdFileSaveL()
// Ascertain whether the document should be saved.
// If so, then save; otherwise do nothing.
//
	{DoFileSaveL(ETrue);}


void CWordAppUi::DoFileSaveL(TBool aEchoFileSaved)
// If the current document has changed, then save it.
// An info message confirmation is only required when not running as embedded.
//
	{
	if (iRichEd->IsReadOnly())
		return;  // all changes are lost - allow print model etc.. to be changed.
	if (DocHasChanged())
		{
		__ASSERT_DEBUG(iDocument->AppFileMode()&EFileWrite,Panic(EWordSaveReadOnlyFile));
		SaveL();
		iRichEd->SetBookmark();
		if (!iContainerAppUi && aEchoFileSaved)
			iEikonEnv->InfoMsg(R_EIK_TBUF_FILE_SAVED);
		}
	else
		{
		if (aEchoFileSaved)
			iEikonEnv->InfoMsg(R_WORD_DOCUMENT_UNCHANGED);
		}
	}


void CWordAppUi::CmdFileSaveAsL()
	{
	TFileName fileName;
	SetInitialPathL(fileName);
	CEikDialog* dialog=new(ELeave) CEikFileSaveAsDialog(&fileName,NULL,NULL);
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS))
		{
		if (Document()->FileNameExists(fileName))
			{
			CmdFileSaveL();
			return;
			}
		TEntry entry;
		if (!iEikonEnv->FsSession().Entry(fileName,entry))
			User::LeaveIfError(iEikonEnv->FsSession().Delete(fileName));
		Document()->DoFileSaveToNewL(fileName);
		iEikonEnv->InfoMsg(R_EIK_TBUF_FILE_SAVED);
		// set the filename label to read correctly
		iEikonEnv->UpdateTaskNameL();
		UpdateFileNameLabelL();
		SetReadOnly(EFalse);
		iRichEd->SetBookmark();
		}
	}


void CWordAppUi::CmdFileRevertL()
	{
	if (DocHasChanged())
		{
		if (ConfirmationDialogL(ERevertTitle,ERevertText))
			{
			Document()->DoFileRevertL();
			TRAPD(ret,
			HandleModelChangeL());  // resets document to not saved yet.
				if (ret!=KErrNone)
					{// Just clear the screen cos we have new content but cannot draw it.
					CWindowGc& gc=iEikonEnv->SystemGc();
					gc.Activate(*iRichEd->DrawableWindow());
					gc.Clear(iRichEd->TextView()->ViewRect());
					gc.Deactivate();
					User::Leave(ret);
					}
			Document()->SetChanged(EFalse);
			iRichEd->ResetUndo();
			iRichEd->SetBookmark();
			}
		}
	else
		iEikonEnv->InfoMsg(R_WORD_DOCUMENT_UNCHANGED);
	}

void CWordAppUi::CmdFileSaveAsTemplateL()
	{
	TFileName filename;
	CEikDialog* dialog=new(ELeave) CWordSaveAsTemplateDialog(filename, TUidType(KNullUid, KUidAppDllDoc, KUidWordAppValue));
	if (dialog->ExecuteLD(R_WORD_TEMPLATE_SAVE_AS))
		{
		iEikonEnv->BusyMsgL(R_WORD_TEMPLATE_SAVING_FILE,KFileBusyInitialDelayInMicroSeconds);
		TParse parse;
		parse.Set(filename,NULL,NULL);
		if (!EikFileUtils::FolderExists(parse.DriveAndPath()))
			iEikonEnv->FsSession().MkDirAll(parse.DriveAndPath());
		if(ConeUtils::FileExists(filename))
			iEikonEnv->FsSession().Delete(filename);
		Document()->DoFileSaveToNewNoSwitchL(filename);
		iEikonEnv->BusyMsgCancel();
		TFileName file = parse.Name(); // can CEikUnifiedFileNameSelector has guaranteed the extra will fit
		TBuf<32> msg;
		iEikonEnv->ReadResource(msg,R_EIK_TBUF_UNIFIED_PRE_DRIVE);
		file.Append(msg);
		file.Append(parse.Drive()[0]);
		iEikonEnv->ReadResource(msg,R_EIK_TBUF_UNIFIED_POST_DRIVE);
		file.Append(msg);
		iEikonEnv->ReadResource(msg,R_WORD_TEMPLATE_SAVED_MESSAGE);
		TBuf<KMaxTemplateMsgLen> fullmsg;
		fullmsg.Format(msg,&file);
		iEikonEnv->InfoMsg(fullmsg);
		}
	}

void CWordAppUi::CmdFileDeleteTemplateL()
	{
	TFileName filename=NormalTemplateFileNameL();
	CEikDialog* dialog=new(ELeave) CWordDeleteTemplateDialog(filename, TUidType(KNullUid, KUidAppDllDoc, KUidWordAppValue));
	if (dialog->ExecuteLD(R_WORD_TEMPLATE_DELETE))
		{
		if (filename.Length())
			iEikonEnv->FsSession().Delete(filename);
		}
	}

void CWordAppUi::CmdFileTextImportL()
	{
    TFileName filename=CurrentFilePath();
    CEikDialog* dialog=new(ELeave) CEikFileOpenDialog(&filename,R_WORD_TEXT_IMPORT_DIALOG_TITLE);
    if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		{
		iCoeEnv->WsSession().Flush();  // !! check this out
		iEikonEnv->BusyMsgL(R_EIK_TBUF_IMPORTING,KFileBusyInitialDelayInMicroSeconds);
		iRichEd->InsertFromTextFileL(filename,CPlainText::EOrganiseByParagraph);
		SetPaginationOutOfDate(ETrue);
		SetDocChanged();
		iEikonEnv->BusyMsgCancel();
		}
	}


void CWordAppUi::CmdFileTextExportL()const
	{
	TFileName filename=CurrentFilePath();
	filename.Append(_L(".txt"));
	ConeUtils::EnsurePathExistsL(filename);
	CEikDialog* dialog=new(ELeave) CEikFileSaveAsDialog(&filename,NULL,R_WORD_TEXT_EXPORT_DIALOG_TITLE);
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS))
		{
		iEikonEnv->BusyMsgL(R_EIK_TBUF_EXPORTING,KFileBusyInitialDelayInMicroSeconds);
		Text()->ExportAsTextL(filename,CPlainText::EOrganiseByParagraph,KExportTextWrapWidth); 
		iEikonEnv->BusyMsgCancel();
		}
	}

void CWordAppUi::CmdFileHtmlImportL()
	{
    TFileName filename=CurrentFilePath();
    CEikDialog* dialog=new(ELeave) CEikFileOpenDialog(&filename,R_WORD_HTML_IMPORT_DIALOG_TITLE);
    if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		{
		iCoeEnv->WsSession().Flush();
		iEikonEnv->BusyMsgL(R_EIK_TBUF_IMPORTING,KFileBusyInitialDelayInMicroSeconds);
		iRichEd->InsertFromHtmlFileL(filename);
		SetPaginationOutOfDate(ETrue);
		SetDocChanged();
		iEikonEnv->BusyMsgCancel();
		}
	}
