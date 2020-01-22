# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/06/15 21:58:26 by apelykh           #+#    #+#              #
#    Updated: 2018/06/17 18:12:47 by apelykh          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = RT
FLAGS = -Wall -Wextra -Werror
CC = gcc $(FLAGS)
LIB_DIR = ./lib/
LIB = ./lib/cJSON/cJSON.a
INCLUDES = -I ./includes/ -I ./lib/cJSON

OS = $(shell uname)
ifeq ($(OS), Linux)
	INCLUDES += -I /usr/include/SDL2
	FRAMEWORKS = -lOpenCL -lSDL2
else ifeq ($(OS), Darwin)
	FRAMEWORKS = -framework OpenCL -F frameworks -rpath frameworks/ -framework SDL2
else
	@ echo "[-] Only Linux and MacOS are supported"
endif


SRC_DIR = ./src/
SRC_FILES = main.c \
			vector_init.c \
			vector_utils.c \
			matrix_transform.c \
			matrix_ops.c \
			matrix_ops2.c \
			cl_utils.c \
			cl_utils2.c \
			sdl_utils.c \
			utils.c \
			controls.c \
			controls_press.c \
			controls_release.c \
			parsing_scene.c \
			parsing_camera.c \
			parsing_lights.c \
			parsing_objects.c \
			values_init.c \
			parsing_utils.c \
			parsing_utils2.c \
			bocal_components.c \
			save_image.c \

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(SRC:.c=.o)


.PHONY: all

all: $(NAME)

.c.o: $(SRC)
	@ $(CC) $(INCLUDES) -c $^ -o $@

$(NAME): $(OBJ)
	@ make -C $(LIB_DIR)/cJSON
	@ $(CC) $(OBJ) $(LIB) $(FRAMEWORKS) -lm -o $@
	@ echo "[+] [$(NAME)] compiled"

clean:
	@ make -C $(LIB_DIR)/cJSON clean
	@ /bin/rm -f $(OBJ)
	@ echo "[+] [$(NAME)] .o files deleted"

fclean: clean
	@ make -C $(LIB_DIR)/cJSON fclean
	@ /bin/rm -f $(NAME)
	@ echo "[+] [$(NAME)] binary files deleted"

re: fclean all