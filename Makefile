# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/06/15 21:58:26 by apelykh           #+#    #+#              #
#    Updated: 2018/06/16 17:55:44 by apelykh          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FLAGS = -Wall -Wextra -Werror
CC = gcc $(FLAGS)
NAME = RT

LIB_DIR = ./lib/
LIB = ./lib/cJSON/cJSON.a

INCLUDES = -I ./includes/
INCLUDES +=	-I ./lib/cJSON
INCLUDES += -I frameworks/SDL2.framework/Headers/

FRAMEWORKS = -framework OpenCL
FRAMEWORKS += -F frameworks
FRAMEWORKS += -framework SDL2

SRC_DIR = ./src/
SRC_FILES = main.c \
			vec_utils.c \
			matrix.c \
			cl_utils.c \
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
			parsing_utils_1.c \
			parsing_utils_2.c \
			bocal_components.c \
			debug_scene.c \

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