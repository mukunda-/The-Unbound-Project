;============================  The Unbound Project  ==========================;
;                                                                             ;
;========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========;

; poopies

SECTION .bss

global _PLANET_SEED_SOURCE
_PLANET_SEED_SOURCE:
	resd 1

SECTION .text

global _sample3_a

global _sunlight_beam_asm

; _sunlight_beam_asm( x, z )
;_sunlight_beam_asm:
;	enter	0x40
;
;	push	ebx
;	push	esi
;	push	edi
;
;
;
;	pop	edi
;	pop	esi
;	pop	ebx
;
;	leave
;	ret



;------------------------------------------------------------------------------------------------------
;_sample_perlin_3d( x, y, z, w, iterations )
;------------------------------------------------------------------------------------------------------
_sample_perlin_3d:
;------------------------------------------------------------------------------------------------------
	push	ebp			; preserve stack frame
	mov	ebp, esp		;
	sub	esp, 0x40		;
;------------------------------------------------------------------------------------------------------
	push	ebx			; preserve registers
	push	esi			;
	push	edi			;
;------------------------------------------------------------------------------------------------------
	
;------------------------------------------------------------------------------------------------------
	pop	edi			; restore registers
	pop	esi			;
	pop	ebx			;
;------------------------------------------------------------------------------------------------------
	mov	esp, ebp		; exit
	pop	ebp			;
	ret				;
;------------------------------------------------------------------------------------------------------


; int sample_3_a(source,x,y,z)
_sample3_a:
;	push	ebp		; preserve stack pointer
;	mov	ebp, esp	;
	;sub	esp, 0x40	; allocate stack space
	
	
	push	ebx
	push	esi
	push	edi

	
	mov	edx, [esp+16+4]	; y
	mov	edi, [esp+20+4]	; z
	mov	ecx, [esp+12+4]	; x
	
	
	shr	edx, 8		; edx=y>>8
	shr	edi, 8		; edi = z>>8
	movzx	esi, cl
	shr	ecx, 8		; ecx = x>>8
	shr	esi, 3		; esi = (x>>3)&31
	

	shl	edx, 5		; y *= yscale
	shl	edi, 7		; z *= zscale

	
	add	edx, edi	; y += z
	add	edx, ecx	; y += x

	mov	ebx, [_PLANET_SEED_SOURCE]
	and	edx, 0x3FFFF
	
	shl	edx, 5		; *= 32
	add	edx, esi

	
	mov	eax, [ebx+edx*4]
	

	movzx	ebx, al		; ebx = a
	movzx	ecx, ah		; ecx = b
	shr	eax, 16		; esi = c
	movzx	esi, al		; edi = d
	movzx	edi, ah


	movzx	eax, byte [esp+16+4]
	

	sub	ecx, ebx	; b-a
	sub	edi, esi	; d-c

	imul	ecx, eax
	imul	edi, eax

	movzx	eax, byte [esp+20+4]
	sar	ecx, 8
	sar	edi, 8
	add	ebx, ecx
	add	esi, edi

	sub	esi, ebx

	imul	esi, eax

	sar	esi, 8
	;add	ebx, esi

	
	lea	eax, [ebx+esi]
	;mov	eax, ebx
	

	pop	edi
	pop	esi
	pop	ebx

;	pop	ebp
;	leave
	ret

	
;int sample3_f( int ux, int y, int z ) {
;	ux>>=3;
;	int xi,yi,zi;
;	int xf;
;	yi = y>>8;
;	zi = z>>8;
;	xi = ux>>5;
;	xf = ux&31;
;	
;
;	u32 data = world->seedx[((xi+yi*SEED_Y_SCALE+zi*SEED_Z_SCALE)&SEED_MASK)*32+xf];
;	y &= 255;
;	z &= 255;
;	int a = data&255;
;	int b = (data>>8)&255;
;	int c = (data>>16)&255;
;	int d = (data>>24);
;	
;	
;	a += ((b-a) * y)>>8;
;	c += ((d-c) * y)>>8;
;	return a + (((c-a)*z)>>8);
;}