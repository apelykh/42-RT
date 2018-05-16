FLAGS = -Wall -Wextra -Werror
CC = gcc $(FLAGS)
NAME = RT

LIB_DIR = ./lib/
LIB = ./lib/cJSON/cJSON.a
# LIB_NAMES = cJSON \

# LIB_PATHS = $(addprefix $(LIB_DIR), $(LIB_NAMES))
# LIB_NAMES_A = $(addsuffix .a, $(LIB_NAMES))
# LIB_FULL_NAMES = $(addprefix $(LIB_PATHS), $(LIB_NAMES_A))

INCLUDE_DIR = ./includes

SRC_DIR = ./src/
SRC_FILES = main.c \
			vec_utils.c \
			matrix.c \
			cl_utils.c \
			sdl_utils.c \
			utils.c \
			scene_parsing.c \
			objects_parsing.c \
			parsing_utils.c \
			scene1.c \

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(SRC:.c=.o)


.PHONY: all

all: $(NAME)

.c.o: $(SRC)
	@ $(CC) -I $(INCLUDE_DIR) -I ./lib/cJSON -c $^ -o $@

$(NAME): $(OBJ)
	@ make -C $(LIB_DIR)/cJSON

	@ $(CC) $(OBJ) -lOpenCL -lSDL2 -lm -o $@ ./lib/cJSON/cJSON.c
	# @ $(CC) $(OBJ) $(LIB) -framework opencl -L/Users/apelykh/.brew/Cellar/sdl2/2.0.8/lib -lSDL2 -lm -o $@
	# @ $(CC) $(OBJ) -framework opencl -framework SDL2 -lm -o $@
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