 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Autoconfig device support
  *
  * (c) 1996 Ed Hanway
  */

#define RTAREA_DEFAULT 0xf00000
#define RTAREA_BACKUP  0xef0000
#define RTAREA_BASE 0xf00000

extern uae_u32 addr (int);
extern void db (uae_u8);
extern void dw (uae_u16);
extern void dl (uae_u32);
extern uae_u32 ds (const char *);
extern void calltrap (uae_u32);
extern void org (uae_u32);
extern uae_u32 here (void);
extern uaecptr makedatatable (uaecptr resid, uaecptr resname, uae_u8 type, uae_s8 priority, uae_u16 ver, uae_u16 rev);

#define deftrap(f) define_trap((f), 0, "")
#ifdef TRACE_TRAPS
#define deftrap2(f, mode, str) define_trap((f), (mode), (str))
#define deftrapres(f, mode, str) define_trap((f), (mode | TRAPFLAG_UAERES), (str))
#else
#define deftrap2(f, mode, str) define_trap((f), (mode), "")
#define deftrapres(f, mode, str) define_trap((f), (mode | TRAPFLAG_UAERES), "")
#endif

extern void align (int);

extern volatile int uae_int_requested;
extern void set_uae_int_flag (void);

#define RTS 0x4e75
#define RTE 0x4e73

extern uaecptr EXPANSION_explibname, EXPANSION_doslibname, EXPANSION_uaeversion;
extern uaecptr EXPANSION_explibbase, EXPANSION_uaedevname, EXPANSION_haveV36;
extern uaecptr EXPANSION_bootcode, EXPANSION_nullfunc;

extern uaecptr ROM_filesys_resname, ROM_filesys_resid;
extern uaecptr ROM_filesys_diagentry;
extern uaecptr ROM_hardfile_resname, ROM_hardfile_resid;
extern uaecptr ROM_hardfile_init;
extern uaecptr filesys_initcode;

extern void filesys_install (void);
extern void filesys_install_code (void);
extern void filesys_store_devinfo (uae_u8 *);
extern void hardfile_install (void);
extern void hardfile_reset (void);
extern void hardfile_cleanup (void);
extern void emulib_install (void);
extern void expansion_init (void);
extern void expansion_cleanup (void);
extern void expansion_clear (void);

extern uae_u8 *rtarea;
