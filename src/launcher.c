/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launcher.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/29 16:03:48 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/14 10:25:27 by awoimbee         ###   ########.fr       */
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

uint8_t g_flag_woody[] = "VOODPACK";

void __attribute__ ((noreturn)) error(const char *s)
{
	fprintf(stderr, "Woody error: %s\n", s);
	syscall(SYS_exit, EXIT_FAILURE);
	while (1);
}

int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	g_flag_woody[0] += 1;

	int self_fd = syscall(SYS_open, argv[0], O_RDONLY);
	if (self_fd == -1) {
		char *err; asprintf(&err, "Could not open self (%s)", argv[0]);
		error(err);
	}

	struct stat st;
	int ret = syscall(SYS_stat, argv[0], &st);
	if (ret == -1) {
		error("Could not query file size of self");
	}

	off_t self_total_size = st.st_size;


	char *dat = mmap(NULL, self_total_size, PROT_READ|PROT_EXEC, MAP_PRIVATE, self_fd, 0);
	if (dat == MAP_FAILED) {
		error("Could not mmap self");
	}

	uint64_t self_size = find_self_size((uint8_t*)dat, self_total_size);
	if (self_size == 0) {
		error("Couold not compute self size, no payload ?");
	}
	size_t inner_prog_size = self_total_size - self_size - 8;

	int tmp_file = syscall(SYS_memfd_create, "kittens", MFD_CLOEXEC);
	if (tmp_file == -1) {
		error("Could not create memfd");
	}

	printf("self_size: %lu\n", self_size);


	if (write(tmp_file, dat + self_size + 8, inner_prog_size) != (ssize_t)inner_prog_size) {
		error("Could not write to memfd");
	}

	pid_t pid = syscall(SYS_getpid);

	char *tmpf_path;
	asprintf(&tmpf_path, "/proc/%d/fd/%d", (int)pid, (int)tmp_file);

	printf("%s\n", tmpf_path);

	argv[0] = tmpf_path;
	printf("...WOODY...\n");
	ret = syscall(SYS_execve, tmpf_path, argv, envp);
	if (ret == -1) {
		error("execve failed");
	}

	return (0);
}
