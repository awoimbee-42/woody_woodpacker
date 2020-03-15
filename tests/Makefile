# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/03/13 16:27:25 by awoimbee          #+#    #+#              #
#    Updated: 2020/03/15 17:24:04 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ASM_SRC = $(shell cd src && find . -type f -name "*.asm")
ASM_PROGS = $(addsuffix .exe,$(ASM_SRC))

C_SRC = $(shell cd src && find . -type f -name "*.c")
C_PROGS = $(addsuffix .exe,$(C_SRC))

PROGS = $(ASM_PROGS) $(C_PROGS)
STRIPPED_PROGS = $(addsuffix .stripped.exe,$(C_SRC)) $(addsuffix .stripped.exe,$(ASM_SRC))

test: all
	for f in $(PROGS); do ../woody_woodpacker $$f; echo ""; done;

all: $(ASM_PROGS) $(C_PROGS) $(STRIPPED_PROGS)

%.stripped.exe: %.exe
	cp $< $@
	strip $@

%.asm.o: src/%.asm
	nasm $< -f elf64 -o $@

%.asm.exe: %.asm.o
	ld -m elf_x86_64 $< -o $@
	cp $@ $@_stripped
	strip $@_stripped

%.c.exe: src/%.c
	gcc $< -o $@
	cp $@ $@_stripped
	strip $@_stripped

fclean:
	rm $(ASM_PROGS) $(C_PROGS)

re: fclean all
