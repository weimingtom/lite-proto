#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_io.h"

#define  EXT_SLI_LENS   256

#define  sl_Wbyte(out, number)		do{		\
										if(sl_emp(out)<sizeof(byte))	\
											return LP_FAIL;		\
										(out)->sp[(out)->sp_len++] = (number);	\
										return LP_TRUE;	\
									}while(0)


int sl_Rstr(slice* sl, char** out)
{
	check_sl(sl);
	*out = (char*)(sl->sp);
	sl->sp += (strlen(*out)+1);
	check_sl(sl);
	
	return LP_TRUE;
}	

int sl_Rlens(slice* sl, unsigned int* out)
{
	check_sl(sl);
	*out = *((unsigned int*)(sl->sp));
	sl->sp += sizeof(unsigned int);
	sl->sp += (*out);
	check_sl(sl);
	sl->sp -= (*out);
	
	return LP_TRUE;
}

int sl_Rbyte(slice* sl, byte* out)
{
	check_sl(sl);
	*out = *(sl->sp);
	sl->sp += 1;
	check_sl(sl);
	
	return LP_TRUE;
}

int  sl_Ruint(slice* sl, size_t* out)
{
	check_sl(sl);
	*out = *((size_t*)(sl->sp));
	sl->sp += sizeof(size_t);
	check_sl(sl);
	
	return LP_TRUE;
}


int sl_Wtag(slice* out, e_ot ot, unsigned int id)
{
//	out->
}


int sl_Wnumber(slice* out, byte* num, size_t lens)
{
	size_t i=0, len=0; 
	for(i=lens-1; i>=0; i--)
	{
		if(num[i])
		{
			len = i+1;
			break;
		}
	}

	for(i=0; i<lens; i++)
	{
		sl_Wbyte(out, (num[i])|0X80 );
		sl_Wbyte(out, (((num[i])&0X80)<<7) );
	}

}


int _sl_Wint32(slice* out, unsigned int num)
{
	byte* p = (byte*)(&num);
	
	if (num >= 0x80)
    {
		sl_Wbyte((byte)(num | 0x80));
		num >>= 7;
		if (num >= 0x80)
        {
			sl_Wbyte((byte)(num | 0x80));
			num >>= 7;
			if (num >= 0x80)
            {
				sl_Wbyte((byte)(num | 0x80));
				num >>= 7;
				if (num >= 0x80)
                {
					sl_Wbyte((byte)(num | 0x80));
					num >>= 7;
                }
            }
        }
    }
	/* assert: value<128 */
	sl_Wbyte((byte)num);
	return LP_TRUE;
}

int sl_Wstring()
{

}

int sl_Wmessage()
{

}


int  llp_out_open(slice* out)
{
	check_null(out, LP_FAIL);
	out->sp = (byte*)malloc(EXT_SLI_LENS*sizeof(byte));
	out->b_sp = out->sp;
	out->sp_len = EXT_SLI_LENS;

	return LP_TRUE;
}


int llp_out_close(slice* out)
{
	check_null(out, LP_FAIL);
	free(out->b_sp);
	memset(out, 0, sizeof(*out));

	return LP_TRUE;
}