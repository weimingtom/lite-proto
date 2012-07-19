#ifndef  _LP_TYPE_H_
#define	 _LP_TYPE_H_

#define LP_VERSION  "lite-proto interpreter version:2.0"

typedef enum _e_t{
	t_error,		// null
	t_num,			// num
	t_lb,			// {
	t_rb,			// }
	t_ass,			// =
	t_ll,			// [
	t_rl,			// ]
	t_ide,			// IDE
	t_end,			// ;
	t_clo,			// .
	t_ca,			// ,
	
	t_Kmessage,		// message
	t_Kinteger,		// integer
	t_kstring,		// string
	t_Kreal,		// real
	t_Kbytes,		// bytes
	
	t_Kextern,		// extern
	
	t_count
}e_t;


#endif
