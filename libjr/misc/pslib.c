#define _POSIX_SOURCE 1

#include "ezport.h"
#include <string.h>
#include "jr/string.h"
#include "jr/misc.h"
#include "jr/trace.h"


jr_int jr_PostScriptLineGetPageNumber(lbuf)
	char *	lbuf ;
{
	jr_int	nitems;
	jr_int	page_number;

	nitems = sscanf (lbuf, "%%%% Page: ? %d", &page_number);

	if (nitems != 1) {
		return -1;
	}

	return page_number;
}


char *jr_PostScriptLineGetShowText(lbuf, text)
	char *lbuf ;
	char *text ;
{
	char *ptr ;
	char *str ;

	*text = 0 ;
	if (ptr = strchr (lbuf, '(')) /* ) */ {
		jr_int open_paren ;

		ptr++ ;
		str = ptr ;

		for (open_paren=1 ;
			(open_paren) && (*ptr); ptr++) {
			switch (*ptr) {
				case '(' : {
					open_paren++ ;
					break ;
				}
				case ')' : {
					open_paren-- ;
					if (open_paren == 0) {
						strncpy(text, str, ptr - str) ;
						text[ptr - str] = 0 ;
						return(text) ;
					}
					break ;
				}
			}
		}
	}
	return(0) ;
}

jr_int jr_PostScriptFileGetBoundingBox (rfp, box_ptr)
	FILE *						rfp;
	jr_PS_BoundingBoxStruct *	box_ptr;
{
	char						lbuf[256] ;
	jr_int						nitems;

	while (fgets(lbuf,sizeof(lbuf),rfp)) {
		if (strncmp(lbuf,"%%BoundingBox",13) == 0) {
			char *tp ;

			for (tp=lbuf; *tp ; tp++) {
				if (isdigit(*tp) || (*tp == '-')) break ;
			}
			nitems = sscanf(tp,"%f %f %f %f",
				&box_ptr->lower_left_x,		&box_ptr->lower_left_y,
				&box_ptr->upper_right_x,	&box_ptr->upper_right_y
			) ;

			if (nitems != 4) {
				continue;
			}
			
			box_ptr->range_x = box_ptr->upper_right_x - box_ptr->lower_left_x ;
			box_ptr->range_y = box_ptr->upper_right_y - box_ptr->lower_left_y ;

			if (jr_do_trace (jr_graphics)) {
				fprintf(stderr,"\t%%%%Bounding Box: %f %f %f %f",
					(box_ptr->lower_left_x),
					(box_ptr->lower_left_y),
					(box_ptr->upper_right_x),
					(box_ptr->upper_right_y)) ;
			}
			rewind(rfp) ;
			return 0;
		}
	}
	rewind(rfp) ;
	return -1;
}

void jr_PS_TransformInit			(
											trans_info,
											trans_x, trans_y, scale_x, scale_y,
											rotation,
											justify, vert_space, page_width, margin_width
									)
	jr_PS_TransformStruct *			trans_info;
	float							trans_x;
	float							trans_y;
	float							scale_x;
	float							scale_y;
	float							rotation;

	jr_int							justify;
	float							vert_space;
	float							page_width;
	float							margin_width;
{
	memset (trans_info, 0, sizeof (jr_PS_TransformStruct));

	trans_info->trans_x					= trans_x;
	trans_info->trans_y					= trans_y;
	trans_info->scale_x					= scale_x;
	trans_info->scale_y					= scale_y;
	trans_info->rotation				= rotation;

	trans_info->justify					= justify;
	trans_info->vert_space				= vert_space;
	trans_info->page_width				= page_width;
	trans_info->margin_width			= margin_width;
}

void jr_PostScriptPrintTransform (trans_info, box_ptr, wfp, roff_ps_prefix)
	jr_PS_TransformStruct *			trans_info;
	jr_PS_BoundingBoxStruct *		box_ptr;
	FILE *							wfp;
	char *							roff_ps_prefix;
{

	/* SCALE */
	fprintf(wfp,"%s/JR_vi   { %3.2f }  def\n",			roff_ps_prefix, trans_info->vert_space) ;
	fprintf(wfp,"%s/JR_llx  { %3.2f }  def\n",			roff_ps_prefix, box_ptr->lower_left_x) ;
	fprintf(wfp,"%s/JR_lly  { %3.2f }  def\n",			roff_ps_prefix, box_ptr->lower_left_y) ;
	fprintf(wfp,"%s/JR_rx   { %3.2f }  def\n",			roff_ps_prefix, box_ptr->range_x) ;
	fprintf(wfp,"%s/JR_ry   { %3.2f }  def\n",			roff_ps_prefix, box_ptr->range_y) ;

	fprintf(wfp,"%s/JR_CurrY   { currentpoint exch pop } def\n",				roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_CurrX   { currentpoint pop } def\n",						roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_Factor  { JR_vi resolution mul  JR_ry  div  } def\n",	roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_Inch    { resolution JR_Factor div }  def\n",			roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_ToOrig  { JR_llx neg  JR_lly neg  translate  } def\n",	roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_ToCurrY { JR_ToOrig  0.0 JR_CurrY translate } def\n",	roff_ps_prefix) ;
	fprintf(wfp,"%s/JR_ToCurrP { JR_ToOrig  JR_CurrX JR_CurrY translate } def\n",	roff_ps_prefix) ;

	fprintf(wfp,"%s/JR_Left    { JR_ToCurrY JR_Inch %3.2f mul 0.0 translate } def\n",
		roff_ps_prefix, trans_info->margin_width
	);

	fprintf(wfp,"%s/JR_Right   { JR_ToCurrY JR_Inch %3.2f mul JR_rx  sub 0.0 translate } def\n",
		roff_ps_prefix, trans_info->page_width - trans_info->margin_width
	);

	fprintf(wfp,
		"%s/JR_Center  { JR_ToCurrY JR_Inch %3.2f mul JR_rx  sub 2 div 0.0 translate } def\n",
		roff_ps_prefix, trans_info->page_width
	);

	fprintf(wfp,"%s/JR_ToScale { JR_Factor dup neg scale } def\n",				roff_ps_prefix) ;
	fprintf(wfp,"%sJR_ToScale\n",												roff_ps_prefix) ;
	
	switch (trans_info->justify) {
		case 'P': {
			fprintf(wfp,"%sJR_ToCurrP\n",										roff_ps_prefix) ;
			break ;
		}
		case 'R': {
			fprintf(wfp,"%sJR_Right\n",										roff_ps_prefix) ;
			break ;
		}
		case 'L': {
			fprintf(wfp,"%sJR_Left\n",											roff_ps_prefix) ;
			break ;
		}
		case 'C':
		default : {
			fprintf(wfp,"%sJR_Center\n",										roff_ps_prefix) ;
			break ;
		}
	}

	if ((trans_info->trans_x + trans_info->trans_y) != 0.0) {
		fprintf(wfp,"%s%f JR_Inch mul %f JR_Inch mul translate\n",
			roff_ps_prefix, trans_info->trans_x, trans_info->trans_y
		) ;
	}
	
	if ((trans_info->scale_x != 1.0) || (trans_info->scale_y != 1.0)) {
		fprintf(wfp,"%s%f %f scale\n",
			roff_ps_prefix, trans_info->scale_x, trans_info->scale_y
		) ;
	}

	if (trans_info->rotation != 0.0) {
		fprintf(wfp,"%s%f rotate\n",
			roff_ps_prefix, trans_info->rotation
		) ;
		fprintf(wfp,"%s0 %f sin %f mul neg translate\n",
			roff_ps_prefix, trans_info->rotation, -box_ptr->range_y
		) ;
	}
	
	fprintf(wfp,"%scurrentpoint moveto\n",										roff_ps_prefix) ;
}

