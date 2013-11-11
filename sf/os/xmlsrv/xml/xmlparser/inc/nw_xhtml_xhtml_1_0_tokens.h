/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


/*
** NOTE:
**
**   We are currently hand editing this file and xhtml_1_0_dict.c.
**
**   However, I did use the dict_create.exe program in cxml\dictionary to generate
**   and then merge with these files.
**
**   Joel Parks 2002.08.27
**
** This file generated on Tue Aug 27 15:33:29 2002
**                        (coordinated universal time)
**
** Command line: dict_creator xhtml_1_0.dict xhtml_1_0_dict.c nw_xhtml_xhtml_1_0_tokens.h
*/

#ifndef _NW_Tempest_nw_xhtml_xhtml_1_0_tokens_h
#define _NW_Tempest_nw_xhtml_xhtml_1_0_tokens_h

#define NW_xhtml_1_0_PublicId 55

typedef enum NW_XHTML_1_0_ElementToken_0_e{
	NW_XHTML_ElementToken_body = 0x0005,
	NW_XHTML_ElementToken_head = 0x0006,
	NW_XHTML_ElementToken_html = 0x0007,
	NW_XHTML_ElementToken_title = 0x0008,
	NW_XHTML_ElementToken_kbd = 0x0009,
	NW_XHTML_ElementToken_meta = 0x000a,
	NW_XHTML_ElementToken_abbr = 0x000b,
	NW_XHTML_ElementToken_acronym = 0x000c,
	NW_XHTML_ElementToken_address = 0x000d,
	NW_XHTML_ElementToken_br = 0x000e,
	NW_XHTML_ElementToken_cite = 0x000f,
	NW_XHTML_ElementToken_dfn = 0x0010,
	NW_XHTML_ElementToken_div = 0x0011,
	NW_XHTML_ElementToken_em = 0x0012,
	NW_XHTML_ElementToken_h1 = 0x0013,
	NW_XHTML_ElementToken_h2 = 0x0014,
	NW_XHTML_ElementToken_h3 = 0x0015,
	NW_XHTML_ElementToken_h4 = 0x0016,
	NW_XHTML_ElementToken_h5 = 0x0017,
	NW_XHTML_ElementToken_h6 = 0x0018,
	NW_XHTML_ElementToken_p = 0x0019,
	NW_XHTML_ElementToken_q = 0x001a,
	NW_XHTML_ElementToken_strong = 0x001b,
	NW_XHTML_ElementToken_var = 0x001c,
	NW_XHTML_ElementToken_blockquote = 0x001d,
	NW_XHTML_ElementToken_pre = 0x001e,
	NW_XHTML_ElementToken_b = 0x001f,
	NW_XHTML_ElementToken_a = 0x0020,
	NW_XHTML_ElementToken_link = 0x0021,
	NW_XHTML_ElementToken_code = 0x0022,
	NW_XHTML_ElementToken_big = 0x0023,
	NW_XHTML_ElementToken_base = 0x0024,
	NW_XHTML_ElementToken_marquee = 0x0025,
	NW_XHTML_ElementToken_hr = 0x0026,
	NW_XHTML_ElementToken_span = 0x0027,
	NW_XHTML_ElementToken_i = 0x0028,
	NW_XHTML_ElementToken_small = 0x0029,
	NW_XHTML_ElementToken_u = 0x002a,
	NW_XHTML_ElementToken_dl = 0x002b,
	NW_XHTML_ElementToken_dt = 0x002c,
	NW_XHTML_ElementToken_dd = 0x002d,
	NW_XHTML_ElementToken_ol = 0x002e,
	NW_XHTML_ElementToken_ul = 0x002f,
	NW_XHTML_ElementToken_li = 0x0030,
	NW_XHTML_ElementToken_fieldset = 0x0031,
	NW_XHTML_ElementToken_input = 0x0032,
	NW_XHTML_ElementToken_form = 0x0033,
	NW_XHTML_ElementToken_label = 0x0034,
	NW_XHTML_ElementToken_select = 0x0035,
	NW_XHTML_ElementToken_option = 0x0036,
	NW_XHTML_ElementToken_optgroup = 0x0037,
	NW_XHTML_ElementToken_textarea = 0x0038,
	NW_XHTML_ElementToken_caption = 0x0039,
	NW_XHTML_ElementToken_table = 0x003a,
	NW_XHTML_ElementToken_td = 0x003b,
	NW_XHTML_ElementToken_th = 0x003c,
	NW_XHTML_ElementToken_tr = 0x003d,
	NW_XHTML_ElementToken_img = 0x003e,
	NW_XHTML_ElementToken_style = 0x003f
}NW_XHTML_1_0_ElementToken_0_t;


typedef enum NW_XHTML_1_0_ElementToken_1_e{
	NW_XHTML_ElementToken_samp = 0x0114,
	NW_XHTML_ElementToken_thead = 0x0115,
	NW_XHTML_ElementToken_tbody = 0x0116,
	NW_XHTML_ElementToken_tfoot = 0x0117,
	NW_XHTML_ElementToken_noscript = 0x0118,
	NW_XHTML_ElementToken_script = 0x0119,
	NW_XHTML_ElementToken_zzzunknown = 0x011a
}NW_XHTML_1_0_ElementToken_1_t;


typedef enum NW_XHTML_1_0_AttributeToken_0_e{
	NW_XHTML_AttributeToken_abbr = 0x0005,
	NW_XHTML_AttributeToken_accept_charset = 0x0006,
	NW_XHTML_AttributeToken_accept = 0x0007,
	NW_XHTML_AttributeToken_accesskey = 0x0008,
	NW_XHTML_AttributeToken_action = 0x0009,
	NW_XHTML_AttributeToken_align = 0x000a,
	NW_XHTML_AttributeToken_alink = 0x000b,
	NW_XHTML_AttributeToken_alt = 0x000c,
	NW_XHTML_AttributeToken_axis = 0x000e,
	NW_XHTML_AttributeToken_bgcolor = 0x0010,
	NW_XHTML_AttributeToken_border = 0x0011,
	NW_XHTML_AttributeToken_cellpadding = 0x0012,
	NW_XHTML_AttributeToken_char = 0x0013,
	NW_XHTML_AttributeToken_charoff = 0x0014,
	NW_XHTML_AttributeToken_charset = 0x0015,
	NW_XHTML_AttributeToken_checked = 0x0016,
	NW_XHTML_AttributeToken_cite = 0x0017,
	NW_XHTML_AttributeToken_class = 0x0018,
	NW_XHTML_AttributeToken_clear = 0x001a,
	NW_XHTML_AttributeToken_code = 0x001b,
	NW_XHTML_AttributeToken_codebase = 0x001c,
	NW_XHTML_AttributeToken_codetype = 0x001d,
	NW_XHTML_AttributeToken_color = 0x001e,
	NW_XHTML_AttributeToken_cols = 0x001f,
	NW_XHTML_AttributeToken_colspan = 0x0020,
	NW_XHTML_AttributeToken_dir = 0x0028,
	NW_XHTML_AttributeToken_disabled = 0x0029,
	NW_XHTML_AttributeToken_enctype = 0x002a,
	NW_XHTML_AttributeToken_height = 0x0030,
	NW_XHTML_AttributeToken_href = 0x0031,
	NW_XHTML_AttributeToken_hspace = 0x0033,
	NW_XHTML_AttributeToken_id = 0x0035,
	NW_XHTML_AttributeToken_label = 0x0037,
	NW_XHTML_AttributeToken_link = 0x003a,
	NW_XHTML_AttributeToken_maxlength = 0x003e,
	NW_XHTML_AttributeToken_media = 0x003f,
	NW_XHTML_AttributeToken_method = 0x0045,
	NW_XHTML_AttributeToken_multiple = 0x0046,
	NW_XHTML_AttributeToken_name = 0x0047,
	NW_XHTML_AttributeToken_noshade = 0x004a,
	NW_XHTML_AttributeToken_nowrap = 0x004b,
	NW_XHTML_AttributeToken_readonly = 0x0061,
	NW_XHTML_AttributeToken_rel = 0x0062,
	NW_XHTML_AttributeToken_rev = 0x0063,
	NW_XHTML_AttributeToken_rows = 0x0064,
	NW_XHTML_AttributeToken_rowspan = 0x0065,
	NW_XHTML_AttributeToken_selected = 0x006a,
	NW_XHTML_AttributeToken_size = 0x006c,
	NW_XHTML_AttributeToken_src = 0x006e,
	NW_XHTML_AttributeToken_start = 0x0070,
	NW_XHTML_AttributeToken_style = 0x0071,
	NW_XHTML_AttributeToken_tabindex = 0x0073,
	NW_XHTML_AttributeToken_title = 0x0076,
	NW_XHTML_AttributeToken_type = 0x0077,
	NW_XHTML_AttributeToken_valign = 0x0079,
	NW_XHTML_AttributeToken_value = 0x007a,
	NW_XHTML_AttributeToken_version = 0x007c,
	NW_XHTML_AttributeToken_vspace = 0x007e,
	NW_XHTML_AttributeToken_width = 0x007f,
	NW_XHTML_AttributeToken_none = 0x0085,
	NW_XHTML_AttributeToken_left = 0x0086,
	NW_XHTML_AttributeToken_right = 0x0087,
	NW_XHTML_AttributeToken_normal = 0x0088,
	NW_XHTML_AttributeToken_italic = 0x0089,
	NW_XHTML_AttributeToken_bold = 0x008a,
	NW_XHTML_AttributeToken_circle = 0x008b,
	NW_XHTML_AttributeToken_disc = 0x008c,
	NW_XHTML_AttributeToken_square = 0x008d,
	NW_XHTML_AttributeToken_always = 0x008e,
	NW_XHTML_AttributeToken_avoid = 0x008f,
	NW_XHTML_AttributeToken_center = 0x0090,
	NW_XHTML_AttributeToken_justify = 0x0091,
	NW_XHTML_AttributeToken_underline = 0x0092,
	NW_XHTML_AttributeToken_blink = 0x0093,
	NW_XHTML_AttributeToken_marquee = 0x0094,
	NW_XHTML_AttributeToken_top = 0x0095,
	NW_XHTML_AttributeToken_middle = 0x0096,
	NW_XHTML_AttributeToken_bottom = 0x0097,
	NW_XHTML_AttributeToken_baseline = 0x0098,
	NW_XHTML_AttributeToken_get = 0x009a,
	NW_XHTML_AttributeToken_post = 0x009b,
	NW_XHTML_AttributeToken_ltr = 0x009d,
	NW_XHTML_AttributeToken_rtl = 0x009e,
	NW_XHTML_AttributeToken_yes = 0x00a0,
	NW_XHTML_AttributeToken_no = 0x00a1,
	NW_XHTML_AttributeToken_auto = 0x00a2,
	NW_XHTML_AttributeToken_button = 0x00a3,
	NW_XHTML_AttributeToken_submit = 0x00a4,
	NW_XHTML_AttributeToken_reset = 0x00a5,
	NW_XHTML_AttributeToken__1 = 0x00ae,
	NW_XHTML_AttributeToken_all = 0x00b3,
	NW_XHTML_AttributeToken_text = 0x00b8,
	NW_XHTML_AttributeToken_password = 0x00b9,
	NW_XHTML_AttributeToken_checkbox = 0x00ba,
	NW_XHTML_AttributeToken_radio = 0x00bb,
	NW_XHTML_AttributeToken_hidden = 0x00bc
}NW_XHTML_1_0_AttributeToken_0_t;


typedef enum NW_XHTML_1_0_AttributeToken_1_e{
	NW_XHTML_AttributeToken_zzzunknown = 0x0105,
	NW_XHTML_AttributeToken_xml_stylesheet = 0x0106,
	NW_XHTML_AttributeTokenName_alternate = 0x0107
}NW_XHTML_1_0_AttributeToken_1_t;


typedef enum NW_XHTML_1_0_ElementToken_2_e{
	NW_XHTML_ElementToken_center = 0x0205,
	NW_XHTML_ElementToken_dir = 0x0206,
	NW_XHTML_ElementToken_font = 0x0207,
	NW_XHTML_ElementToken_menu = 0x0208,
	NW_XHTML_ElementToken_blink = 0x0209
}NW_XHTML_1_0_ElementToken_2_t;


typedef enum NW_XHTML_1_0_AttributeToken_2_e{
	NW_XHTML_AttributeToken_behavior = 0x0205,
	NW_XHTML_AttributeToken_direction = 0x0206,
	NW_XHTML_AttributeToken_loop = 0x0207,
	NW_XHTML_AttributeToken_scrollamount = 0x0208,
	NW_XHTML_AttributeToken_scrolldelay = 0x0209,
	NW_XHTML_AttributeToken_cti = 0x020b,
	NW_XHTML_AttributeToken_telbook = 0x020d,
	NW_XHTML_AttributeToken_email = 0x020f,
	NW_XHTML_AttributeToken_A = 0x0286,
	NW_XHTML_AttributeToken_a = 0x0287,
	NW_XHTML_AttributeToken_I = 0x0288,
	NW_XHTML_AttributeToken_i = 0x0289,
	NW_XHTML_AttributeToken_scroll = 0x028a,
	NW_XHTML_AttributeToken_slide = 0x028b,
	NW_XHTML_AttributeToken_alternate = 0x028c
}NW_XHTML_1_0_AttributeToken_2_t;


#endif
