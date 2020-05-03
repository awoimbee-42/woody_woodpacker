#ifndef WOODPACKER_H
#define WOODPACKER_H

#include <elf.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#define KEY_MAXLEN 16
#define FILENAME "woody"

#define ARR_LEN(v) (sizeof(v)/sizeof(v[0]))

void	handle_elf64(void *mmap_ptr, size_t original_filesize);

void	fatal_error(char *s) __attribute__((noreturn));

void 	rc4(unsigned char *key,int key_len,char *buff,int len);
void	*ft_memmove(void *dst, const void *src, size_t len);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memset(void *b, int c, size_t n);
int	ft_strcmp(const char *s1, const char *s2);

extern unsigned char		key[KEY_MAXLEN];

#endif
