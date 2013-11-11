
*******************************************************************************
*************   Default name, hotkeys, menubar titles and toolbar resources   *
*******************************************************************************

RESOURCE TBUF { buf="Word"; } // default filename
	(Second resource in file).
	Default file name - should be same as app name.
R_WORD_HOTKEYS
	All the menu hotkeys (shortcut keys) for this app. Where there are two lists,
	the first contains unshifted keys, the second shifted keys. Translators should
	rearrange the items within each list into alphabetical order of the hotkeys in
	the target language - this will facilitate checking for any accidental
	duplicates. (Note that, within each list, every line must end with a comma,
	except for the last line which must not!)
R_WORD_MENUBAR
R_WORD_TOOLBAR
	All toolbar button label text is subject to a length restriction.
	eg, "Insert\nSketch" must fit on 2 lines, within a specific width restriction.
	-> The toolbar is a fixed width !
R_WORD_TOOLBAND
	The length of these buttons must not be changed



****************************************************************
*************   Menu resources            **********************
****************************************************************

R_WORD_FILE_MENU
R_WORD_FILEOPTIONS_MENU
	File menu cascade
R_WORD_PRINTEXTRA_MENU
	File menu - printing cascade
R_WORD_EDIT_MENU
R_WORD_EDITFIND_MENU
	Edit menu cascade
R_WORD_EDITOPTIONS_MENU
	Edit menu - object cascade
R_WORD_VIEW_MENU
R_WORD_INSERT_MENU
R_WORD_TEXT_MENU
R_WORD_PARAGRAPH_MENU
R_WORD_PARA_BULLETS_MENU
	Paragraph menu - bullets cascade
R_WORD_PARA_ALIGNMENT_MENU
	Paragraph menu - alignment cascade
R_WORD_TOOLS_MENU
R_WORD_TOOLS_INFRARED
	Tools menu - infrared beaming cascade


****************************************************************
*************   Dialog resources           *********************
****************************************************************

R_WORD_DIALOG_GOTO
R_WORD_DIALOG_BULLET
R_WORD_BUTTONS_BULLET
	Describes button text appearing on the r_word_dialog_bullet dialog
R_WORD_DIALOG_FORMAT_BULLET
	The 'format bullet character' dialog

R_WORD_DIALOG_STYLE_GALLERY
	The main style gallery dialog
R_WORD_BUTTONS_STYLEOPTION
	Describes button text appearing on the r_word_dialog_style_gallery dialog
R_WORD_DIALOG_STYLE_FORMAT
R_WORD_STYLE_FORMAT_MENU
R_WORD_BUTTONS_STYLEADDOPTION
	Describes button text on the r_word_dialog_style_format dialog

R_WORD_DIALOG_SPELL
	The spell checking progress dialog
R_WORD_BUTTONS_SPELL
	The buttons that appear on the spell checking progress dialog
R_WORD_DIALOG_SPELL_INCORRECT
	The unrecognised word dialog
R_WORD_DIALOG_SPELL_TOO_LONG
	The unregognised word dialog used when the word is too long to be displayed in the title of the dialog.
R_WORD_BUTTONS_SPELL_INCORRECT
	The buttons for the spell incorrect dialog
R_WORD_BUTTONS_SPELL_TOO_LONG
	The buttons for the spell too long dialog
R_WORD_DIALOG_SPELL_CORRECTION
	The correcting a word dialog
R_WORD_BUTTONS_SPELL_CORRECTION
	The buttons for the correcting a word dialog


R_WORD_DIALOG_VERIFY_PASSWORD
	The password protected dialog 
R_WORD_DIALOG_SET_PASSWORD
	The set password dialog

R_WORD_VIEW_PREF_ARRAY
	Do not reorder the contained items
	Displays the text at the top of the "view preferences" dialog.
	Indicates which non-printing characters have been 'turned on'
R_WORD_DIALOG_VIEW_PREFERENCES
	The view preferences dialog
R_WORD_DIALOG_OBJECT_PREFERENCES
	The object preferences dialog
R_WORD_DIALOG_ZOOM_SET
	The set zoom dialog

R_WORD_DIALOG_OUTLINE
	The document outline dialog
R_WORD_OUTLINE_BUTTONS
	The buttons on the document outline dialog
R_WORD_TEMPLATE_FILE_NEW
	The create new file dialog. The title is taken from a resource in eikon
R_WORD_TEMPLATE_DELETE
	The delete template dialog
R_WORD_TEMPLATE_SAVE_AS
	The save as template dialog

****************************************************************
*************   General string resources   *********************
****************************************************************

+++ file operations text +++
R_WORD_DOCUMENT_UNCHANGED
	Displayed when a file revert\save operation is attempted on a file that
	has not changed since it was last saved.
+++ template filename +++
R_WORD_NORMAL_TEMPLATE_FILE_NAME
	Used for the name of the normal template
R_WORD_BLANK_TEMPLATE_FILE_NAME
	Used for the name of the blank document template
+++ template messages +++
R_WORD_TEMPLATE_SAVING_FILE
	Used for the message while the template is being saved
R_WORD_TEMPLATE_DELETE_CONFIRMATION_PRE
	Used for confirm delete template dialog, this is the text that appears before the template name
	Max length 30 characters
R_WORD_TEMPLATE_DELETE_CONFIRMATION_POST
	Used for confrim delete template dialog, this is the text that appears after the template name
	Max length 30 characters
R_WORD_TEMPLATE_NOT_AVAILABLE
	Used for when a template has been saved but then for some reason does not exist
R_WORD_TEMPLATE_SAVED_MESSAGE
	Used for the message when the template has been saved. The %s inserts the template name.

+++ file operations text +++
R_WORD_TEXT_IMPORT_DIALOG_TITLE
	Used for title of the dialog when importing text
R_WORD_TEXT_EXPORT_DIALOG_TITLE
	Used for title of the dialog when exporting text
R_WORD_FILE_READ_ONLY
R_WORD_FILE_SAVE_READ_ONLY
R_WORD_FILE_CREATE_FILE
	Info message used when creating file from a template

+++ spelling dialogs text +++
R_WORD_SPELL_NO_SUGGESTIONS
	Text displayed in correcting a word dialog when there ar no suggestions for a word. maximum length is 30 characters
R_WORD_SPELL_CORRECTION
	the message displayed if the correction is the same as the unrecognised word

R_WORD_SPELL_UNRECOGNIZED_WORD
	The title for the unrecognised word dialog. max length 40

+++ Messages displayed at the end of spell checking to inform user of number of unrecognised words found +++
R_WORD_SPELL_ONE_UNRECOGNIZED_WORD
	maximum length is 32 characters
R_WORD_SPELL_MANY_UNRECOGNIZED_WORDS
	The %D is used to display the count.
R_WORD_SPELL_ONE_UNRECOGNIZED_WORD_IN_SELECTION
	Message for informmaximum length is 52 characters
R_WORD_SPELL_MANY_UNRECOGNIZED_WORDS_IN_SELECTION
	maximum length is 49 characters

R_WORD_SPELL_LOAD_DICTIONARY
	Used for Opening dictionaries message when run spell sheck is selected.
R_WORD_SPELL_USER_DICT_NOT_ENABLED
	The message that tells the user the personal dictionary is not being used
R_WORD_SPELL_DICT_NOT_AVAILABLE
	The message that tells the user that although the personal dictionary is selecected it could not be read or updated. 
R_WORD_SPELL_SELECTION
	r_word_dialog_spell dialog title (when actioned with a text selection)
R_WORD_SPELL_NOT_STARTED
	The message to the user if the spell checker could not be started.
R_WORD_SPELL_INFORMATION
	The title for the spell not started dialog
R_WORD_SPELL_NOTHING_TO_CHECK
	The message for being asked to check an empty document

+++ style dialogs text +++
R_WORD_STYLEFORMAT_TITLE_MODIFY
	Used for the title of the dialog to modify a style. maximum length is 24 characters
R_WORD_STYLEFORMAT_TITLE_ADD
	Used for the title of the dialog to add a new style.
R_WORD_NORMAL
	The name of the paragraph style name used for Normal style
R_WORD_NOSTYLENAME
	Message used when the style name is empty when adding or modifying a style.
R_WORD_STYLENAMEEXISTS
	Message used when the style name has been used already when adding or modifing a style.
R_WORD_CLOSE
	The text to replace the text on the cancel button in the style gallery dialog when a style has been added, deleted or modifiedThe maximum length is 20 characters
R_WORD_DELETE_STYLE_TITLE
	Title of the delete style dialog
R_WORD_HOTKEYEXISTS
	Message to inform user that the shortcut is in use by another style. maximum length is 48 characters
R_WORD_HOTKEY_INVALID
	Message when the shortcut key for the style is not a letter or number when adding or modifying a style
R_WORD_HOTKEY
	maximum length is 20 characters
R_WORD_STYLE_GALLERY_DESCRIPTIONS
	do not reorder the contained resources
	displayed in the description box of the main style gallery.
R_WORD_STYLE_REVERT_TITLE
	No longer used
R_WORD_STYLE_REVERT_CHAR
	No longer used
R_WORD_STYLE_REVERT_PARA
	No longer used

+++ miscellaneous text +++
R_WORD_FONT_UNITS
	appears in the font height top toolbar button
R_WORD_NO_WORDS
	used to display a count of the total number of words in a document.
R_WORD_NO_OUTLINE
	Message nothing to show in outline
R_WORD_NORMAL_OUTLINE
	Message when user tries to change outline level of normel style
R_WORD_OUTLINE
	Outlining message
R_WORD_EXPAND_OUTLINE
	Message displayed when open all button on outline dialog is selected
R_WORD_COLLAPSE_OUTLINE
	Message displayed when close all button on outline dialog is selected

+++ confirmation dialogs text +++
R_WORD_CONFIRMATION_TEXTS
	Do not reorder the contained resources
	This list contains the text for various "confirmation" dialogs,
	used throughout the word processor.
	The maximum length for each resource in this array is 80 characters.