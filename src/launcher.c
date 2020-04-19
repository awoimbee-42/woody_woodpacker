/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launcher.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 16:03:48 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/20 00:21:24 by awoimbee         ###   ########.fr       */
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
#include <errno.h>

uint8_t g_flag_woody[] = "VOODPACK";

int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	g_flag_woody[0] += 1;

	int self_fd = syscall(SYS_open, argv[0], O_RDONLY);
	if (self_fd == -1)
		err_exit("Could not open self");

	struct stat st;
	int ret = syscall(SYS_stat, argv[0], &st);
	if (ret == -1)
		err_exit("Could not query file size of self");

	off_t self_total_size = st.st_size;

	self_total_size += 16 - (self_total_size % 16);
	char *dat = (char*)syscall(SYS_mmap, NULL, self_total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, self_fd, 0);
	if (dat == MAP_FAILED)
		err_exit("Could not mmap self");

	uint64_t self_size = find_self_size((uint8_t *)dat, self_total_size);
	if (self_size == 0)
		err_exit("Could not compute self size, no payload ?");

	size_t inner_prog_size = self_total_size - self_size - 8;

	int tmp_file = syscall(SYS_memfd_create, "", 0);
	if (tmp_file == -1) {
		err_exit("Could not create memfd");
	}

	int8_t enc_key[]    = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	aes128_load_key(enc_key);
	aes128_dec(dat + self_size + 8, inner_prog_size);

	if (syscall(SYS_write, tmp_file, dat + self_size + 8, inner_prog_size) != (ssize_t)inner_prog_size) {
		err_exit("Could not write to memfd");
	}

	pid_t pid = syscall(SYS_getpid);

	char tmpf_path[6 + 11 + 4 + 11];
	{
		ft_memcpy(tmpf_path, "/proc/", 6);
		char *t = itoa_u32_buf(pid, &tmpf_path[6]);
		ft_memcpy(t, "/fd/", 4);
		itoa_u32_buf(tmp_file, &t[4]);
	}
	argv[0] = tmpf_path;
	syscall(SYS_write, 1, "....WOODY....\n", 14);

	ret = syscall(SYS_execve, tmpf_path, argv, envp);
	if (ret == -1) {
		err_exit("execve failed");
	}

	return (0);
}
