.text
.globl main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	$1, %rax
	movq	%rax, -8(%rbp)
	movq	$2, %rax
	movq	-8(%rbp), %rbx
	movq	%rax, %rcx
	addq	%rbx, %rcx
	movq	%rcx, -24(%rbp)
	movq	$10, %rax
	movq	-24(%rbp), %rbx
	movq	%rax, %rcx
	addq	%rbx, %rcx
	movq	%rcx, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -16(%rbp)
