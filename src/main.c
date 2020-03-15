/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/12 18:11:07 by awoimbee          #+#    #+#             */
/*   Updated: 2020/03/15 18:31:29 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <libft/ft_exit.h>
#include <libft/ft_str.h>
#include <libft/ft_mem.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MALLOC_HEADWAY 500

int		main(int argc, char *argv[])
{
	if (argc != 2 || !argv[1]) { printf("fuck you?\n"); return 1; }
	struct stat statbuf;
	ft_bzero(&statbuf, sizeof(statbuf));
	int fd = open(argv[1], O_RDONLY);
	ft_assert(syscall(SYS_fstat, fd, &statbuf) != -1, "Could not get file (%s) size: %s", argv[1], strerror(errno));
	size_t fsize = statbuf.st_size;
	void *content = NULL;
	ft_assertp_safe((content = malloc(fsize + MALLOC_HEADWAY)), "malloc fail", NULL);
	ft_assert(syscall(SYS_read, fd, content, fsize) != -1, "Could not read: %s", strerror(errno));
	// ft_assert(content != MAP_FAILED, "mmap of %s failed: %s", argv[1], strerror (errno));
	close(fd);

	Elf64_Ehdr *elf_head = content;
	ft_assert(ft_memcmp(ELFMAG, &elf_head->e_ident[0], SELFMAG) == 0, "Not an elf file: %s", argv[1]);

	Elf64_Section *sections = content + elf_head->e_shoff;

	printf("Sec num %d sec size %d\n", elf_head->e_shnum, elf_head->e_shentsize);
	void *vsec;
	for (
		vsec = sections;
		vsec < sections + elf_head->e_shnum * (elf_head->e_shentsize);
		vsec += elf_head->e_shentsize
		) {
		printf("SECTION\n");
		Elf64_Shdr *s = vsec;
		// for (uint8_t *d = s->sh_addr; d < s->sh_addr + s->sh_size; ++d)
		// *d += 1;
	}
	ft_assert(elf_head->e_shentsize <= MALLOC_HEADWAY, "Fuck, didnt code that in");
	// elf_head->e_shnum += 1;
	// Elf64_Shdr *s = vsec;

	//                                     |               |                                                                                   |              |
	char code[] = "\xBA\x0E\x00\x00\x00\xB9\xFF\xFF\xFF\xFF\xBB\x01\x00\x00\x00\xB8\x04\x00\x00\x00\xCD\x80\xB8\x01\x00\x00\x00\xCD\xFF\x25\xFF\xFF\xFF\xFFHello, world!\n";
	*(uint32_t*)&code[6] = (uint32_t)vsec + 34;
	*(uint32_t*)&code[30] = (uint32_t)elf_head->e_entry;
	elf_head->e_entry = 0x400000+ vsec ;

	// ft_memcpy(content + (uintptr_t)vsec, code, sizeof(code));

	int out = open("a.out", O_TRUNC|O_WRONLY|O_CREAT);
	ft_assert(out != -1, "PUTE0 %s", strerror(errno));
	ft_assert(write(out, content, fsize) != -1, "pd");
	write(out, code, sizeof(code));
	close(out);

	// munmap(content, fsize);
	return (0);
}
