.section .rodata
.align 8
.LC0:
	.string "Default k0 test string."
.text
.globl main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$8, %rsp
L-288028976:
L-288044352:
	movq	$5, %rax
	movq	%rax, -0(%rbp)
L1:
	movq	-0(%rbp), %rax
	movq	$0, %rbx
	cmpq	%rbx, %rax
	jg	L8
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	je	L2
	jmp	L2
	movq	-0(%rbp), %rax
	movq	$1, %rbx
	movq	%rax, %rcx
	subq	%rbx, %rcx
	movq	%rcx, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -0(%rbp)
	jmp	L1
L2:

