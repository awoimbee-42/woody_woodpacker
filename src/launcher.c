/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launcher.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 16:03:48 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/17 22:37:21 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include "woody.h"

#include <inttypes.h>

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#include <elf.h>
#include <stdbool.h>

#define PAGESZ 0x1000
typedef unsigned long ul;
#define ALIGN_DOWN(x) ((x) & -PAGESZ)
#define ALIGN_UP(x) ((x) + PAGESZ - 1 & -PAGESZ)
#define INLINE static inline

#ifdef DEBUG
#define DP(fmt, ...) fprintf(stderr, "%s:%d:%s " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define DP(...)
#endif

#if __WORDSIZE == 64
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_auxv_t Elf64_auxv_t
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_auxv_t Elf32_auxv_t
#endif

// should not intersect with normal mapping addresses
#ifdef __i386
#define LDSO_BASE 0x20000
#elif defined(__x86_64)
#define LDSO_BASE 0x6666666000l
#elif defined(__arm__)
#define LDSO_BASE 0x200000
#elif defined(__mips__)
#define LDSO_BASE 0x200000
#else
#error "this arch is not supported"
#endif

INLINE void *map_file(const char *file)
{
	int fd = open(file, O_RDONLY);
	off_t len = lseek(fd, 0, SEEK_END);
	void *ret = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
	DP("ret %p", ret);
	close(fd);
	return ret;
}

static void *load_elf(void *elf, Elf_Ehdr **elf_ehdr, Elf_Ehdr **interp_ehdr)
{
	DP("entry");
	Elf_Ehdr *ehdr = (Elf_Ehdr *)elf;
	bool is_pic = ehdr->e_type == ET_DYN;
	int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS | (is_pic ? 0 : MAP_FIXED);
	Elf_Phdr *phdr = (Elf_Phdr *)((char *)ehdr + ehdr->e_phoff), *interp = NULL;
	ulong entry = ehdr->e_entry, first_addr = 0, first_specified, brk_addr = 0;
	if (interp_ehdr)
		*interp_ehdr = NULL;

	for (int i = 0; i < ehdr->e_phnum; ++i)
	{
		DP("program header %d", i);
		switch (phdr->p_type)
		{
		case PT_INTERP:
			interp = phdr;
			break;
		case PT_LOAD:
		{
			ulong specified;
			if (!is_pic)
				specified = ALIGN_DOWN(phdr->p_vaddr);
			else if (first_addr)
			{
				specified = ALIGN_DOWN(phdr->p_vaddr - first_specified + first_addr);
				mmap_flags |= MAP_FIXED;
			}
			else
			{
				specified = 0;
				// TODO hack
				entry += specified = LDSO_BASE;
				mmap_flags |= MAP_FIXED;
			}
			ulong rounded_len = ALIGN_UP(phdr->p_vaddr % PAGESZ + phdr->p_memsz);
			DP("mmap(%#x, %#x, ...)", specified, rounded_len);
			ulong addr = (ulong)mmap((void *)specified, rounded_len, PROT_WRITE | PROT_EXEC, mmap_flags, -1, 0);
			DP("mmap = %#x", addr);
			ft_memcpy((void *)(addr + phdr->p_vaddr % PAGESZ), (const void *)ehdr + phdr->p_offset, phdr->p_filesz);
			mprotect((void *)addr, rounded_len, (phdr->p_flags & PF_R ? PROT_READ : 0) | (phdr->p_flags & PF_W ? PROT_WRITE : 0) | (phdr->p_flags & PF_X ? PROT_EXEC : 0));

			if (!first_addr)
			{
				if (elf_ehdr)
					*elf_ehdr = (Elf_Ehdr *)addr;
				first_addr = addr;
				if (is_pic)
				{
					first_specified = phdr->p_vaddr;
					entry += addr - specified;
				}
			}
			ulong t = phdr->p_vaddr % PAGESZ + phdr->p_memsz;
			if (t > brk_addr)
				brk_addr = t;
		}
		}
		phdr++;
	}
	if (interp)
		entry = (long)load_elf(map_file((const char *)ehdr + interp->p_offset), interp_ehdr, NULL);
	if (!is_pic)
		brk((void *)ALIGN_UP(brk_addr));
	return (void *)entry;
}

void myexec(void *elf, int *argc)
{
	Elf_Ehdr *elf_ehdr, *interp_ehdr;
	void *entry = load_elf(elf, &elf_ehdr, &interp_ehdr);

	// argc[0]

	volatile long stack[999], *p = stack;
	*p++ = 1;				 // argc
	volatile long *argv = p; // argv[]
	*p++ = (long)stack;
	*p++ = 0;
	volatile long *envp = p; // envp[]
	// //*p++ = "LD_SHOW_AUXV=1";
	*p++ = 0;
	volatile long *auxv = p; // auxv[]
	// //*p++ = AT_SYSINFO_EHDR; // AT_SYSINFO_EHDR for x86-64 arm; AT_SYSINFO for x86
	// //*p++ = -1;
	*p++ = AT_PHDR;
	*p++ = (long)((char *)elf_ehdr + elf_ehdr->e_phoff);
	*p++ = AT_PHNUM;
	*p++ = elf_ehdr->e_phnum;
	*p++ = AT_BASE;
	*p++ = (long)interp_ehdr;
	*p++ = AT_ENTRY;
	*p++ = elf_ehdr->e_entry;
	*p++ = AT_RANDOM;
	*p++ = (long)stack;
	*p++ = AT_NULL;
	*p++ = 0;

#ifdef __i386
	asm("movl %0, %%esp\n\t"
		"jmp *%1\n\t" ::"g"(stack),
		"r"(entry));
#elif defined(__x86_64)
	asm("movq %0, %%rsp\n\t"
		"jmp *%1\n\t" ::"g"(stack),
		"r"(entry), "d"(0));
#elif defined(__arm__)
	asm("mov sp, %0\n\t"
		"bx %1\n\t" ::"r"(stack),
		"r"(entry));
#elif defined(__mips__)
	asm("lw $sp, %0\n\t"
		"j %1\n\t" ::"m"(stack),
		"r"(entry));
#endif
}

uint8_t g_flag_woody[] = "VOODPACK";

void __attribute__((noreturn)) error(const char *s)
{
	fprintf(stderr, "Woody error: %s\n", s);
	syscall(SYS_exit, EXIT_FAILURE);
	while (1)
		;
}

int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	g_flag_woody[0] += 1;

	int self_fd = syscall(SYS_open, argv[0], O_RDONLY);
	if (self_fd == -1)
	{
		char *err;
		asprintf(&err, "Could not open self (%s)", argv[0]);
		error(err);
	}

	struct stat st;
	int ret = syscall(SYS_stat, argv[0], &st);
	if (ret == -1)
	{
		error("Could not query file size of self");
	}

	off_t self_total_size = st.st_size;

	char *dat = mmap(NULL, self_total_size, PROT_READ | PROT_EXEC, MAP_PRIVATE, self_fd, 0);
	if (dat == MAP_FAILED)
	{
		error("Could not mmap self");
	}

	uint64_t self_size = find_self_size((uint8_t *)dat, self_total_size);
	if (self_size == 0)
	{
		error("Could not compute self size, no payload ?");
	}
	size_t inner_prog_size = self_total_size - self_size - 8;

	////////////////////////////////////////////////////////////////////////////
	myexec(dat + self_size + 8, &argc);

	// int tmp_file = syscall(SYS_memfd_create, "kittens", MFD_CLOEXEC);
	// if (tmp_file == -1) {
	// 	error("Could not create memfd");
	// }

	// printf("self_size: %lu\n", self_size);

	// if (write(tmp_file, dat + self_size + 8, inner_prog_size) != (ssize_t)inner_prog_size) {
	// 	error("Could not write to memfd");
	// }

	// pid_t pid = syscall(SYS_getpid);

	// char *tmpf_path;
	// asprintf(&tmpf_path, "/proc/%d/fd/%d", (int)pid, (int)tmp_file);

	// printf("%s\n", tmpf_path);

	// argv[0] = tmpf_path;
	// printf("....WOODY....\n");
	// ret = syscall(SYS_execve, tmpf_path, argv, envp);
	// if (ret == -1) {
	// 	error("execve failed");
	// }

	return (0);
}
