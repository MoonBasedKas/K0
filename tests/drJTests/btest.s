.section .rodata
.align 8
.LC0:
	.string "Default k0 test string."
.text
.globl main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
L1327865680:
L1327852224:
	movq	$1, %rax
	movq	%rax, -0(%rbp)
	movq	$2, %rax
	movq	-0(%rbp), %rbx
	movq	%rax, %rcx
	addq	%rbx, %rcx
	movq	%rcx, -16(%rbp)
	movq	$10, %rax
	movq	-16(%rbp), %rbx
	movq	%rax, %rcx
	addq	%rbx, %rcx
	movq	%rcx, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
