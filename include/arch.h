#ifndef ARCH_H
#define ARCH_H

typedef struct registers_t {
	char name[32];
	uint64_t offset;
	uint64_t size;
	uint64_t value;
} registers_t;

struct registers_t x86_64[] = {
	{"rax",		0,		8,	0},
	{"rbx",		8,		8,	0},
	{"rcx",		16,		8,	0},
	{"rdx",		24,		8,	0},
	{"rsi",		32,		8,	0},
	{"rdi",		40,		8,	0},
	{"rbp",		48,		8,	0},
	{"rsp",		56,		8,	0},
	{"r8",		64,		8,	0},
	{"r9",		72,		8,	0},
	{"r10",		80,		8,	0},
	{"r11",		88,		8,	0},
	{"r12",		96,		8,	0},
	{"r13",		104,	8,	0},
	{"r14",		112,	8,	0},
	{"r15",		120,	8,	0},
	{"rip",		128,	8,	0},
	{"eflags",136,	4,	0},
	{"cs",		140,	4,	0},
	{"ss",		144,	4,	0},
	{"ds",		148,	4,	0},
	{"es",		152,	4,	0},
	{"fs",		156,	4,	0},
	{"gs",		160,	4,	0},
	{"st0",		164,	10,	0},
	{"st1",		174,	10,	0},
	{"st2",		184,	10,	0},
	{"st3",		194,	10,	0},
	{"st4",		204,	10,	0},
	{"st5",		214,	10,	0},
	{"st6",		224,	10,	0},
	{"st7",		234,	10,	0},
	{"fctrl",	244,	4,	0},
	{"fstat",	248,	4,	0},
	{"ftag",	252,	4,	0},
	{"fiseg",	256,	4,	0},
	{"fioff",	260,	4,	0},
	{"foseg",	264,	4,	0},
	{"fooff",	268,	4,	0},
	{"fop",		272,	4,	0},
	{"xmm0",	276,	16,	0},
	{"xmm1",	292,	16,	0},
	{"xmm2",	308,	16,	0},
	{"xmm3",	324,	16,	0},
	{"xmm4",	340,	16,	0},
	{"xmm5",	356,	16,	0},
	{"xmm6",	372,	16,	0},
	{"xmm7",	388,	16,	0},
	{"xmm8",	404,	16,	0},
	{"xmm9",	420,	16,	0},
	{"xmm10",	436,	16,	0},
	{"xmm11",	452,	16,	0},
	{"xmm12",	468,	16,	0},
	{"xmm13",	484,	16,	0},
	{"xmm14",	500,	16,	0},
	{"xmm15",	516,	16,	0},
	{"mxcsr",	532,	4,	0}
};

#endif
