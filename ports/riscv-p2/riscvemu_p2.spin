'#define CHECK_WRITES
#define DEBUG
#define USE_LUT_FOR_CODE
{{
   RISC-V Emulator for Parallax Propeller
   Copyright 2017 Total Spectrum Software Inc.
   Terms of use: MIT License (see the file LICENSE.txt)

   An emulator for the RISC-V processor architecture, designed to run
   in a single Propeller COG.

   The interface is pretty simple: the params array passed to the start
   method should contain:
   
   params[0] = address of command register
   params[1] = base of emulated memory
   params[2] = size of emulated memory (stack will start at base + size and grow down)
   params[3] = initial pc (usually 0)
   params[4] = address for register dump (40 longs; x0-x31, pc, opcode, 4xinfo, stepcount, rununtil)

   The command register is used for communication from the RISC-V back to the host.
   The lowest 4 bits contain a command, as follows:
   $1 = single step (registers have been dumped)
   $2 = illegal instruction encountered (registers have been dumped)
   $D = request to dump checksum of memory
   $E = request to read a character (data goes in long after command register)
   $F = request to write character in bits 12-8 of command register

   The host should write 0 to the command register as an "ACK" to restart the RISC-V.
  ---------------------------------------------------------------------
  Reads and writes go directly to the host HUB memory. To access COG memory
  or special registers use the CSR instructions. CSRs we know about:
     7Fx - COG registers 1F0-1FF
     BC0 - UART register
     BC1 - wait register (not implemented)
     BC2 - debug register
     C00 - cycle counter   
}}

VAR
  long cog			' 1 + the cog id of the cog running the emulator
  
PUB start(params)
  cog := cognew(@enter, params) + 1
  return cog

PUB stop
  if (cog > 0)
    cogstop(cog-1)

DAT
		org 0
enter
x0		mov	temp, ptra
x1		rdlong	cmd_addr, temp
x2		add	temp, #4
x3		rdlong	membase, temp
x4		add	temp, #4
x5		rdlong	memsize, temp
x6		add	temp, #4
x7		rdlong	shadowpc, temp
x8		add	temp, #4
x9		mov	x0+2,memsize	' set up stack pointer
x10		add	x0+2,membase
x11		rdlong	dbgreg_addr, temp

		'' set up opcode table in LUT at offset 0
		'' initialize to all 0
x12		mov    dest, #0
x13		mov    temp, #$80
oinitlp
x14		wrlut	jmpillegalinstr, dest
x15		add	dest, #1
x16		djnz	temp, #oinitlp

		'' now set up some specific opcodes
x17		wrlut	jmploadop, #3
x18		wrlut	jmpimmop,  #3+($04<<2)
x19		wrlut	jmpauipc,  #3+($05<<2)
x20		wrlut	jmpstoreop,#3+(8<<2)
x21		wrlut	jmpregop,  #3+($0C<<2)
x22		wrlut	jmplui,    #3+($0D<<2)
x23		wrlut	jmpcondbranch, #3+($18<<2)
x24		wrlut	jmpjalr,   #3+($19<<2)
x25		wrlut	jmpjal,    #3+($1b<<2)
x26		wrlut	jmpsys,    #3+($1c<<2)
x27		mov	x0, #0
x28		wrlut	jmpcustom0, #3+($02<<2) 
x29		wrlut	jmpcustom1, #3+($0A<<2)
x30		nop
x31		jmp	#emustart

		'' registers
		'' pc must follow x0-x31
		'' next one after is also displayed in debug
shadowpc	long	0
opcode		long	0
info1		long	0	' debug info
info2		long	0	' debug info
info3		long	0	' debug info
info4		long	0	' debug info
rununtil	long	0
stepcount	long	0	' 1 to start in single step mode
#ifdef DEBUG
debugtrace	long	0
lastpc		long	0
#endif

cycleh		long	0
lastcnt		long	$0
chfreq		long	$80000000

		'' ISR for CT rolling over
ct3_isr
		addct3	lastcnt, chfreq
		testb	lastcnt, #31 wc
		addx	cycleh, #0
		reti3
		
emustart
		'' set up interrupt for CT3 == 0
		'' to measure cycle rollover
		getct   lastcnt
		and	lastcnt, chfreq
		addct3	lastcnt, chfreq
		mov   IJMP3, #ct3_isr
		setint3	#3   '' ct3

#ifdef USE_LUT_FOR_CODE
		' load helper code into LUT
		' LUT 00-7f is used for an opcode table
		' 80-1ff is free for code
		setq2   #$1ff-$80
		rdlong	$80, lut_code_ptr
		jmp	#nexti
lut_code_ptr
		long	@@@LUT_Helper
#else
		jmp	#nexti
#endif		
jmpillegalinstr
		long	illegalinstr
		
jmploadop
{00}		long	loadop	' load
jmpcustom0
{02}		long	custom0op	' immediate custom
jmpimmop
{04}		long	immediateop	' math immediate
jmpauipc
{05}		long	auipc		' auipc
jmpstoreop
{08}		long	storeop	' store
jmpcustom1
{0A}		long	custom1op	' reg/reg  custom
jmpregop
{0C}		long	regop		' math reg
jmplui
{0D}		long	lui		' lui
jmpcondbranch
{18}		long	condbranch	' conditional branch
jmpjalr
{19}		long	jalr
jmpjal
{1B}		long	jal
jmpsys
{1C}		long	sysinstr	' system

''
'' table for "regular" math operations
'' note that if bits 31..25 of the opcode == 1, we should use
'' the "mul" table instead
'' 
mathregtab
{0}		long	imp_addsub	'' add or sub, based on imm field
{1}		long	imp_sll	'' shl
{2}		long	imp_slt	'' set if less than, signed
{3}		long	imp_sltu	'' set if less than, unsigned
{4}		long	imp_xor	'' xori
{5}		long	imp_shr	'' srli or srai, based on imm 
{6}		long	imp_or		'' ori
{7}		long	imp_and	'' andi

multab
{0}		long	imp_mul
{1}		long	illegalinstr	'' mulh, not implemented
{2}		long	illegalinstr	'' mulhsu, not implemented
{3}		long	imp_muluh
{4}		long	imp_div
{5}		long	imp_divu
{6}		long	imp_rem
{7}		long	imp_remu

custom0tab
		long	illegalinstr
		long	illegalinstr
		long	drvpininstr
		long	fltpininstr
		long	outpininstr
		long	dirpininstr
		long	wrpininstr
		long	getpininstr
		
		''
		'' main instruction decode loop
		''

		'' write back last result from "dest" here
write_and_nexti
		mov	0-0, dest
nexti
#ifdef DEBUG
		call	#checkdebug
#endif
		rdlong	opcode, shadowpc
		add	shadowpc, #4
   		mov	temp, opcode
		shr	temp, #7
		and	temp, #$1f wz
   if_z		mov	temp, #dest	' writes to x0 get ignored
   		setd	write_and_nexti, temp
		mov	temp, opcode
		and	temp, #$7f
		rdlut	info1, temp
		jmp	info1

		
		'' come here for illegal instructions
illegalinstr
		call	#dumpregs
		mov	newcmd, #2	' signal illegal instruction
		call	#waitcmdclear
		call	#sendcmd
		jmp	#nexti


		''
		'' utility: extract rs1 field from opcode
		'' NOTE: does not actually read value from register
		''
getrs1
		mov	rs1, opcode
		shr	rs1, #15
    _ret_	and	rs1, #$1f


getrs2
		mov	rs2, opcode
		sar	rs2, #20
    _ret_	and	rs2, #$1f


		'' extract funct3 field
getfunct3
		mov	funct3, opcode
		shr	funct3, #12
    _ret_	and	funct3, #7

    		'' extract all of funct3, rs2, rs1
decodeall
		mov	rs1, opcode
		shr	rs1, #15
    		and	rs1, #$1f
		mov	rs2, opcode
		sar	rs2, #20
    		and	rs2, #$1f
		mov	funct3, opcode
		shr	funct3, #12
    _ret_	and	funct3, #7

mulbit		long	(1<<25)

mathskiptab
{0}		long	%11_111_1_1111_1_0_00	'' add 
{1}		long	%11_111_1_1111_0_1_00 	'' imp_sll	'' shl
{2}		long	%11_111_1_0010_1_1_00 	'' imp_slt	'' set if less than, signed
{3}		long	%11_111_1_0001_1_1_00	'' imp_sltu	'' set if less than, unsigned
{4}		long	%11_111_0_1111_1_1_00	'' imp_xor	'' xori
{5}		long	%11_000_1_1111_1_1_00	'' imp_shr	'' srli or srai, based on imm 
{6}		long	%10_111_1_1111_1_1_00	'' imp_or
{7}		long	%01_111_1_1111_1_1_00	'' imp_and

'' math immediate operations
'' implemented by skipping over some other instructions

immediateop
		mov	rs2, opcode
		sar	rs2, #20
		call	#getrs1
		call	#getfunct3
		alts	funct3, #mathskiptab	' funct3 pts at instruction
		mov	funct3, 0-0
		
		'' actually execute the decoded instructions below
		skipf	funct3
		
		alts	rs1, #x0
		mov	dest, 0-0		' load rs1 into dest
		
		add	dest, rs2
		
		shl	dest, rs2

		cmps	dest, rs2 wc		' slt
		cmp	dest, rs2 wc		' sltu
		mov	dest, #0
		muxc	dest, #1

		xor	dest, rs2

		'' depending on opcode we do sar or shr
		test	opcode, sra_mask wc
	if_nc	shr	dest, rs2
	if_c	sar	dest, rs2

		or	dest, rs2

		and	dest, rs2

		jmp	#write_and_nexti

		'' math register operations
regop
		call	#decodeall
		alts	rs2, #x0
		mov	rs2, 0-0
		test	opcode, mulbit wz
		mov	desth, #mathregtab
	if_nz	mov	desth, #multab

		'' fall through
		
		'' generic math routine
		'' enter with rs2 having the decoded value of rs2
		'' (register fetched if applicable)
		'' and with desth containing the table to use
		'' (generic mathtab, or multab)
domath
		alts	rs1, #x0
		mov	dest, 0-0		' load rs1 into dest
		alts	funct3, desth		' funct3 pts at instruction
		mov	funct3, 0-0
		'' actually execute the decoded instruction here
		jmp	funct3

		'' execute math instructions
		'' for all of these, rs1 is in temp, rs2 has the needed value
		'' result should go in temp
imp_add
		add	dest, rs2
		jmp	#write_and_nexti
imp_addsub
		test	opcode, sra_mask wc
		sumc	dest, rs2	 ' if C=1 dest = dest - rs2, otherwise dest = dest + rs2
		jmp	#write_and_nexti

imp_sll		shl	dest, rs2
		jmp	#write_and_nexti
		
imp_slt		cmps	dest, rs2 wz,wc
		mov	dest, #0
		muxc	dest, #1
  		jmp	#write_and_nexti
imp_sltu	cmp	dest, rs2 wz,wc
		mov	dest, #0
		muxc	dest, #1
		jmp	#write_and_nexti
imp_xor
		xor	dest, rs2
		jmp	#write_and_nexti
imp_shr
		'' depending on opcode we do sar or shr
		test	opcode, sra_mask wc
	if_nc	shr	dest, rs2
	if_c	sar	dest, rs2
		jmp	#write_and_nexti
		
imp_or		or	dest, rs2
		jmp	#write_and_nexti
		
imp_and		and	dest, rs2
		jmp	#write_and_nexti
		
		'' for sra 31..26 = 16
		'' so 31..24 = 64 = $40
sra_mask	long	$40000000

'' load upper immediate (20 bits)
lui
    		'' extract upper immediate
		mov	dest, opcode
		and	dest, luimask
		jmp	#write_and_nexti
luimask		long	$fffff000


'' load upper 20 bits, added with pc
auipc
		mov	dest, opcode
		and	dest, luimask
		add	dest, shadowpc
		sub	dest, #4
		jmp	#write_and_nexti

''''''''''''''''''''''''''''''''''''''''''''''''''''
'' jal: jump and link
''''''''''''''''''''''''''''''''''''''''''''''''''''
Jmask		long	$fff00fff
bit11		long	(1<<11)
jal
		mov	temp, opcode	' extract J-immediate
		sar	temp, #20	' sign extend, get some bits in place
		and	temp, Jmask
		andn	opcode, Jmask
		or	temp, opcode	' set bits 19:12
		test	temp, #1 wc	' check old bit 20
		andn	temp, #1 	' clear low bit
		muxc	temp, bit11	' set bit 11
		mov	dest, shadowpc
		sub	shadowpc, #4		' compensate for pc bump
		add	shadowpc, temp
		andn	shadowpc, #1	' clear low bit
		jmp	#write_and_nexti

jalr
		call	#getrs1
		sar	opcode, #20	' get offset

		mov	dest, shadowpc
		alts	rs1, #x0
		mov	shadowpc, 0-0	' fetch rs1 value
		add	shadowpc, opcode
		andn	shadowpc, #1	' clear low bit
		jmp	#write_and_nexti
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' implement load and store
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'
' table of load instructions
' the I field gives the PASM instruction we have to use to implement
' the load; the S field is 0 for unsigned load, 1 for signed

loadtab
		long	do_rdbytes
		long	do_rdwords
		long	do_rdlong
		long	illegalinstr
		long	do_rdbyte
		long	do_rdword
		long	do_rdlong
		long	illegalinstr
loadop
		call	#getrs1
		call	#getfunct3
		alts	rs1, #x0
    		mov	dest, 0-0	' set dest to value of rs1
		sar	opcode, #20	' extract immediate
		add	dest, opcode	' add offset
		alts	funct3, #loadtab
		mov	funct3, 0-0
		jmp	funct3

bytesignextend	long	$FFFFFF00
wordsignextend	long	$FFFF0000

do_rdbyte
		rdbyte	dest, dest
		jmp	#write_and_nexti
		
do_rdbytes
		rdbyte	dest, dest wc
		muxc	dest, bytesignextend
		jmp	#write_and_nexti
do_rdword
		rdword	dest, dest
		jmp	#write_and_nexti
do_rdwords
		rdword	dest, dest wc
		muxc	dest, wordsignextend
		jmp	#write_and_nexti
do_rdlong
		rdlong	dest, dest
		jmp	#write_and_nexti

		''
		'' re-order bits of opcode so that it is
		'' an s-type immediate value
get_s_imm
		mov	temp, opcode
		shr	temp, #7
		and	temp, #$1f
		sar	opcode, #20
		andn	opcode, #$1f
    _ret_	or	opcode, temp	' opcode has offset

storetab
		long	do_wrbyte
		long	do_wrword
		long	do_wrlong
		long	illegalinstr
		
storeop
		call	#getrs2
		call	#getrs1
		call	#getfunct3
		alts	rs2, #x0
		mov	dest, 0-0	' set dest to value of rs2 (value to store)
		alts	rs1, #x0
     		mov	rs1, 0-0	' set rs1 to address of memory
		test	funct3, #4 wz	' check for signed/unsigned; Z is set for signed
	if_nz	jmp	#illegalinstr
		and	funct3, #3

		'' extract s-type immediate
		call	#get_s_imm
		add	rs1, opcode	' find address
		alts	funct3, #storetab
		mov	funct3, 0-0
		jmp	funct3		' go do store

iobase		long	$f0000000
do_wrlong
		wrlong	dest, rs1
#ifdef CHECK_WRITES
		cmp	rs1, watchaddr wz
	if_z	call	#breakit
#endif
		jmp	#nexti		' no writeback

do_wrword
		wrword	dest, rs1
		jmp	#nexti		' no writeback
do_wrbyte
		wrbyte	dest, rs1
		jmp	#nexti		' no writeback

''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' implement csrrw instruction
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
sysinstrtab
		long	illegalinstr
		long	csrrw
		long	csrrs
		long	csrrc
		long	illegalinstr
		long	illegalinstr
		long	illegalinstr
		long	illegalinstr
sysinstr
		call	#getrs1
		call	#getfunct3
		shr	opcode, #20	' extract CSR address
		alts	funct3, #sysinstrtab
		mov	funct3, 0-0
		jmp	funct3

''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' implement conditional branches
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
condbranch_tab
		long	do_beq
		long	do_bne
		long	illegalinstr
		long	illegalinstr
		long	do_blt
		long	do_bge
		long	do_bltu
		long	do_bgeu
condbranch
		call	#decodeall
		alts	rs1, #x0
		mov	rs1, 0-0
		alts	rs2, #x0
        	mov	rs2, 0-0
		call	#get_s_imm	' opcode now contains s-type immediate
		test	opcode, #1 wc	' get low bit into carry
		muxc	opcode, bit11	' copy up to bit 11
		andn	opcode, #1	' clear low bit
		alts	funct3, #condbranch_tab
		mov	funct3, 0-0
		jmp	funct3

do_beq
		cmp	rs1, rs2 wz
	if_nz	jmp	#nexti
		'' fall through
takebranch
		add	opcode, shadowpc
		sub	opcode, #4	' opcode now has desired destination
		mov	shadowpc, opcode
		jmp	#nexti

do_bne
		cmp	rs1, rs2 wz
	if_z	jmp	#nexti
		jmp	#takebranch
		
do_bltu
		cmp	rs1, rs2 wcz
	if_nc	jmp	#nexti
		jmp	#takebranch
do_blt
		cmps	rs1, rs2 wcz
	if_nc	jmp	#nexti
		jmp	#takebranch
		
do_bgeu
		cmp	rs1, rs2 wcz
	if_c	jmp	#nexti
		jmp	#takebranch
do_bge
		cmps	rs1, rs2 wcz
	if_c	jmp	#nexti
		jmp	#takebranch

''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' unsigned multiply rs1 * rs2 -> (dest, desth)
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
imp_mul
		call	#umul
		jmp	#write_and_nexti
imp_muluh	call	#umul
		mov	dest, desth
		jmp	#write_and_nexti

imp_divu	call	#udiv
		jmp	#write_and_nexti
imp_remu	call	#udiv
		mov	dest, desth
		jmp	#write_and_nexti

imp_rem
		mov	divflags, dest	' remainder should have sign of rs1
		abs	dest, dest
		abs	rs2, rs2
		call	#udiv
		mov	dest, desth
		testb	divflags, #31 wc
	if_c	neg	dest
		jmp	#write_and_nexti
		
imp_div
		mov	divflags, dest
		xor	divflags, rs2
		abs	dest, dest 
		abs	rs2, rs2
		call	#udiv
		testb	divflags, #31 wc	' check sign
		negc	dest
		jmp	#write_and_nexti

umul
		qmul	dest, rs2
		getqx	dest
    _ret_	getqy	desth

		'' calculate dest / rs2; result in dest, remainder in desth
udiv
		tjz	rs2, #div_by_zero
		setq	#0		' set high 64 bits
		qdiv	dest, rs2	' dest/rs2
		getqx	dest  		' quotient
    _ret_	getqy	desth		' remainder

div_by_zero
		neg	dest, #1
		mov	desth, rs1
		ret

''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' debug routines
''''''''''''''''''''''''''''''''''''''''''''''''''''''''

checkdebug
		tjz	debugtrace, #skip_memchk
		mov	temp, lastpc
		xor	temp, shadowpc
		andn	temp, #$F wz
	if_z	jmp	#skip_memchk
		call	#mem_checksum
skip_memchk
		mov	lastpc, shadowpc
		tjz	stepcount, #checkdebug_ret
		djnz	stepcount, #checkdebug_ret
breakit
		call	#dumpregs
		mov	newcmd, #1	' single step command
		call	#sendcmd	' send info
		call	#waitcmdclear	' wait for response
		jmp	#readregs	' read new registers
checkdebug_ret
		ret
		
newcmd		long 0

sendrecvcmd
		call	#waitcmdclear
		wrlong	newcmd, cmd_addr
		call	#waitcmdclear
		add	cmd_addr, #4
		rdlong	dest, cmd_addr
		sub	cmd_addr, #4
sendrecvcmd_ret	ret

t0		long	0
waitcmdclear
		rdlong	t0, cmd_addr wz
	if_nz	jmp	#waitcmdclear	' far end is still processing last command
waitcmdclear_ret
		ret
		
	
dumpregs
		mov	cogaddr, #x0
		mov	hubaddr, dbgreg_addr
		mov	hubcnt, #40*4
		call	#cogxfr_write
		ret

'------------------------------------------------------------------------------
' routines for fast transfer of COG memory to/from hub
' "hubaddr"   is the HUB memory address
' "cogaddr"   is the COG memory address
' "hubcnt"    is the number of *bytes* to transfer
'------------------------------------------------------------------------------

cogxfr_read
		setd	.rdins, cogaddr
		add	hubcnt, #3
		shr	hubcnt, #2	' count of longs
		sub	hubcnt, #1
		setq	hubcnt
.rdins		rdlong	0-0, hubaddr
		ret
		
cogxfr_write
		setd	.rdins, cogaddr
		add	hubcnt, #3
		shr	hubcnt, #2	' count of longs
		sub	hubcnt, #1
		setq	hubcnt
.rdins		wrlong	0-0, hubaddr
		ret

temp		long 0
dest		long 0
desth		long 0	' high word of result for mul/div
dbgreg_addr	long 0	' address where registers go in HUB during debug
cmd_addr	long 0	' address of HUB command word
membase		long 0	' base of emulated RAM
memsize		long 0	' size of emulated RAM
hubaddr		long 0
cogaddr		long 0
hubcnt		long 0
watchaddr	long 0  '$79f18 + (4*13)

rd		long	0
rs1		long	0
rs2		long	0
rs3		long	0
funct3		long	0
funct2		long	0
divflags	long	0

		fit	$1f0	'$1F0 is whole thing

#ifdef USE_LUT_FOR_CODE
		' helper code in LUT
		org	 $280
LUT_Helper
#else
		orgh
#endif		
readregs
		mov	cogaddr, #x0
		mov	hubaddr, dbgreg_addr
		mov	hubcnt, #40*4
		call	#cogxfr_read
		mov	x0, #0
		ret

sendcmd
		call	#waitcmdclear
		wrlong	newcmd, cmd_addr
		ret

''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' implement custom instructions
''''''''''''''''''''''''''''''''''''''''''''''''''''''''
custom0op
		call	#decodeall
	
		' these are the various set pin instructions
		alts	rs2, #x0
		mov	dest, 0-0	' set dest to value of rs2 (value to write)
		alts	rs1, #x0
		mov	rs1, 0-0	' set rs1 to pin offset
		
		cmp	funct3, #7 wz
	if_z	jmp	#getpininstr

		'' extract s-type immediate into "opcode"
		call	#get_s_imm
		mov	funct2, opcode
		sar	funct2, #10
		and	funct2, #3
		and	opcode, #$3f	' isolate pin
		add	rs1, opcode	' update pin value
		alts	funct3, #custom0tab
		mov	funct3,0-0
		jmp	funct3		' go do operation

		'' fetch pin value
		'' similar to a load instruction
getpininstr
		shr	opcode, #20	' extract immediate
		mov	funct2, opcode
		shr	funct2, #10	' extract function selector
		and	opcode, #$3f	' extract pin mask
		add	rs1, opcode	' rs1 contains pin to fetch
		rczr	funct2 wcz	' c = D[1], z = d[0]
  if_00		jmp	#do_testp
  if_01		rdpin	dest, rs1
  if_10		rqpin	dest, rs1
  if_11		akpin	rs1
  		jmp	#write_and_nexti
do_testp	testp	rs1 wc
  		wrc	dest
		jmp	#write_and_nexti

		'' do one of wrpin, wxpin, wypin
wrpininstr
		rczr	funct2 wcz
    if_00	wrpin	dest, rs1
    if_01	wxpin	dest, rs1
    if_10	wypin	dest, rs1
    if_11	jmp	#illegalinstr
    		jmp	#nexti

		' pin instructions; funct2 controls
		' value to use for pin
		' 00 = use dest
		' 01 = use !dest
		' 10 = set dest to random
		' 11 = use !(old pin value)
get_pinval
		rczr	funct2 wcz
    if_01	not	dest
    if_10	getrnd	dest
    if_11	call	#get_not_old_pinval
    		testb	dest, #0 wc
    		ret
		
get_not_old_pinval
		test	rs1, #$20 wc	' bits 32-63?
    if_c	mov	dest, outb
    if_nc	mov	dest, outa
    		testb	dest, rs1 wc
    		wrnc	dest
    		ret

drvpininstr
		call	#get_pinval
    		drvc	rs1
		jmp	#nexti
fltpininstr
		call	#get_pinval
    		fltc	rs1
		jmp	#nexti
dirpininstr
		call	#get_pinval
    		dirc	rs1
    		jmp	#nexti
outpininstr
		call	#get_pinval
    		outc	rs1
		jmp	#nexti
custom1op
		call	#decodeall
		mov	info1, opcode
		mov	info2, #$ff
		cmp	funct3, #0 wz
    if_nz	jmp	#not_coginit
    		alts	rs1, #x0
		mov	rs1, 0-0
		alts	rs2, #x0
		mov	rs2, 0-0
		mov	funct2, opcode
		shr	funct2, #25
		mov	rs3, funct2
		shr	rs3, #2
		and	funct2, #3 wz
		mov	info4, rs3
		mov	info3, funct2
		mov	info2, #$ee
    if_nz	jmp	#illegalinstr

		alts	rs3, #x0
		mov	rs3, 0-0
		mov	dest, rs1
    		setq	rs3
		coginit dest, rs2 wc
    if_c	neg	dest, #1
		jmp	#write_and_nexti
not_coginit
		jmp	#illegalinstr

csrrw
		mov	funct3, #0
		jmp	#do_csr
csrrs
		mov	funct3, #1
		jmp	#do_csr
csrrc
		mov	funct3, #2
do_csr
		mov	info3, opcode
		mov	dest, #0
		cmp	opcode, ##$C00 wz, wc
	if_nz	jmp	#not_timer
		getct	dest
		jmp	#write_and_nexti
not_timer
		cmp	opcode, ##$C80 wz
	if_nz	jmp	#not_timerh
		mov	dest, cycleh
		jmp	#write_and_nexti
not_timerh
		cmp	opcode, ##$BC0 wz
	if_nz	jmp	#not_uart
		'' is this a send or receive
		cmp	rs1, #0 wz
	if_nz	jmp	#uart_send
		mov	newcmd, #$E	' read
		call	#sendrecvcmd
		jmp	#write_and_nexti
uart_send
		alts	rs1, #x0
		mov	newcmd, 0-0
		shl	newcmd, #4
		or	newcmd, #$F
		call	#sendcmd
		jmp	#nexti
not_uart
		cmp	opcode, ##$BC1 wz
	if_nz	jmp	#not_waitct
		alts	rs1, #x0
		mov	rs1, 0-0
		addct1	rs1, #0
		waitct1
		jmp	#nexti
not_waitct
		cmp	opcode, ##$BC2 wz
	if_nz	jmp	#not_debug
		alts	rs1, #x0
		mov	debugtrace, 0-0
		call	#mem_checksum
		jmp	#nexti
not_debug
		' check for COG register
		getnib	funct2, opcode, #2
		mov	temp, opcode
		and	temp, #$1FF
		cmp	funct2, #7 wz
	if_nz	jmp	#illegalinstr

		' first, get the old value
		alts	temp, #0
		mov	dest, 0-0
skip_read
		' now write the new value to the hw register, unless src is x0
		cmp	rs1, #0 wz
	if_e	jmp   	#skip_write
		alts  	rs1, #x0
		mov   	rs1, 0-0
		' depending on opcode, we may want to do a mov, or, or andn
		rczr	funct3 wcz
	if_00	altd	temp, #0
	if_00	mov   	0-0, rs1
	if_01	altd  	temp, #0
	if_01	or    	0-0, rs1
	if_10	altd  	temp, #0
	if_10	andn  	0-0, rs1
skip_write
		jmp	#write_and_nexti

mem_checksum
		call	#dumpregs
		mov	newcmd, #$D
		jmp	#sendrecvcmd
		

END_LUT_Helper
		fit	$3ff
