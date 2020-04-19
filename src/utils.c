/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/19 12:19:36 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/19 22:25:12 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdint.h>

char *itoa_u32_buf(unsigned int i, char *buf)
{
	register char *ptr = buf;
	if (!i) ++ptr;
	for (register unsigned int k = i; k; k /= 10, ++ptr);
	*ptr = '\0';
	char *end = ptr;
	while (ptr != buf) {
		*(--ptr) = i % 10 + '0';
		i /= 10;
	}
	return end;
}

unsigned int strlen_small(const char *s)
{
	unsigned int i = 0;
	while (s[i])
		++i;
	return i;
}

void __attribute__((noreturn)) err_exit(const char *context)
{
	const char bckspce = '\n';
	int err = errno;
	syscall(SYS_write, 2, "Error: ", 7);
	if (context) {
		syscall(SYS_write, 2, context, strlen_small(context));\
		syscall(SYS_write, 2, &bckspce, 1);
	}
    if (err < sys_nerr) {
		const char *err_str = sys_errlist[err];
		syscall(SYS_write, 2, err_str, strlen_small(err_str));
	}
    else {
		syscall(SYS_write, 2, "Unknown error", 14);
	}
	syscall(SYS_write, 2, &bckspce, 1);
	syscall(SYS_exit, EXIT_FAILURE);
	__builtin_unreachable();
}
