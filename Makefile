# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/11/26 22:06:19 by marvin            #+#    #+#              #
#    Updated: 2020/04/14 10:27:07 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

################################################################################
###############                  VARIABLES                       ###############
################################################################################

NAME := woody_woodpacker

CFLAGS := -g3 -Wall -Wextra #-Werror -fno-builtin

SRC_NAME = ingester.c

WOODY_SRC_NAME = launcher.c self_size.c

SRC_SUBFOLDERS = $(shell cd src && find . -type d)

BUILD_FOLDER =	build

################################################################################
###############                  CONSTANTS                       ###############
################################################################################
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	NUMPROC := $(shell grep -c ^processor /proc/cpuinfo)
	EXEC_RPATH := \$$ORIGIN
else ifeq ($(UNAME_S),Darwin)
	NUMPROC := $(shell sysctl hw.ncpu | awk '{print $$2}')
	EXEC_RPATH := @loader_path #https://blogs.oracle.com/dipol/dynamic-libraries,-rpath,-and-mac-os
endif

CC = gcc
LDLIBS =
LDFLAGS =
CFLAGS += -MMD -I./src

SRC_FOLDERS =	$(SRC_SUBFOLDERS)

REPO_PATH = $(patsubst %/,%,$(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
BUILD_PATH = $(REPO_PATH)/$(BUILD_FOLDER)
SRC_PATH =	src
OBJ_FOLDER = $(BUILD_FOLDER)/obj

OBJ = $(addprefix $(OBJ_FOLDER)/, $(SRC_NAME:.c=.c.o))

OBJ_WOODY = $(addprefix $(OBJ_FOLDER)/, $(WOODY_SRC_NAME:.c=.c.o))

################################################################################
#################                  RULES                       #################
################################################################################
all :
	@$(MAKE) -j$(NUMPROC) $(NAME) --no-print-directory

$(OBJ_FOLDER)/woody: $(OBJ_WOODY)
	# @printf "$(GRN)Linking $@...$(EOC)\n"
	$(CC) $(CFLAGS) $(OBJ_WOODY) -o $@ $(LDFLAGS) $(LDLIBS)
	ld -r -b binary -o $@.o $@

$(NAME) : $(OBJ_FOLDER)/woody $(OBJ)
	@printf "$(GRN)Linking $@...$(EOC)\n"
	$(CC) $(CFLAGS) $(OBJ) $(OBJ_FOLDER)/woody.o -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_FOLDER) :
	@mkdir -p $(BUILD_PATH) 2> /dev/null | true
	@mkdir -p $(OBJ_FOLDER) 2> /dev/null | true
	@mkdir -p $(addprefix $(OBJ_FOLDER)/,$(SRC_FOLDERS)) 2> /dev/null | true

$(BUILD_FOLDER)/obj :
	mkdir -p $(BUILD_FOLDER) $(BUILD_FOLDER)/obj 2> /dev/null | true
	mkdir -p $(addprefix $(OBJ_FOLDER)/,$(SRC_FOLDERS)) 2> /dev/null | true

#objects
$(OBJ_FOLDER)/%.c.o : $(SRC_PATH)/%.c | $(BUILD_FOLDER)/obj
	@printf "\t$(CC) (...) $@\n"
	@$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_FOLDER)/%.cpp.o : $(SRC_PATH)/%.cpp | $(BUILD_FOLDER)/obj
	@printf "\t$(CXX) (...) $@\n"
	@$(CXX) $(CFLAGS) -o $@ -c $<

# Add rules written in .d files (by gcc -MMD)
# The '-' makes it doesn't care if the file exists or not
-include $(OBJ_NM:.o=.d) $(OBJ_OTOOL:.o=.d)

obj_clean :
	@printf "$(RED)Cleaning $(OBJ_FOLDER)$(EOC)\n"
	@rm -rf $(OBJ_FOLDER)

clean :
	@printf "$(RED)Cleaning $(BUILD_FOLDER)$(EOC)\n"
	@rm -rf $(BUILD_FOLDER)

fclean : clean
	@printf "$(RED)Cleaning $(NAME) & $(ASM)$(EOC)\n"
	@rm -f $(NAME)
	@rm -f $(ASM)

sclean	:	obj_clean
re		:	fclean		all
sre		:	obj_clean		all

.PHONY: all obj_clean clean fclean re sre

RED = \033[1;31m
GRN = \033[1;32m
YLW = \033[1;33m
INV = \033[7m
EOC = \033[0m
