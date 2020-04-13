/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 15:05:43 by awoimbee          #+#    #+#             */
/*   Updated: 2020/04/13 15:44:33 by awoimbee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H

#include <inttypes.h>
#include <string.h>

extern uint8_t g_flag_woody[];

size_t find_self_size(uint8_t *self, size_t self_total_size);

#endif