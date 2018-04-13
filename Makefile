FLAGS = -Wall -Wextra -Werror
CC = gcc $(FLAGS)
NAME = RT
# LIB_DIR = libft/
# LIB = $(addprefix $(LIB_DIR), libft.a)
SRC_DIR = src/
SRC_FILES = main.c \
			vec_utils.c \
			scene1.c \
			matrix.c \
			cl_utils.c \
			sdl_utils.c \
			utils.c \

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(SRC:.c=.o)
INCLUDE_DIR = includes


.PHONY: all

all: $(NAME)
	
$(NAME): $(OBJ)
	@ $(CC) $(OBJ) -lOpenCL -lSDL2 -lm -o $@
	# @ $(CC) $(OBJ) -framework opencl -L/Users/mdubrovs/.brew/Cellar/sdl2/2.0.8/lib -lSDL2 -lm -o $@
	# @ $(CC) $(OBJ) -framework opencl -framework SDL2 -lm -o $@
	@ echo "[+] [$(NAME)] compiled"

.c.o: $(SRC)
	@ $(CC) -I includes -c $^ -o $@

clean:
	@ /bin/rm -f $(OBJ)
	@ echo "[+] [$(NAME)] .o files deleted"

fclean: clean
	@ /bin/rm -f $(NAME)
	@ echo "[+] [$(NAME)] binary file deleted"

re: fclean all