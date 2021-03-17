#ifndef __jr_mail_h___
#define __jr_mail_h___

#include "ezport.h"

#include "jr/alist.h"

/******** Mail Library ********/


extern jr_int		jr_MailStringMessage		PROTO ((
						char *					smtp_server_name,
						char *					sender_full_address,
						char *					recipient_address,
						char **					opt_header_vector,
						char *					message_string,
						char *					error_buf
					));


extern jr_int		jr_MailStringMessageWithSubjectOnly	PROTO ((
						char *					smtp_server_name,
						char *					sender_address,
						char *					recipient_address,
						char *					opt_subject_buf,
						char *					message_string,
						char *					error_buf
					));

extern jr_int		jr_MailServerValidate		PROTO ((
						const char *			smtp_server_name,
						const char *			sender_address,
						char *					error_buf
					));

extern jr_int		jr_MailAddressValidate		PROTO ((
						const char *			recipient_address,
						const char *			smtp_server_name,
						const char *			sender_address,
						char *					error_buf
					));

extern jr_int		jr_MailServerValidateWithTimeOut	PROTO ((
						const char *			smtp_server_name,
						const char *			opt_sender_address,
						jr_int					time_out_seconds,
						char *					error_buf
					));
				
extern jr_int		jr_MailAddressValidateWithTimeOut	PROTO ((
						const char *			recipient_address,
						const char *			smtp_server_name,
						const char *			sender_address,
						jr_int					time_out_seconds,
						char *					error_buf
					));


/******* MX Record Lookup ********/

/*
 * Need to link with -lresolv
 */


extern jr_int		jr_MailGetMailExchanger			PROTO ((
						const char *				host_name,
						char *						smtp_server_name,
						jr_int						smtp_server_name_length,
						char *						error_buf
					));


typedef struct {
	const char *			mx_name;
	jr_int					mx_preference;
	jr_int					inet_addr_number;
} jr_MX_EntryType;


typedef struct {
	jr_AList				list_field[1];
} jr_MX_EntryListType;



extern void			jr_MX_EntryListInit				PROTO ((
						jr_MX_EntryListType *		mx_entry_list
					));

extern void			jr_MX_EntryListUndo				PROTO ((
						jr_MX_EntryListType *		mx_entry_list
					));

extern void			jr_MX_EntryListAddEntry			PROTO ((
						jr_MX_EntryListType *		mx_entry_list,
						const char *				mx_name,
						jr_int						mx_preference
					));

extern void			jr_MX_EntryListSetInetAddress	PROTO ((
						jr_MX_EntryListType *		mx_entry_list,
						const char *				mx_name,
						jr_int						inet_addr_number
					));

extern jr_int		jr_MX_EntryListLoadHostNameInfo	PROTO ((
						jr_MX_EntryListType *		mx_entry_list,
						const char *				host_name,
						char *						error_buf
					));

#define				jr_MX_EntryListIsEmpty(mx_entry_list)							\
					jr_AListIsEmpty ((mx_entry_list)->list_field)

#define				jr_MX_EntryListSize(mx_entry_list)								\
					jr_AListSize ((mx_entry_list)->list_field)

#define				jr_MX_EntryListSortByPreference(mx_entry_list)					\
					jr_AListQSort ((mx_entry_list)->list_field, jr_MX_EntryPreferenceCmp)

#define				jr_MX_EntryListEntryPtr(mx_entry_list, k)						\
					((jr_MX_EntryType *) jr_AListElementPtr ((mx_entry_list)->list_field, k))

#define				jr_MX_EntryListFirstEntryPtr(mx_entry_list)						\
					((jr_MX_EntryType *) jr_AListHeadPtr ((mx_entry_list)->list_field))

#define				jr_MX_EntryListForEachEntryPtr(mx_entry_list, mx_entry_ptr)		\
					jr_AListForEachElementPtr ((mx_entry_list)->list_field, mx_entry_ptr)

#define				jr_MX_EntryListForEachEntryIndex(mx_entry_list, k)		\
					jr_AListForEachElementIndex ((mx_entry_list)->list_field, k)


extern void			jr_MX_EntryInit					PROTO (( 
						jr_MX_EntryType *			mx_entry_ptr,
						const char *				mx_name,
						jr_int						mx_preference
					));

extern void			jr_MX_EntryUndo					PROTO ((
						jr_MX_EntryType *			mx_entry_ptr
					));

extern jr_int		jr_MX_EntryPreferenceCmp		PROTO ((
						const void *				void_ptr_1,
						const void *				void_ptr_2
					));


#define				jr_MX_EntryName(mx_entry_ptr)			((mx_entry_ptr)->mx_name)
#define				jr_MX_EntryPreference(mx_entry_ptr)		((mx_entry_ptr)->mx_preference)
#define				jr_MX_EntryInetAddrNumber(mx_entry_ptr)	((mx_entry_ptr)->inet_addr_number)

#define				jr_MX_EntrySetInetAddrNumber(mx_entry_ptr, v)	\
					((mx_entry_ptr)->inet_addr_number = (v))

#endif
