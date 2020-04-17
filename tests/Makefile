# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/03/13 16:27:25 by awoimbee          #+#    #+#              #
#    Updated: 2020/04/17 18:12:57 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ASM_SRC = $(shell cd src && find . -type f -name "*.asm")
ASM_PROGS = $(addsuffix .exe,$(ASM_SRC))

C_SRC = $(shell cd src && find . -type f -name "*.c")
C_PROGS = $(addsuffix .exe, $(C_SRC))

PROGS = $(ASM_PROGS) $(C_PROGS)
STRIPPED_PROGS = $(PROGS:.exe=.stripped.exe)

test: all


	@for f in $(PROGS); \
	do \
		echo "Testing: $$f"; \
		printf "....WOODY....\n" > out; \
		./$$f >> out; \
		../woody_woodpacker $$f; \
		./woody > wout; \
		diff out wout; \
		echo ""; \
	done
	@rm woody

../woody_woodpacker:
	$(MAKE) -C ../

all: $(ASM_PROGS) $(C_PROGS) $(STRIPPED_PROGS) ../woody_woodpacker test

%.stripped.exe: %.exe
	cp $< $@
	strip $@

%.asm.o: src/%.asm
	nasm $< -f elf64 -o $@

%.asm.exe: %.asm.o
	ld -m elf_x86_64 $< -o $@

%.c.exe: src/%.c
	gcc $< -o $@

fclean:
	rm $(ASM_PROGS) $(C_PROGS)

re: fclean all
