/* --------------------------------------------------------------------------

                    Alexandria Genesis Library
					--------------------------

Title:			Function Prototype Definitions

File:			FUNCDEF.H

Description:	

		All typedefs used for defining pointers to functions
		should reside here. When defining a new function, check to make
		sure that the typedef you require does not already exist. If it
		does not, create a typedef and store it here.

Note:	It is always assumed that the first parameter is always
		Object *, and should not be specified in the typedef
		declaration.


-------------------------------------------------------------------------- */

#ifndef _FUNCDEF_H_
#define	_FUNCDEF_H_

#include <object.h>
#include <alexdef.h>




typedef	void	(*Func_v)(Object *);

typedef	void	(*Func_v_uw)(Object *, UWORD);

typedef void	(*Func_v_w)(Object *, WORD);

typedef void	(*Func_v_pCO_w)(Object *, Object *, WORD);

typedef void	(*Func_v_w_w)(Object*, WORD, WORD);


typedef ULONG	(*Func_ul)(Object *);


typedef UWORD	(*Func_uw)(Object *);

typedef UWORD	(*Func_uw_pv)(Object *, void *);

typedef	UWORD	(*Func_uw_uw)(Object *, UWORD);


typedef BOOL	(*Func_f)(Object *);

typedef BOOL	(*Func_f_w)(Object *p, WORD);

typedef BOOL	(*Func_f_pw)(Object *p, WORD *);

typedef BOOL	(*Func_f_pCO)(Object *p, Object *);



#endif



/* EOF -- FUNCDEF.H */



