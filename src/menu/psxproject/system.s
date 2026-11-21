.set noreorder
.set noat

## Exception handler

.set BADV,  $8
.set SR,    $12
.set CAUSE, $13
.set EPC,   $14

.section .text._exceptionVector, "ax", @progbits
.global _exceptionVector
.type _exceptionVector, @function

_exceptionVector:
	# This tiny stub is going to be relocated to the address the CPU jumps to
	# when an exception occurs (0x80000080) at runtime, overriding the default
	# one installed by the BIOS. We're going to fetch a pointer to the current
	# thread, grab the EPC (i.e. the address of the instruction that was being
	# executed before the exception occurred) and jump to the exception handler.
	# NOTE: we can't use any registers other than $k0 and $k1 here, as doing so
	# would destroy their contents and corrupt the current thread's state.
	lui   $k0, %hi(currentThread)
	lw    $k0, %lo(currentThread)($k0)

	j     _exceptionHandler
	mfc0  $k1, EPC

.section .text._exceptionHandler, "ax", @progbits
.global _exceptionHandler
.type _exceptionHandler, @function

_exceptionHandler:
	# Save the full state of the thread in order to make sure the interrupt
	# handler callback (invoked later on) can use any register. The state of
	# $hi/$lo is saved after all other registers in order to let the multiplier
	# finish any ongoing calculation.
	sw    $at, 0x04($k0)
	sw    $v0, 0x08($k0)
	sw    $v1, 0x0c($k0)
	sw    $a0, 0x10($k0)
	sw    $a1, 0x14($k0)
	sw    $a2, 0x18($k0)
	sw    $a3, 0x1c($k0)
	sw    $t0, 0x20($k0)
	sw    $t1, 0x24($k0)
	sw    $t2, 0x28($k0)
	sw    $t3, 0x2c($k0)
	sw    $t4, 0x30($k0)
	sw    $t5, 0x34($k0)
	sw    $t6, 0x38($k0)
	sw    $t7, 0x3c($k0)
	sw    $s0, 0x40($k0)
	sw    $s1, 0x44($k0)
	sw    $s2, 0x48($k0)
	sw    $s3, 0x4c($k0)
	sw    $s4, 0x50($k0)
	sw    $s5, 0x54($k0)
	sw    $s6, 0x58($k0)
	sw    $s7, 0x5c($k0)
	sw    $t8, 0x60($k0)
	sw    $t9, 0x64($k0)
	sw    $gp, 0x68($k0)
	sw    $sp, 0x6c($k0)
	sw    $fp, 0x70($k0)
	sw    $ra, 0x74($k0)

	mfhi  $v0
	mflo  $v1
	sw    $v0, 0x78($k0)
	sw    $v1, 0x7c($k0)

	# Check bits 2-6 of the CAUSE register to determine what triggered the
	# exception. If it was caused by a syscall, increment EPC to make sure
	# returning to the thread won't trigger another syscall.
	mfc0  $v0, CAUSE
	lui   $v1, %hi(interruptHandler)

	andi  $v0, 0x1f << 2 # if (((CAUSE >> 2) & 0x1f) == 0) goto checkForGTEInst
	beqz  $v0, .LcheckForGTEInst
	li    $at, 8 << 2 # if (((CAUSE >> 2) & 0x1f) == 8) goto applyIncrement
	beq   $v0, $at, .LapplyIncrement
	lw    $v1, %lo(interruptHandler)($v1)

.LotherException:
	# If the exception was not triggered by a syscall nor by an interrupt call
	# _unhandledException(), which will then display information about the
	# exception and lock up.
	sw    $k1, 0x00($k0)

	mfc0  $a1, BADV # _unhandledException((CAUSE >> 2) & 0x1f, BADV)
	srl   $a0, $v0, 2
	jal   _unhandledException
	addiu $sp, -8

	b     .Lreturn
	addiu $sp, 8

.LcheckForGTEInst:
	# If the exception was caused by an interrupt, check if the interrupted
	# instruction was a GTE opcode and increment EPC to avoid executing it again
	# if that is the case. This is a workaround for a hardware bug.
	lw    $v0, 0($k1) # if ((*EPC >> 25) == 0x25) EPC++
	li    $at, 0x25
	srl   $v0, 25
	bne   $v0, $at, .LskipIncrement
	lw    $v1, %lo(interruptHandler)($v1)

.LapplyIncrement:
	addiu $k1, 4

.LskipIncrement:
	# Save the modified EPC and dispatch any pending interrupts. The handler
	# will temporarily use the current thread's stack.
	sw    $k1, 0x00($k0)

	lui   $a0, %hi(interruptHandlerArg)
	lw    $a0, %lo(interruptHandlerArg)($a0)
	jalr  $v1 # interruptHandler(interruptHandlerArg)
	addiu $sp, -8

	addiu $sp, 8

.Lreturn:
	# Grab a pointer to the next thread to be executed, restore its state and
	# return.
	lui   $k0, %hi(nextThread)
	lw    $k0, %lo(nextThread)($k0)
	lui   $at, %hi(currentThread)
	sw    $k0, %lo(currentThread)($at)

	lw    $v0, 0x78($k0)
	lw    $v1, 0x7c($k0)
	mthi  $v0
	mtlo  $v1

	lw    $k1, 0x00($k0)
	lw    $at, 0x04($k0)
	lw    $v0, 0x08($k0)
	lw    $v1, 0x0c($k0)
	lw    $a0, 0x10($k0)
	lw    $a1, 0x14($k0)
	lw    $a2, 0x18($k0)
	lw    $a3, 0x1c($k0)
	lw    $t0, 0x20($k0)
	lw    $t1, 0x24($k0)
	lw    $t2, 0x28($k0)
	lw    $t3, 0x2c($k0)
	lw    $t4, 0x30($k0)
	lw    $t5, 0x34($k0)
	lw    $t6, 0x38($k0)
	lw    $t7, 0x3c($k0)
	lw    $s0, 0x40($k0)
	lw    $s1, 0x44($k0)
	lw    $s2, 0x48($k0)
	lw    $s3, 0x4c($k0)
	lw    $s4, 0x50($k0)
	lw    $s5, 0x54($k0)
	lw    $s6, 0x58($k0)
	lw    $s7, 0x5c($k0)
	lw    $t8, 0x60($k0)
	lw    $t9, 0x64($k0)
	lw    $gp, 0x68($k0)
	lw    $sp, 0x6c($k0)
	lw    $fp, 0x70($k0)
	lw    $ra, 0x74($k0)

	jr    $k1
	rfe

## Fast reboot stubs

.set COP0_DCIC, $7

.section .text._fastRebootBreakVector, "ax", @progbits
.global _fastRebootBreakVector
.type _fastRebootBreakVector, @function

_fastRebootBreakVector:
	# When performing a fast reboot (i.e. forcing the BIOS to skip running the
	# shell and move immediately onto booting the CD-ROM), this 16-byte stub is
	# going to be placed at the address the CPU jumps to when a COP0 breakpoint
	# is hit (0x80000040). The breakpoint will be configured to trip when the
	# BIOS relocates the first byte of the shell to RAM; once that stage is
	# reached the code below will proceed to remove the breakpoint, undo the
	# write, then hijack the control flow and force the copying function to
	# return before anything else is copied.
	mtc0  $0, COP0_DCIC
	sw    $0, -1($a0)
	jr    $ra
	rfe

.section .text._fastRebootDummyShell, "ax", @progbits
.global _fastRebootDummyShell
.type _fastRebootDummyShell, @function

_fastRebootDummyShell:
	# Since the COP0 breakpoint will not prevent the BIOS from attempting to run
	# the shell it "loaded", this dummy function will be copied in its place.
	# This is the simplest possible implementation of a shell: once it returns,
	# the kernel will proceed to launch the boot executable on the CD-ROM. Note
	# that the first instruction here will be overwritten with a nop by
	# _fastRebootBreakVector().
	nop
	jr    $ra
	nop
	nop

.section .text._fastRebootKernelWipe, "ax", @progbits
.global _fastRebootKernelWipe
.type _fastRebootKernelWipe, @function

_fastRebootKernelWipe:
	# Faithful port of FUN_80019e44 from the rc5 reference menu (0x80019e44,
	# called right before the fast reboot). The fast path enters the BIOS at
	# the alternate entry point and skips the stage that initializes the
	# caches; this routine replicates that initialization by hand, so stale
	# cached data from the previous boot and from the menu runtime cannot
	# leak into the warm boot.
	#
	# The sequence is exactly the rc5 one: save SR and the cache control
	# register (0xfffe0130, "BCC"), program the I-cache into tag test mode
	# ((BCC & ~0x80) | 0x804), set SR to 0x10000 — the R3000 isolate-cache
	# bit — and sweep one full cache size (0x000..0xfff spans every line
	# index) with zero stores. In tag test mode the stores hit the I-cache
	# tag array and invalidate every line; with the data cache isolated
	# they cannot reach RAM, which is also why the raw kuseg addresses
	# never pass through the TLB. The BCC and SR are restored afterwards.
	#
	# Like the rc5 menu (which jumped straight to the uncached kseg1 mirror
	# 0xa0019e5c), the body executes from its kseg1 mirror because it
	# manipulates the cache control register itself. It clobbers
	# $a0..$a3, $t0, $t1 and $t9; the caller's return address is kept in
	# $t9 for the duration.
	la    $a0, .LkernelWipeBody   # kseg0 address of the body
	li    $a1, 0x20000000
	or    $a0, $a0, $a1           # kseg1 mirror
	move  $t9, $ra
	jr    $a0
	nop

.LkernelWipeBody:
	lui   $t0, 0xfffe             # cache control register page
	mfc0  $a2, SR
	lw    $a3, 0x130($t0)         # save BCC
	mtc0  $0, SR
	li    $a1, -0x81              # ~0x80
	and   $a1, $a1, $a3
	li    $t1, 0x804
	or    $a1, $a1, $t1
	sw    $a1, 0x130($t0)         # BCC = (BCC & ~0x80) | 0x804
	li    $t0, 0x10000            # SR = isolate-cache bit
	mtc0  $t0, SR
	ori   $a1, $0, 0xf00
1:	sw    $0, 0x00($a1)
	sw    $0, 0x10($a1)
	sw    $0, 0x20($a1)
	sw    $0, 0x30($a1)
	sw    $0, 0x40($a1)
	sw    $0, 0x50($a1)
	sw    $0, 0x60($a1)
	sw    $0, 0x70($a1)
	sw    $0, 0x80($a1)
	sw    $0, 0x90($a1)
	sw    $0, 0xa0($a1)
	sw    $0, 0xb0($a1)
	sw    $0, 0xc0($a1)
	sw    $0, 0xd0($a1)
	sw    $0, 0xe0($a1)
	sw    $0, 0xf0($a1)
	bgtz  $a1, 1b
	addiu $a1, $a1, -0x100
	mtc0  $0, SR                  # SR = 0
	nop
	lui   $t0, 0xfffe
	sw    $a3, 0x130($t0)         # restore BCC
	nop
	mtc0  $a2, SR                 # restore SR
	nop
	jr    $t9
	nop

.set IO_BASE, 0xbf801000

.set TIMER2_VALUE,  IO_BASE | 0x120
.set TIMER2_CTRL,   IO_BASE | 0x124
.set TIMER2_RELOAD, IO_BASE | 0x128

.section .text.delayMicrosecondsBusy, "ax", @progbits
.global delayMicrosecondsBusy
.type delayMicrosecondsBusy, @function

delayMicrosecondsBusy:
	# Calculate the approximate number of CPU cycles that need to be burned,
	# assuming a 33.8688 MHz clock (1 us = 33.8688 = ~33.875 cycles).
	sll   $a1, $a0, 8 # cycles = ((us * 271) + 4) / 8
	sll   $a2, $a0, 4
	addu  $a1, $a2
	subu  $a1, $a0
	addiu $a1, 4
	sra   $a0, $a1, 3

	# Compensate for the overhead of calculating the cycle count and returning.
	addiu $a0, -(6 + 1 + 2)

.Lloop: # while (cycles > 0) cycles -= 2
	bgtz  $a0, .Lloop
	addiu $a0, -2

	jr    $ra
	nop
