/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ingester.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 14:34:54 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/20 00:21:41 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#define _GNU_SOURCE

#include "woody.h"

#include <inttypes.h>

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

extern const unsigned char _binary_build_obj_woody_start;
extern const unsigned char _binary_build_obj_woody_end;

int main(int argc, char *argv[])
{
	if (
		argc != 2
	|| (argv[1][0] == '-' && argv[1][1] == 'h' && argv[1][2] == '\0')
	) {
		const char *usage = "usage: [PROGRAM]";
		syscall(SYS_write, 1, usage, 7);
		syscall(SYS_write, 1, argv[0], strlen_small(argv[0]));
		syscall(SYS_write, 1, &usage[7], 9);
		errno = EINVAL;
		err_exit(NULL);
	}

	int in_fd = syscall(SYS_open, argv[1], O_RDONLY);
	if (in_fd == -1) {
		err_exit("Could not open requested executable");
	}

	struct stat st;
	if (syscall(SYS_fstat, in_fd, &st) == -1) {
		err_exit("Could not stat requested executable");
	}
	ssize_t in_size = st.st_size;

	in_size += 16 - (in_size % 16);
	char *in_dat = (char*)syscall(SYS_mmap, NULL, in_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, in_fd, 0);

	// char *in_dat = syscall(SYS_mmap, NULL, in_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	// if (read(in_fd, in_dat, in_size) != in_size) {
	// 	err_exit("Could not read requested file");
	// }

	if (in_dat == MAP_FAILED) {
		err_exit("Failed to mmap");
	}

	int out_fd = syscall(SYS_open, "./woody", O_RDWR | O_TRUNC | O_CREAT, 0777);
	if (out_fd == -1) {
		err_exit("Could not open ./woody");
	}

	ssize_t s = &_binary_build_obj_woody_end - &_binary_build_obj_woody_start;

	if (syscall(SYS_write, out_fd, &_binary_build_obj_woody_start, s) != s) {
		err_exit("Could not write launcher binary blob");
	}
	if (syscall(SYS_write, out_fd, "WOODPACK", 8) != 8) {
		err_exit("Could not write magic str");
	}
	int8_t enc_key[]    = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	aes128_load_key(enc_key);
	aes128_enc(in_dat, in_size);

	if (syscall(SYS_write, out_fd, in_dat, in_size) != in_size) {
		err_exit("Could not write wrapped binary");
	}

	syscall(SYS_close, in_fd);
	syscall(SYS_close, out_fd);
	return (EXIT_SUCCESS);
}
