#include <Windows.h>
#define SQUIRREL_API extern __declspec(dllexport)
#include <assert.h>
#include <squirrel/impl/sqstate.h>
#include <squirrel/impl/sqstring.h>
#include <squirrel/impl/sqvm.h>
#include <squirrel/squirrel.h>

#include <new>

// **************************** //
// ****** Native Offsets ****** //
// **************************** //
/*vm*/
constexpr auto OFF_sq_open            = 0x607140;
constexpr auto OFF_sq_newthread       = 0x606FF0;
constexpr auto OFF_sq_seterrorhandler = 0x607EF0;
constexpr auto OFF_sq_close           = 0x000000; // TODO find
constexpr auto OFF_sq_setforeignptr   = 0x000000; // TODO find
constexpr auto OFF_sq_getforeignptr   = 0x000000; // TODO find
// constexpr auto OFF_sq_setprintfunc = 0x000000; // self-implemented
constexpr auto OFF_sq_getprintfunc = 0x6068C0;
constexpr auto OFF_sq_suspendvm    = 0x000000; // TODO find
constexpr auto OFF_sq_wakeupvm     = 0x608510;
constexpr auto OFF_sq_getvmstate   = 0x606C00;

/*compiler*/
constexpr auto OFF_sq_compile         = 0x605F40;
constexpr auto OFF_sq_compilebuffer   = 0x606060;
constexpr auto OFF_sq_enabledebuginfo = 0x606190;
// constexpr auto OFF_sq_notifyallexceptions = 0x000000; // self-implemented
constexpr auto OFF_sq_setcompilererrorhandler = 0x607CE0;

/*stack operations*/
constexpr auto OFF_sq_push         = 0x607240;
constexpr auto OFF_sq_pop          = 0x616990;
constexpr auto OFF_sq_poptop       = 0x616930;
constexpr auto OFF_sq_remove       = 0x616B40;
constexpr auto OFF_sq_gettop       = 0x606A50;
constexpr auto OFF_sq_settop       = 0x608330;
constexpr auto OFF_sq_reservestack = 0x000000; // TODO find/implement
constexpr auto OFF_sq_cmp          = 0x000000; // TODO find/implement
constexpr auto OFF_sq_move         = 0x606CD0;

/*object creation handling*/
constexpr auto OFF_sq_newuserdata          = 0x6070C0;
constexpr auto OFF_sq_newtable             = 0x606F50;
constexpr auto OFF_sq_newarray             = 0x000000; // TODO find/implement
constexpr auto OFF_sq_newclosure           = 0x606D20;
constexpr auto OFF_sq_setparamscheck       = 0x6080D0;
constexpr auto OFF_sq_bindenv              = 0x605B20;
constexpr auto OFF_sq_pushstring           = 0x607410;
constexpr auto OFF_sq_pushfloat            = 0x6072E0;
constexpr auto OFF_sq_pushinteger          = 0x607330;
constexpr auto OFF_sq_pushbool             = 0x607280;
constexpr auto OFF_sq_pushuserpointer      = 0x000000; // TODO find/implement
constexpr auto OFF_sq_pushnull             = 0x607380;
constexpr auto OFF_sq_gettype              = 0x606A60;
constexpr auto OFF_sq_getsize              = 0x6068E0;
constexpr auto OFF_sq_getbase              = 0x000000; // TODO find/implement
constexpr auto OFF_sq_instanceof           = 0x000000; // TODO find/implement
constexpr auto OFF_sq_tostring             = 0x608480;
constexpr auto OFF_sq_tobool               = 0x608430;
constexpr auto OFF_sq_getstring            = 0x6069A0;
constexpr auto OFF_sq_getinteger           = 0x6066F0;
constexpr auto OFF_sq_getfloat             = 0x6065C0;
constexpr auto OFF_sq_getbool              = 0x6063C0;
constexpr auto OFF_sq_getthread            = 0x000000; // TODO find/implement
constexpr auto OFF_sq_getuserpointer       = 0x606B50;
constexpr auto OFF_sq_getuserdata          = 0x606A90;
constexpr auto OFF_sq_settypetag           = 0x000000; // TODO find/implement
constexpr auto OFF_sq_gettypetag           = 0x000000; // TODO find/implement
constexpr auto OFF_sq_setreleasehook       = 0x608220;
constexpr auto OFF_sq_getscratchpad        = 0x6068D0;
constexpr auto OFF_sq_getfunctioninfo      = 0x000000; // TODO find/implement
constexpr auto OFF_sq_getclosureinfo       = 0x000000; // TODO find/implement
constexpr auto OFF_sq_setnativeclosurename = 0x607FE0;
constexpr auto OFF_sq_setinstanceup        = 0x607F80;
constexpr auto OFF_sq_getinstanceup        = 0x606630;
constexpr auto OFF_sq_setclassudsize       = 0x000000; // TODO find/implement
constexpr auto OFF_sq_newclass             = 0x000000; // TODO find/implement
constexpr auto OFF_sq_createinstance       = 0x6060A0;
constexpr auto OFF_sq_setattributes        = 0x607B10;
constexpr auto OFF_sq_getattributes        = 0x606270;
constexpr auto OFF_sq_getclass             = 0x606410;
constexpr auto OFF_sq_weakref              = 0x608730;
constexpr auto OFF_sq_getdefaultdelegate   = 0x000000; // TODO find/implement

/*object manipulation*/
constexpr auto OFF_sq_pushroottable     = 0x607400;
constexpr auto OFF_sq_pushregistrytable = 0x6073F0;
constexpr auto OFF_sq_pushconsttable    = 0x000000; // TODO find/implement
constexpr auto OFF_sq_setroottable      = 0x6082A0;
constexpr auto OFF_sq_setconsttable     = 0x607CF0;
constexpr auto OFF_sq_newslot           = 0x606E60;
constexpr auto OFF_sq_deleteslot        = 0x000000; // TODO find/implement
constexpr auto OFF_sq_set               = 0x000000; // TODO find/implement
constexpr auto OFF_sq_get               = 0x6061C0;
constexpr auto OFF_sq_rawget            = 0x607620;
constexpr auto OFF_sq_rawset            = 0x620B80;
constexpr auto OFF_sq_rawdeleteslot     = 0x607490;
constexpr auto OFF_sq_arrayappend       = 0x605710;
constexpr auto OFF_sq_arraypop          = 0x605800;
constexpr auto OFF_sq_arrayresize       = 0x000000; // TODO find/implement
constexpr auto OFF_sq_arrayreverse      = 0x000000; // TODO find/implement
constexpr auto OFF_sq_arrayremove       = 0x000000; // TODO find/implement
constexpr auto OFF_sq_arrayinsert       = 0x000000; // TODO find/implement
constexpr auto OFF_sq_setdelegate       = 0x607E00;
constexpr auto OFF_sq_getdelegate       = 0x606500;
constexpr auto OFF_sq_clone             = 0x000000; // TODO find/implement
constexpr auto OFF_sq_setfreevariable   = 0x000000; // TODO find/implement
// constexpr auto OFF_sq_next = 0x000000; // self-implemented
constexpr auto OFF_sq_getweakrefval = 0x606C20;
constexpr auto OFF_sq_clear         = 0x605E20;

/*calls*/
constexpr auto OFF_sq_call            = 0x605D30;
constexpr auto OFF_sq_resume          = 0x000000; // TODO find/implement
constexpr auto OFF_sq_getlocal        = 0x606760;
constexpr auto OFF_sq_getfreevariable = 0x000000; // TODO find/implement
constexpr auto OFF_sq_throwerror      = 0x6083A0;
constexpr auto OFF_sq_reseterror      = 0x000000; // TODO find/implement
constexpr auto OFF_sq_getlasterror    = 0x606750;

/*raw object handling*/
constexpr auto OFF_sq_getstackobj = 0x606960;
constexpr auto OFF_sq_pushobject  = 0x607390;
constexpr auto OFF_sq_addref      = 0x6056F0;
constexpr auto OFF_sq_release     = 0x607A70;
constexpr auto OFF_sq_resetobject = 0x607B00;
// constexpr auto OFF_sq_objtostring = 0x000000; // self-implemented
// constexpr auto OFF_sq_objtobool = 0x000000; // self-implemented
// constexpr auto OFF_sq_objtointeger = 0x000000; // self-implemented
// constexpr auto OFF_sq_objtofloat = 0x000000; // self-implemented
constexpr auto OFF_sq_getobjtypetag = 0x000000; // TODO find/implement

/*GC*/
constexpr auto OFF_sq_collectgarbage = 0x605F30;

/*serialization*/
constexpr auto OFF_sq_writeclosure = 0x000000; // TODO find/implement
constexpr auto OFF_sq_readclosure  = 0x607990;

/*mem allocation*/
constexpr auto OFF_sq_malloc  = 0x608E70;
constexpr auto OFF_sq_realloc = 0x608E80;
constexpr auto OFF_sq_free    = 0x608E60;

/*debug*/
constexpr auto OFF_sq_stackinfos   = 0x608D60;
constexpr auto OFF_sq_setdebughook = 0x607D70;

// **************************** //
// ****** Function Types ****** //
// **************************** //
/*vm*/
using PTR_sq_open            = HSQUIRRELVM (*)(SQInteger initialstacksize);
using PTR_sq_newthread       = HSQUIRRELVM (*)(HSQUIRRELVM friendvm, SQInteger initialstacksize);
using PTR_sq_seterrorhandler = void (*)(HSQUIRRELVM v);
using PTR_sq_close           = void (*)(HSQUIRRELVM v);
using PTR_sq_setforeignptr   = void (*)(HSQUIRRELVM v, SQUserPointer p);
using PTR_sq_getforeignptr   = SQUserPointer (*)(HSQUIRRELVM v);
using PTR_sq_setprintfunc    = void (*)(HSQUIRRELVM v, SQPRINTFUNCTION printfunc);
using PTR_sq_getprintfunc    = SQPRINTFUNCTION (*)(HSQUIRRELVM v);
using PTR_sq_suspendvm       = SQRESULT (*)(HSQUIRRELVM v);
using PTR_sq_wakeupvm =
    SQRESULT (*)(HSQUIRRELVM v, SQBool resumedret, SQBool retval, SQBool raiseerror, SQBool throwerror);
using PTR_sq_getvmstate = SQInteger (*)(HSQUIRRELVM v);

/*compiler*/
using PTR_sq_compile =
    SQRESULT (*)(HSQUIRRELVM v, SQLEXREADFUNC read, SQUserPointer p, const SQChar* sourcename, SQBool raiseerror);
using PTR_sq_compilebuffer =
    SQRESULT (*)(HSQUIRRELVM v, const SQChar* s, SQInteger size, const SQChar* sourcename, SQBool raiseerror);
using PTR_sq_enabledebuginfo         = void (*)(HSQUIRRELVM v, SQBool enable);
using PTR_sq_notifyallexceptions     = void (*)(HSQUIRRELVM v, SQBool enable);
using PTR_sq_setcompilererrorhandler = void (*)(HSQUIRRELVM v, SQCOMPILERERROR f);

/*stack operations*/
using PTR_sq_push         = void (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_pop          = void (*)(HSQUIRRELVM v, SQInteger nelemstopop);
using PTR_sq_poptop       = void (*)(HSQUIRRELVM v);
using PTR_sq_remove       = void (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_gettop       = SQInteger (*)(HSQUIRRELVM v);
using PTR_sq_settop       = void (*)(HSQUIRRELVM v, SQInteger newtop);
using PTR_sq_reservestack = void (*)(HSQUIRRELVM v, SQInteger nsize);
using PTR_sq_cmp          = SQInteger (*)(HSQUIRRELVM v);
using PTR_sq_move         = void (*)(HSQUIRRELVM dest, HSQUIRRELVM src, SQInteger idx);

/*object creation handling*/
using PTR_sq_newuserdata     = SQUserPointer (*)(HSQUIRRELVM v, SQUnsignedInteger size);
using PTR_sq_newtable        = void (*)(HSQUIRRELVM v);
using PTR_sq_newarray        = void (*)(HSQUIRRELVM v, SQInteger size);
using PTR_sq_newclosure      = void (*)(HSQUIRRELVM v, SQFUNCTION func, SQUnsignedInteger nfreevars);
using PTR_sq_setparamscheck  = SQRESULT (*)(HSQUIRRELVM v, SQInteger nparamscheck, const SQChar* typemask);
using PTR_sq_bindenv         = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_pushstring      = void (*)(HSQUIRRELVM v, const SQChar* s, SQInteger len);
using PTR_sq_pushfloat       = void (*)(HSQUIRRELVM v, SQFloat f);
using PTR_sq_pushinteger     = void (*)(HSQUIRRELVM v, SQInteger n);
using PTR_sq_pushbool        = void (*)(HSQUIRRELVM v, SQBool b);
using PTR_sq_pushuserpointer = void (*)(HSQUIRRELVM v, SQUserPointer p);
using PTR_sq_pushnull        = void (*)(HSQUIRRELVM v);
using PTR_sq_gettype         = SQObjectType (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getsize         = SQInteger (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getbase         = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_instanceof      = SQBool (*)(HSQUIRRELVM v);
using PTR_sq_tostring        = void (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_tobool          = void (*)(HSQUIRRELVM v, SQInteger idx, SQBool* b);
using PTR_sq_getstring       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, const SQChar** c);
using PTR_sq_getinteger      = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQInteger* i);
using PTR_sq_getfloat        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQFloat* f);
using PTR_sq_getbool         = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQBool* b);
using PTR_sq_getthread       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, HSQUIRRELVM* thread);
using PTR_sq_getuserpointer  = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p);
using PTR_sq_getuserdata     = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p, SQUserPointer* typetag);
using PTR_sq_settypetag      = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer typetag);
using PTR_sq_gettypetag      = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer* typetag);
using PTR_sq_setreleasehook  = void (*)(HSQUIRRELVM v, SQInteger idx, SQRELEASEHOOK hook);
using PTR_sq_getscratchpad   = SQChar* (*)(HSQUIRRELVM v, SQInteger minsize);
using PTR_sq_getfunctioninfo = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQFunctionInfo* fi);
using PTR_sq_getclosureinfo  = SQRESULT (*)(HSQUIRRELVM v,
                                           SQInteger idx,
                                           SQUnsignedInteger* nparams,
                                           SQUnsignedInteger* nfreevars);
using PTR_sq_setnativeclosurename = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, const SQChar* name);
using PTR_sq_setinstanceup        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer p);
using PTR_sq_getinstanceup        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p, SQUserPointer typetag);
using PTR_sq_setclassudsize       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQInteger udsize);
using PTR_sq_newclass             = SQRESULT (*)(HSQUIRRELVM v, SQBool hasbase);
using PTR_sq_createinstance       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_setattributes        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getattributes        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getclass             = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_weakref              = void (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getdefaultdelegate   = SQRESULT (*)(HSQUIRRELVM v, SQObjectType t);

/*object manipulation*/
using PTR_sq_pushroottable     = void (*)(HSQUIRRELVM v);
using PTR_sq_pushregistrytable = void (*)(HSQUIRRELVM v);
using PTR_sq_pushconsttable    = void (*)(HSQUIRRELVM v);
using PTR_sq_setroottable      = SQRESULT (*)(HSQUIRRELVM v);
using PTR_sq_setconsttable     = SQRESULT (*)(HSQUIRRELVM v);
using PTR_sq_newslot           = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQBool bstatic);
using PTR_sq_deleteslot        = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
using PTR_sq_set               = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_get               = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_rawget            = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_rawset            = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_rawdeleteslot     = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
using PTR_sq_arrayappend       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_arraypop          = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQBool pushval);
using PTR_sq_arrayresize       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQInteger newsize);
using PTR_sq_arrayreverse      = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_arrayremove       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQInteger itemidx);
using PTR_sq_arrayinsert       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQInteger destpos);
using PTR_sq_setdelegate       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getdelegate       = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_clone             = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_setfreevariable   = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger nval);
using PTR_sq_next              = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_getweakrefval     = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);
using PTR_sq_clear             = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx);

/*calls*/
using PTR_sq_call            = SQRESULT (*)(HSQUIRRELVM v, SQInteger params, SQBool retval, SQBool raiseerror);
using PTR_sq_resume          = SQRESULT (*)(HSQUIRRELVM v, SQBool retval, SQBool raiseerror);
using PTR_sq_getlocal        = const SQChar* (*)(HSQUIRRELVM v, SQUnsignedInteger level, SQUnsignedInteger idx);
using PTR_sq_getfreevariable = const SQChar* (*)(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger nval);
using PTR_sq_throwerror      = SQRESULT (*)(HSQUIRRELVM v, const SQChar* err);
using PTR_sq_reseterror      = void (*)(HSQUIRRELVM v);
using PTR_sq_getlasterror    = void (*)(HSQUIRRELVM v);

/*raw object handling*/
using PTR_sq_getstackobj   = SQRESULT (*)(HSQUIRRELVM v, SQInteger idx, HSQOBJECT* po);
using PTR_sq_pushobject    = void (*)(HSQUIRRELVM v, HSQOBJECT obj);
using PTR_sq_addref        = void (*)(HSQUIRRELVM v, HSQOBJECT* po);
using PTR_sq_release       = SQBool (*)(HSQUIRRELVM v, HSQOBJECT* po);
using PTR_sq_resetobject   = void (*)(HSQOBJECT* po);
using PTR_sq_objtostring   = const SQChar* (*)(HSQOBJECT* o);
using PTR_sq_objtobool     = SQBool (*)(HSQOBJECT* o);
using PTR_sq_objtointeger  = SQInteger (*)(HSQOBJECT* o);
using PTR_sq_objtofloat    = SQFloat (*)(HSQOBJECT* o);
using PTR_sq_getobjtypetag = SQRESULT (*)(HSQOBJECT* o, SQUserPointer* typetag);

/*GC*/
using PTR_sq_collectgarbage = SQInteger (*)(HSQUIRRELVM v);

/*serialization*/
using PTR_sq_writeclosure = SQRESULT (*)(HSQUIRRELVM vm, SQWRITEFUNC writef, SQUserPointer up);
using PTR_sq_readclosure  = SQRESULT (*)(HSQUIRRELVM vm, SQREADFUNC readf, SQUserPointer up);

/*mem allocation*/
using PTR_sq_malloc  = void* (*)(SQUnsignedInteger size);
using PTR_sq_realloc = void* (*)(void* p, SQUnsignedInteger oldsize, SQUnsignedInteger newsize);
using PTR_sq_free    = void (*)(void* p, SQUnsignedInteger size);

/*debug*/
using PTR_sq_stackinfos   = SQRESULT (*)(HSQUIRRELVM v, SQInteger level, SQStackInfos* si);
using PTR_sq_setdebughook = void (*)(HSQUIRRELVM v);

// ************************************* //
// ****** Function Implementation ****** //
// ************************************* //
/*vm*/
HSQUIRRELVM sq_open(SQInteger initialstacksize)
{
    return ((PTR_sq_open)((char*)GetModuleHandle(NULL) + OFF_sq_open))(initialstacksize);
}
HSQUIRRELVM sq_newthread(HSQUIRRELVM friendvm, SQInteger initialstacksize)
{
    return ((PTR_sq_newthread)((char*)GetModuleHandle(NULL) + OFF_sq_newthread))(friendvm, initialstacksize);
}
void sq_seterrorhandler(HSQUIRRELVM v)
{
    return ((PTR_sq_seterrorhandler)((char*)GetModuleHandle(NULL) + OFF_sq_seterrorhandler))(v);
}
void sq_close(HSQUIRRELVM v) { return ((PTR_sq_close)((char*)GetModuleHandle(NULL) + OFF_sq_close))(v); }
void sq_setforeignptr(HSQUIRRELVM v, SQUserPointer p)
{
    return ((PTR_sq_setforeignptr)((char*)GetModuleHandle(NULL) + OFF_sq_setforeignptr))(v, p);
}
SQUserPointer sq_getforeignptr(HSQUIRRELVM v)
{
    return ((PTR_sq_getforeignptr)((char*)GetModuleHandle(NULL) + OFF_sq_getforeignptr))(v);
}
void sq_setprintfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc) { _ss(v)->_printfunc = printfunc; }
SQPRINTFUNCTION sq_getprintfunc(HSQUIRRELVM v)
{
    return ((PTR_sq_getprintfunc)((char*)GetModuleHandle(NULL) + OFF_sq_getprintfunc))(v);
}
SQRESULT sq_suspendvm(HSQUIRRELVM v)
{
    return ((PTR_sq_suspendvm)((char*)GetModuleHandle(NULL) + OFF_sq_suspendvm))(v);
}
SQRESULT sq_wakeupvm(HSQUIRRELVM v, SQBool resumedret, SQBool retval, SQBool raiseerror, SQBool throwerror)
{
    return ((PTR_sq_wakeupvm)((char*)GetModuleHandle(NULL) +
                              OFF_sq_wakeupvm))(v, resumedret, retval, raiseerror, throwerror);
}
SQInteger sq_getvmstate(HSQUIRRELVM v)
{
    return ((PTR_sq_getvmstate)((char*)GetModuleHandle(NULL) + OFF_sq_getvmstate))(v);
}

/*compiler*/
SQRESULT sq_compile(HSQUIRRELVM v, SQLEXREADFUNC read, SQUserPointer p, const SQChar* sourcename, SQBool raiseerror)
{
    return ((PTR_sq_compile)((char*)GetModuleHandle(NULL) + OFF_sq_compile))(v, read, p, sourcename, raiseerror);
}
SQRESULT sq_compilebuffer(HSQUIRRELVM v, const SQChar* s, SQInteger size, const SQChar* sourcename, SQBool raiseerror)
{
    return ((PTR_sq_compilebuffer)((char*)GetModuleHandle(NULL) +
                                   OFF_sq_compilebuffer))(v, s, size, sourcename, raiseerror);
}
void sq_enabledebuginfo(HSQUIRRELVM v, SQBool enable)
{
    return ((PTR_sq_enabledebuginfo)((char*)GetModuleHandle(NULL) + OFF_sq_enabledebuginfo))(v, enable);
}
void sq_notifyallexceptions(HSQUIRRELVM v, SQBool enable) { _ss(v)->_notifyallexceptions = enable ? true : false; }
void sq_setcompilererrorhandler(HSQUIRRELVM v, SQCOMPILERERROR f)
{
    return ((PTR_sq_setcompilererrorhandler)((char*)GetModuleHandle(NULL) + OFF_sq_setcompilererrorhandler))(v, f);
}

/*stack operations*/
void sq_push(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_push)((char*)GetModuleHandle(NULL) + OFF_sq_push))(v, idx);
}
void sq_pop(HSQUIRRELVM v, SQInteger nelemstopop)
{
    return ((PTR_sq_pop)((char*)GetModuleHandle(NULL) + OFF_sq_pop))(v, nelemstopop);
}
void sq_poptop(HSQUIRRELVM v) { return ((PTR_sq_poptop)((char*)GetModuleHandle(NULL) + OFF_sq_poptop))(v); }
void sq_remove(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_remove)((char*)GetModuleHandle(NULL) + OFF_sq_remove))(v, idx);
}
SQInteger sq_gettop(HSQUIRRELVM v) { return ((PTR_sq_gettop)((char*)GetModuleHandle(NULL) + OFF_sq_gettop))(v); }
void sq_settop(HSQUIRRELVM v, SQInteger newtop)
{
    return ((PTR_sq_settop)((char*)GetModuleHandle(NULL) + OFF_sq_settop))(v, newtop);
}
void sq_reservestack(HSQUIRRELVM v, SQInteger nsize)
{
    return ((PTR_sq_reservestack)((char*)GetModuleHandle(NULL) + OFF_sq_reservestack))(v, nsize);
}
SQInteger sq_cmp(HSQUIRRELVM v) { return ((PTR_sq_cmp)((char*)GetModuleHandle(NULL) + OFF_sq_cmp))(v); }
void sq_move(HSQUIRRELVM dest, HSQUIRRELVM src, SQInteger idx)
{
    return ((PTR_sq_move)((char*)GetModuleHandle(NULL) + OFF_sq_move))(dest, src, idx);
}

/*object creation handling*/
SQUserPointer sq_newuserdata(HSQUIRRELVM v, SQUnsignedInteger size)
{
    return ((PTR_sq_newuserdata)((char*)GetModuleHandle(NULL) + OFF_sq_newuserdata))(v, size);
}
void sq_newtable(HSQUIRRELVM v) { return ((PTR_sq_newtable)((char*)GetModuleHandle(NULL) + OFF_sq_newtable))(v); }
void sq_newarray(HSQUIRRELVM v, SQInteger size)
{
    return ((PTR_sq_newarray)((char*)GetModuleHandle(NULL) + OFF_sq_newarray))(v, size);
}
void sq_newclosure(HSQUIRRELVM v, SQFUNCTION func, SQUnsignedInteger nfreevars)
{
    return ((PTR_sq_newclosure)((char*)GetModuleHandle(NULL) + OFF_sq_newclosure))(v, func, nfreevars);
}
SQRESULT sq_setparamscheck(HSQUIRRELVM v, SQInteger nparamscheck, const SQChar* typemask)
{
    return ((PTR_sq_setparamscheck)((char*)GetModuleHandle(NULL) + OFF_sq_setparamscheck))(v, nparamscheck, typemask);
}
SQRESULT sq_bindenv(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_bindenv)((char*)GetModuleHandle(NULL) + OFF_sq_bindenv))(v, idx);
}
void sq_pushstring(HSQUIRRELVM v, const SQChar* s, SQInteger len)
{
    return ((PTR_sq_pushstring)((char*)GetModuleHandle(NULL) + OFF_sq_pushstring))(v, s, len);
}
void sq_pushfloat(HSQUIRRELVM v, SQFloat f)
{
    return ((PTR_sq_pushfloat)((char*)GetModuleHandle(NULL) + OFF_sq_pushfloat))(v, f);
}
void sq_pushinteger(HSQUIRRELVM v, SQInteger n)
{
    return ((PTR_sq_pushinteger)((char*)GetModuleHandle(NULL) + OFF_sq_pushinteger))(v, n);
}
void sq_pushbool(HSQUIRRELVM v, SQBool b)
{
    return ((PTR_sq_pushbool)((char*)GetModuleHandle(NULL) + OFF_sq_pushbool))(v, b);
}
void sq_pushuserpointer(HSQUIRRELVM v, SQUserPointer p)
{
    return ((PTR_sq_pushuserpointer)((char*)GetModuleHandle(NULL) + OFF_sq_pushuserpointer))(v, p);
}
void sq_pushnull(HSQUIRRELVM v) { return ((PTR_sq_pushnull)((char*)GetModuleHandle(NULL) + OFF_sq_pushnull))(v); }
SQObjectType sq_gettype(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_gettype)((char*)GetModuleHandle(NULL) + OFF_sq_gettype))(v, idx);
}
SQInteger sq_getsize(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getsize)((char*)GetModuleHandle(NULL) + OFF_sq_getsize))(v, idx);
}
SQRESULT sq_getbase(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getbase)((char*)GetModuleHandle(NULL) + OFF_sq_getbase))(v, idx);
}
SQBool sq_instanceof(HSQUIRRELVM v)
{
    return ((PTR_sq_instanceof)((char*)GetModuleHandle(NULL) + OFF_sq_instanceof))(v);
}
void sq_tostring(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_tostring)((char*)GetModuleHandle(NULL) + OFF_sq_tostring))(v, idx);
}
void sq_tobool(HSQUIRRELVM v, SQInteger idx, SQBool* b)
{
    return ((PTR_sq_tobool)((char*)GetModuleHandle(NULL) + OFF_sq_tobool))(v, idx, b);
}
SQRESULT sq_getstring(HSQUIRRELVM v, SQInteger idx, const SQChar** c)
{
    return ((PTR_sq_getstring)((char*)GetModuleHandle(NULL) + OFF_sq_getstring))(v, idx, c);
}
SQRESULT sq_getinteger(HSQUIRRELVM v, SQInteger idx, SQInteger* i)
{
    return ((PTR_sq_getinteger)((char*)GetModuleHandle(NULL) + OFF_sq_getinteger))(v, idx, i);
}
SQRESULT sq_getfloat(HSQUIRRELVM v, SQInteger idx, SQFloat* f)
{
    return ((PTR_sq_getfloat)((char*)GetModuleHandle(NULL) + OFF_sq_getfloat))(v, idx, f);
}
SQRESULT sq_getbool(HSQUIRRELVM v, SQInteger idx, SQBool* b)
{
    return ((PTR_sq_getbool)((char*)GetModuleHandle(NULL) + OFF_sq_getbool))(v, idx, b);
}
SQRESULT sq_getthread(HSQUIRRELVM v, SQInteger idx, HSQUIRRELVM* thread)
{
    return ((PTR_sq_getthread)((char*)GetModuleHandle(NULL) + OFF_sq_getthread))(v, idx, thread);
}
SQRESULT sq_getuserpointer(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p)
{
    return ((PTR_sq_getuserpointer)((char*)GetModuleHandle(NULL) + OFF_sq_getuserpointer))(v, idx, p);
}
SQRESULT sq_getuserdata(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p, SQUserPointer* typetag)
{
    return ((PTR_sq_getuserdata)((char*)GetModuleHandle(NULL) + OFF_sq_getuserdata))(v, idx, p, typetag);
}
SQRESULT sq_settypetag(HSQUIRRELVM v, SQInteger idx, SQUserPointer typetag)
{
    return ((PTR_sq_settypetag)((char*)GetModuleHandle(NULL) + OFF_sq_settypetag))(v, idx, typetag);
}
SQRESULT sq_gettypetag(HSQUIRRELVM v, SQInteger idx, SQUserPointer* typetag)
{
    return ((PTR_sq_gettypetag)((char*)GetModuleHandle(NULL) + OFF_sq_gettypetag))(v, idx, typetag);
}
void sq_setreleasehook(HSQUIRRELVM v, SQInteger idx, SQRELEASEHOOK hook)
{
    return ((PTR_sq_setreleasehook)((char*)GetModuleHandle(NULL) + OFF_sq_setreleasehook))(v, idx, hook);
}
SQChar* sq_getscratchpad(HSQUIRRELVM v, SQInteger minsize)
{
    return ((PTR_sq_getscratchpad)((char*)GetModuleHandle(NULL) + OFF_sq_getscratchpad))(v, minsize);
}
SQRESULT sq_getfunctioninfo(HSQUIRRELVM v, SQInteger idx, SQFunctionInfo* fi)
{
    return ((PTR_sq_getfunctioninfo)((char*)GetModuleHandle(NULL) + OFF_sq_getfunctioninfo))(v, idx, fi);
}
SQRESULT sq_getclosureinfo(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger* nparams, SQUnsignedInteger* nfreevars)
{
    return ((PTR_sq_getclosureinfo)((char*)GetModuleHandle(NULL) + OFF_sq_getclosureinfo))(v, idx, nparams, nfreevars);
}
SQRESULT sq_setnativeclosurename(HSQUIRRELVM v, SQInteger idx, const SQChar* name)
{
    return ((PTR_sq_setnativeclosurename)((char*)GetModuleHandle(NULL) + OFF_sq_setnativeclosurename))(v, idx, name);
}
SQRESULT sq_setinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer p)
{
    return ((PTR_sq_setinstanceup)((char*)GetModuleHandle(NULL) + OFF_sq_setinstanceup))(v, idx, p);
}
SQRESULT sq_getinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer* p, SQUserPointer typetag)
{
    return ((PTR_sq_getinstanceup)((char*)GetModuleHandle(NULL) + OFF_sq_getinstanceup))(v, idx, p, typetag);
}
SQRESULT sq_setclassudsize(HSQUIRRELVM v, SQInteger idx, SQInteger udsize)
{
    return ((PTR_sq_setclassudsize)((char*)GetModuleHandle(NULL) + OFF_sq_setclassudsize))(v, idx, udsize);
}
SQRESULT sq_newclass(HSQUIRRELVM v, SQBool hasbase)
{
    return ((PTR_sq_newclass)((char*)GetModuleHandle(NULL) + OFF_sq_newclass))(v, hasbase);
}
SQRESULT sq_createinstance(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_createinstance)((char*)GetModuleHandle(NULL) + OFF_sq_createinstance))(v, idx);
}
SQRESULT sq_setattributes(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_setattributes)((char*)GetModuleHandle(NULL) + OFF_sq_setattributes))(v, idx);
}
SQRESULT sq_getattributes(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getattributes)((char*)GetModuleHandle(NULL) + OFF_sq_getattributes))(v, idx);
}
SQRESULT sq_getclass(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getclass)((char*)GetModuleHandle(NULL) + OFF_sq_getclass))(v, idx);
}
void sq_weakref(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_weakref)((char*)GetModuleHandle(NULL) + OFF_sq_weakref))(v, idx);
}
SQRESULT sq_getdefaultdelegate(HSQUIRRELVM v, SQObjectType t)
{
    return ((PTR_sq_getdefaultdelegate)((char*)GetModuleHandle(NULL) + OFF_sq_getdefaultdelegate))(v, t);
}

/*object manipulation*/
void sq_pushroottable(HSQUIRRELVM v)
{
    return ((PTR_sq_pushroottable)((char*)GetModuleHandle(NULL) + OFF_sq_pushroottable))(v);
}
void sq_pushregistrytable(HSQUIRRELVM v)
{
    return ((PTR_sq_pushregistrytable)((char*)GetModuleHandle(NULL) + OFF_sq_pushregistrytable))(v);
}
void sq_pushconsttable(HSQUIRRELVM v)
{
    return ((PTR_sq_pushconsttable)((char*)GetModuleHandle(NULL) + OFF_sq_pushconsttable))(v);
}
SQRESULT sq_setroottable(HSQUIRRELVM v)
{
    return ((PTR_sq_setroottable)((char*)GetModuleHandle(NULL) + OFF_sq_setroottable))(v);
}
SQRESULT sq_setconsttable(HSQUIRRELVM v)
{
    return ((PTR_sq_setconsttable)((char*)GetModuleHandle(NULL) + OFF_sq_setconsttable))(v);
}
SQRESULT sq_newslot(HSQUIRRELVM v, SQInteger idx, SQBool bstatic)
{
    return ((PTR_sq_newslot)((char*)GetModuleHandle(NULL) + OFF_sq_newslot))(v, idx, bstatic);
}
SQRESULT sq_deleteslot(HSQUIRRELVM v, SQInteger idx, SQBool pushval)
{
    return ((PTR_sq_deleteslot)((char*)GetModuleHandle(NULL) + OFF_sq_deleteslot))(v, idx, pushval);
}
SQRESULT sq_set(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_set)((char*)GetModuleHandle(NULL) + OFF_sq_set))(v, idx);
}
SQRESULT sq_get(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_get)((char*)GetModuleHandle(NULL) + OFF_sq_get))(v, idx);
}
SQRESULT sq_rawget(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_rawget)((char*)GetModuleHandle(NULL) + OFF_sq_rawget))(v, idx);
}
SQRESULT sq_rawset(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_rawset)((char*)GetModuleHandle(NULL) + OFF_sq_rawset))(v, idx);
}
SQRESULT sq_rawdeleteslot(HSQUIRRELVM v, SQInteger idx, SQBool pushval)
{
    return ((PTR_sq_rawdeleteslot)((char*)GetModuleHandle(NULL) + OFF_sq_rawdeleteslot))(v, idx, pushval);
}
SQRESULT sq_arrayappend(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_arrayappend)((char*)GetModuleHandle(NULL) + OFF_sq_arrayappend))(v, idx);
}
SQRESULT sq_arraypop(HSQUIRRELVM v, SQInteger idx, SQBool pushval)
{
    return ((PTR_sq_arraypop)((char*)GetModuleHandle(NULL) + OFF_sq_arraypop))(v, idx, pushval);
}
SQRESULT sq_arrayresize(HSQUIRRELVM v, SQInteger idx, SQInteger newsize)
{
    return ((PTR_sq_arrayresize)((char*)GetModuleHandle(NULL) + OFF_sq_arrayresize))(v, idx, newsize);
}
SQRESULT sq_arrayreverse(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_arrayreverse)((char*)GetModuleHandle(NULL) + OFF_sq_arrayreverse))(v, idx);
}
SQRESULT sq_arrayremove(HSQUIRRELVM v, SQInteger idx, SQInteger itemidx)
{
    return ((PTR_sq_arrayremove)((char*)GetModuleHandle(NULL) + OFF_sq_arrayremove))(v, idx, itemidx);
}
SQRESULT sq_arrayinsert(HSQUIRRELVM v, SQInteger idx, SQInteger destpos)
{
    return ((PTR_sq_arrayinsert)((char*)GetModuleHandle(NULL) + OFF_sq_arrayinsert))(v, idx, destpos);
}
SQRESULT sq_setdelegate(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_setdelegate)((char*)GetModuleHandle(NULL) + OFF_sq_setdelegate))(v, idx);
}
SQRESULT sq_getdelegate(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getdelegate)((char*)GetModuleHandle(NULL) + OFF_sq_getdelegate))(v, idx);
}
SQRESULT sq_clone(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_clone)((char*)GetModuleHandle(NULL) + OFF_sq_clone))(v, idx);
}
SQRESULT sq_setfreevariable(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger nval)
{
    return ((PTR_sq_setfreevariable)((char*)GetModuleHandle(NULL) + OFF_sq_setfreevariable))(v, idx, nval);
}
SQRESULT sq_next(HSQUIRRELVM v, SQInteger idx)
{
    SQObjectPtr o = stack_get(v, idx), &refpos = stack_get(v, -1), realkey, val;
    if (type(o) == OT_GENERATOR) { return sq_throwerror(v, _SC("cannot iterate a generator")); }
    int faketojump;
    if (!v->FOREACH_OP(o, realkey, val, refpos, 0, 666, faketojump)) return SQ_ERROR;
    if (faketojump != 666)
    {
        v->Push(realkey);
        v->Push(val);
        return SQ_OK;
    }
    return SQ_ERROR;
}
SQRESULT sq_getweakrefval(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_getweakrefval)((char*)GetModuleHandle(NULL) + OFF_sq_getweakrefval))(v, idx);
}
SQRESULT sq_clear(HSQUIRRELVM v, SQInteger idx)
{
    return ((PTR_sq_clear)((char*)GetModuleHandle(NULL) + OFF_sq_clear))(v, idx);
}

/*calls*/
SQRESULT sq_call(HSQUIRRELVM v, SQInteger params, SQBool retval, SQBool raiseerror)
{
    return ((PTR_sq_call)((char*)GetModuleHandle(NULL) + OFF_sq_call))(v, params, retval, raiseerror);
}
SQRESULT sq_resume(HSQUIRRELVM v, SQBool retval, SQBool raiseerror)
{
    return ((PTR_sq_resume)((char*)GetModuleHandle(NULL) + OFF_sq_resume))(v, retval, raiseerror);
}
const SQChar* sq_getlocal(HSQUIRRELVM v, SQUnsignedInteger level, SQUnsignedInteger idx)
{
    return ((PTR_sq_getlocal)((char*)GetModuleHandle(NULL) + OFF_sq_getlocal))(v, level, idx);
}
const SQChar* sq_getfreevariable(HSQUIRRELVM v, SQInteger idx, SQUnsignedInteger nval)
{
    return ((PTR_sq_getfreevariable)((char*)GetModuleHandle(NULL) + OFF_sq_getfreevariable))(v, idx, nval);
}
SQRESULT sq_throwerror(HSQUIRRELVM v, const SQChar* err)
{
    return ((PTR_sq_throwerror)((char*)GetModuleHandle(NULL) + OFF_sq_throwerror))(v, err);
}
void sq_reseterror(HSQUIRRELVM v) { return ((PTR_sq_reseterror)((char*)GetModuleHandle(NULL) + OFF_sq_reseterror))(v); }
void sq_getlasterror(HSQUIRRELVM v)
{
    return ((PTR_sq_getlasterror)((char*)GetModuleHandle(NULL) + OFF_sq_getlasterror))(v);
}

/*raw object handling*/
SQRESULT sq_getstackobj(HSQUIRRELVM v, SQInteger idx, HSQOBJECT* po)
{
    return ((PTR_sq_getstackobj)((char*)GetModuleHandle(NULL) + OFF_sq_getstackobj))(v, idx, po);
}
void sq_pushobject(HSQUIRRELVM v, HSQOBJECT obj)
{
    return ((PTR_sq_pushobject)((char*)GetModuleHandle(NULL) + OFF_sq_pushobject))(v, obj);
}
void sq_addref(HSQUIRRELVM v, HSQOBJECT* po)
{
    return ((PTR_sq_addref)((char*)GetModuleHandle(NULL) + OFF_sq_addref))(v, po);
}
SQBool sq_release(HSQUIRRELVM v, HSQOBJECT* po)
{
    return ((PTR_sq_release)((char*)GetModuleHandle(NULL) + OFF_sq_release))(v, po);
}
void sq_resetobject(HSQOBJECT* po)
{
    return ((PTR_sq_resetobject)((char*)GetModuleHandle(NULL) + OFF_sq_resetobject))(po);
}
const SQChar* sq_objtostring(HSQOBJECT* o)
{
    if (sq_type(*o) == OT_STRING) { return _stringval(*o); }
    return NULL;
}
SQBool sq_objtobool(HSQOBJECT* o)
{
    if (sq_isbool(*o)) { return _integer(*o); }
    return SQFalse;
}
SQInteger sq_objtointeger(HSQOBJECT* o)
{
    if (sq_isnumeric(*o)) { return tointeger(*o); }
    return 0;
}
SQFloat sq_objtofloat(HSQOBJECT* o)
{
    if (sq_isnumeric(*o)) { return tofloat(*o); }
    return 0;
}
SQRESULT sq_getobjtypetag(HSQOBJECT* o, SQUserPointer* typetag)
{
    return ((PTR_sq_getobjtypetag)((char*)GetModuleHandle(NULL) + OFF_sq_getobjtypetag))(o, typetag);
}

/*GC*/
SQInteger sq_collectgarbage(HSQUIRRELVM v)
{
    return ((PTR_sq_collectgarbage)((char*)GetModuleHandle(NULL) + OFF_sq_collectgarbage))(v);
}

/*serialization*/
SQRESULT sq_writeclosure(HSQUIRRELVM vm, SQWRITEFUNC writef, SQUserPointer up)
{
    return ((PTR_sq_writeclosure)((char*)GetModuleHandle(NULL) + OFF_sq_writeclosure))(vm, writef, up);
}
SQRESULT sq_readclosure(HSQUIRRELVM vm, SQREADFUNC readf, SQUserPointer up)
{
    return ((PTR_sq_readclosure)((char*)GetModuleHandle(NULL) + OFF_sq_readclosure))(vm, readf, up);
}

/*mem allocation*/
void* sq_malloc(SQUnsignedInteger size)
{
    return ((PTR_sq_malloc)((char*)GetModuleHandle(NULL) + OFF_sq_malloc))(size);
}
void* sq_realloc(void* p, SQUnsignedInteger oldsize, SQUnsignedInteger newsize)
{
    return ((PTR_sq_realloc)((char*)GetModuleHandle(NULL) + OFF_sq_realloc))(p, oldsize, newsize);
}
void sq_free(void* p, SQUnsignedInteger size)
{
    return ((PTR_sq_free)((char*)GetModuleHandle(NULL) + OFF_sq_free))(p, size);
}

/*debug*/
SQRESULT sq_stackinfos(HSQUIRRELVM v, SQInteger level, SQStackInfos* si)
{
    return ((PTR_sq_stackinfos)((char*)GetModuleHandle(NULL) + OFF_sq_stackinfos))(v, level, si);
}
void sq_setdebughook(HSQUIRRELVM v)
{
    return ((PTR_sq_setdebughook)((char*)GetModuleHandle(NULL) + OFF_sq_setdebughook))(v);
}

// ********************************************* //
// ****** Custom Implementation Helper ********* //
// ********************************************* //
constexpr auto OFF_SQVM_FOREACH_OP = 0x614E80;

using PTR_SQVM_FOREACH_OP = bool (*)(SQVM* v,
                                     SQObjectPtr& o1,
                                     SQObjectPtr& o2,
                                     SQObjectPtr& o3,
                                     SQObjectPtr& o4,
                                     SQInteger arg_2,
                                     int exitpos,
                                     int& jump);

void SQVM::Push(const SQObjectPtr& o) { _stack[_top++] = o; }
SQObjectPtr& SQVM::GetUp(SQInteger n) { return _stack[_top + n]; }
SQObjectPtr& SQVM::GetAt(SQInteger n) { return _stack[n]; }

bool SQVM::FOREACH_OP(SQObjectPtr& o1,
                      SQObjectPtr& o2,
                      SQObjectPtr& o3,
                      SQObjectPtr& o4,
                      SQInteger arg_2,
                      int exitpos,
                      int& jump)
{
    return ((PTR_SQVM_FOREACH_OP)((char*)GetModuleHandle(NULL) +
                                  OFF_SQVM_FOREACH_OP))(this, o1, o2, o3, o4, arg_2, exitpos, jump);
}