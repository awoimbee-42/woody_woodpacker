# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/03/13 16:27:25 by awoimbee          #+#    #+#              #
#    Updated: 2020/05/03 19:50:04 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ASM_SRC = $(shell cd src && find . -type f -name "*.asm")
ASM_PROGS = $(addsuffix .exe,$(ASM_SRC))

C_SRC = $(shell cd src && find . -type f -name "*.c")
C_PROGS = $(addsuffix .exe, $(C_SRC))

PROGS = $(ASM_PROGS) $(C_PROGS)
STRIPPED_PROGS = $(PROGS:.exe=.stripped.exe)

all: $(PROGS) $(STRIPPED_PROGS)

tests: all ../woody_woodpacker
	@for f in $(PROGS); \
	do                  \
		echo "Testing: $$f";                \
		printf "....WOODY....\n" > tmp_std; \
		./$$f >> tmp_std;                   \
		../woody_woodpacker $$f;            \
		./woody > tmp_wrapped;              \
		diff tmp_std tmp_wrapped;           \
		echo "";                            \
	done
	@rm woody

../woody_woodpacker:
	$(MAKE) -C ../

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
	rm $(PROGS) $(STRIPPED_PROGS)

re: fclean all
