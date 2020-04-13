/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   self_size.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 14:36:15 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/13 15:41:47 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <x86intrin.h>

#ifdef __AVX2__
static size_t len_avx2(const uint8_t *start, const uint8_t *end)
{
  const uint8_t *i = start;
  const uint64_t flag_woody_u64 = *(uint64_t*)g_flag_woody;

  __m256i q = _mm256_set1_epi8(g_flag_woody[0]);

  for (; i+32 < end; i+=32) {
    __m256i x  = _mm256_lddqu_si256((const __m256i*)(i));
    __m256i r  = _mm256_cmpeq_epi8(x, q);
    uint32_t z = _mm256_movemask_epi8(r);
    if (z) {
		const uint64_t *t = (const uint64_t*)(i + __builtin_ffs(z) - 1);
		if (*t == flag_woody_u64)
			return ((uintptr_t)t - (uintptr_t)start);
	}
  }

  for (; i < end - 63; ++i)
    if (*(uint64_t*)i == flag_woody_u64)
      return (i - start);

  return 0;
}
#endif

static size_t len_slow(const uint8_t *s, const uint8_t *src_end)
{
	uint8_t *src;

	src = (uint8_t *)s;
	while (src != src_end - 63)
	{
		if (*(uint64_t*)src == *(uint64_t*)g_flag_woody)
			return ((uintptr_t)src - (uintptr_t)s);
		++src;
	}
	return (0);
}

size_t find_self_size(uint8_t *self, size_t self_total_size)
{
	// TODO: check at runtime:
	#ifdef __AVX2__
		return len_avx2(self, self + self_total_size);
	#else
		return len_slow(self, self + self_total_size);
	#endif
}
