/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 15:05:43 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/20 00:25:32 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H

#include <inttypes.h>
#include <string.h>

extern uint8_t g_flag_woody[];

size_t find_self_size(uint8_t *self, size_t self_total_size);

void *ft_memcpy(void *dst, const void *src, size_t n);

void __attribute__((noreturn)) err_exit(const char *context);
unsigned int strlen_small(const char *s);
char *itoa_u32_buf(unsigned int i, char *buf);

void aes128_load_key(int8_t *enc_key);
void aes128_enc(void *data, size_t len);
void aes128_dec(void *data, size_t len);

#endif
