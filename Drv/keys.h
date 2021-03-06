#ifndef __KEYS_H__
#define __KEYS_H__

/*
#define PASSIVE_LEVEL		0
#define LOW_LEVEL		0
#define APC_LEVEL		1
#define DISPATCH_LEVEL		2
#define PROFILE_LEVEL		27
#define CLOCK1_LEVEL		28
#define CLOCK2_LEVEL		28
#define IPI_LEVEL		29
#define POWER_LEVEL		30
#define HIGH_LEVEL		31
*/

#define LCONTROL		((unsigned short)0x1D) 
#define CAPS_LOCK		((unsigned short)0x3A) 
#define Q_CONTROL		((unsigned short)0x10) /* й */
#define W_CONTROL		((unsigned short)0x11) /* ц */
#define E_CONTROL		((unsigned short)0x12) /* у */
#define R_CONTROL		((unsigned short)0x13) /* к */
#define T_CONTROL		((unsigned short)0x14) /* е */
#define Y_CONTROL		((unsigned short)0x15) /* н */
#define U_CONTROL		((unsigned short)0x16) /* г */
#define I_CONTROL		((unsigned short)0x17) /* ш */
#define O_CONTROL		((unsigned short)0x18) /* щ */
#define P_CONTROL		((unsigned short)0x19) /* з */
#define A_CONTROL		((unsigned short)0x1e) /* ф */
#define S_CONTROL		((unsigned short)0x1f) /* ы */
#define D_CONTROL		((unsigned short)0x20) /* в */
#define F_CONTROL		((unsigned short)0x21) /* а */
#define G_CONTROL		((unsigned short)0x22) /* п */
#define H_CONTROL		((unsigned short)0x23) /* р */
#define J_CONTROL		((unsigned short)0x24) /* о */
#define K_CONTROL		((unsigned short)0x25) /* л */
#define L_CONTROL		((unsigned short)0x26) /* д */
#define Z_CONTROL		((unsigned short)0x2c) /* я */
#define X_CONTROL		((unsigned short)0x2d) /* ч */
#define C_CONTROL		((unsigned short)0x2e) /* с */
#define V_CONTROL		((unsigned short)0x2f) /* м */
#define B_CONTROL		((unsigned short)0x30) /* и */
#define N_CONTROL		((unsigned short)0x31) /* т */
#define M_CONTROL		((unsigned short)0x32) /* ь */

#define L_SHIFT			((unsigned short)0x2a)
#define R_SHIFT			((unsigned short)0x36)
#define TAB_CONTROL		((unsigned short)0xf)
#define L_CTRL			((unsigned short)0x1d)
#define R_CTRL			((unsigned short)0x1d)
#define L_ALT			((unsigned short)0x38)
#define R_ALT			((unsigned short)0x38)
#define WIN_CONTROL		((unsigned short)0x5b)
#define SPACE_CONTROL	((unsigned short)0x39)
#define LEFT_ARROW		((unsigned short)0x4b)
#define RIGHT_ARROW		((unsigned short)0x4d)
#define UP_ARROW		((unsigned short)0x48)
#define DOWN_ARROW		((unsigned short)0x50)
#define X_CONTROL1		((unsigned short)0x29) // `
#define X_CONTROL2		((unsigned short)0x33) // , /* б */
#define X_CONTROL3		((unsigned short)0x34) // . /* ю */
#define X_CONTROL4		((unsigned short)0x35) // /
#define X_CONTROL5		((unsigned short)0x27) // ; /* ж */
#define X_CONTROL6		((unsigned short)0x28) // ' /* э */
#define X_CONTROL7		((unsigned short)0x2b) // \ 
#define X_CONTROL8		((unsigned short)0x1a) // [ /* х */
#define X_CONTROL9		((unsigned short)0x1b) // ] /* ъ *
#define X_CONTROL10		((unsigned short)0xc) // -
#define X_CONTROL11		((unsigned short)0xd) // =
#define BACKSPACE		((unsigned short)0xe)
#define ENTER			((unsigned short)0xc)
#define END_CONTROL		((unsigned short)0x4f)
#define PGDN_CONTROL	((unsigned short)0x51)
#define PGUP_CONTROL	((unsigned short)0x49)
#define HOME_CONTROL	((unsigned short)0x47)
#define DEL_CONTROL		((unsigned short)0x53)
#define INS_CONTROL		((unsigned short)0x52)
#define PAUSEBREAK		((unsigned short)0x45)
#define PRTSC_SYSR		((unsigned short)0x37)

#define F12_CONTROL		((unsigned short)0x58)
#define F11_CONTROL		((unsigned short)0x57)
#define F10_CONTROL		((unsigned short)0x44)
#define F9_CONTROL		((unsigned short)0x43)
#define F8_CONTROL		((unsigned short)0x42)
#define F7_CONTROL		((unsigned short)0x41)
#define F6_CONTROL		((unsigned short)0x40)
#define F5_CONTROL		((unsigned short)0x3f)
#define F4_CONTROL		((unsigned short)0x3e)
#define F3_CONTROL		((unsigned short)0x3d)
#define F2_CONTROL		((unsigned short)0x3c)
#define F1_CONTROL		((unsigned short)0x3b)

#endif 