/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ingester.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 14:34:54 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/13 18:46:34 by awoimbee         ###   ########.fr       */
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


extern const unsigned char _binary_build_obj_woody_start[];
extern const unsigned char _binary_build_obj_woody_end[];
// extern const size_t _binary_build_obj_woody_size;

int main(int argc, char *argv[])
{
	if (
		argc != 2
	|| (argv[1][0] == '-' && argv[1][1] == 'h' && argv[1][2] == '\0')
	) {
		printf("usage: %s [PROGRAM]\n", argv[0]);
		fprintf(stderr, "Error: bad arguments\n");
		return 1;
	}

	int in_fd = syscall(SYS_open, argv[1], O_RDONLY);
	if (in_fd == -1) {
		fprintf(stderr, "Error: Could not open requested executable\n");
		return 1;
	}

	struct stat st;
	if (fstat(in_fd, &st) == -1) {
		fprintf(stderr, "Error: Could not stat requested executable\n");
		return 1;
	}
	ssize_t in_size = st.st_size;

	char *in_dat = mmap(NULL, in_size, PROT_READ, MAP_SHARED, in_fd, 0);

	// char *in_dat = mmap(NULL, in_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	// if (read(in_fd, in_dat, in_size) != in_size) {
	// 	fprintf(stderr, "Error: Could not read requested file\n");
	// 	return -1;
	// }

	if (in_dat == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap\n");
		return 1;
	}

	int out_fd = syscall(SYS_open, "./woody", O_RDWR | O_TRUNC | O_CREAT, 0777);
	if (out_fd == -1) {
		fprintf(stderr, "Error: Could not open ./woody\n");
		return 1;
	}

	ssize_t s = &_binary_build_obj_woody_end[0] - &_binary_build_obj_woody_start[0];

	if (write(out_fd, _binary_build_obj_woody_start, s) != s) {
		fprintf(stderr, "Could not write launcher binary blob (%lu bytes)\n", s);
		return 1;
	}
	if (write(out_fd, "WOODPACK", 8) != 8) {
		fprintf(stderr, "Could not write magic str\n");
		return 1;
	}
	if (write(out_fd, in_dat, in_size) != in_size) {
		fprintf(stderr, "Could not write wrapped binary (%p, %ld) %s\n", in_dat, in_size, strerror(errno));
		return 1;
	}

	printf(
		"wrote the binary blob (%lu), \"WOODPACK\" & the input prog (%lu)\n",
		(size_t)s, in_size
	);

	close(in_fd);
	close(out_fd);
	return (1);
}
