#ifndef		_VARIABLES_H_
#define		_VARIABLES_H_

#undef _VARIABLES_H_DECLS
//#define _VARIABLES_H_DECLS
#ifndef _VARIABLES_H_DECLS
#define _DECL extern
#define _INIT(x)
#else
#define _DECL  
#define _INIT(x)  = x
#endif


_DECL char filename[128] _INIT("./data/a09100");
_DECL int old_phase6_signal_status _INIT(1);
_DECL int ramp_flag _INIT(0);
_DECL int signal_flag _INIT(0);

#endif
