# Apple: gcc -o hello hello.c -framework opencl

FLAGS = -Wall -Wextra
CC = gcc $(FLAGS)
NAME = rt
# LIB_DIR = libft/
# LIB = $(addprefix $(LIB_DIR), libft.a)
SRC_DIR = src/
SRC_FILES = main.c \

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(SRC:.c=.o)
INCLUDE_DIR = includes


.PHONY: all

all: $(NAME)
	
$(NAME): $(OBJ)
	# @ $(CC) $(OBJ) -lOpenCL -lSDL2 -lSDL2_image -lm -o $@
	@ $(CC) $(OBJ) -framework Opencl -lm -o $@
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