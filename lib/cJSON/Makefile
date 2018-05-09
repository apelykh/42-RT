FLAGS	= -Wall -Wextra -Werror
CC		= gcc $(FLAGS)
NAME	= cJSON.a
SRC		= cJSON.c
OBJ		= $(SRC:.c=.o)

.PHONY: all

all: $(NAME)

%.o:%.c
	$(CC) -c $^ -o $@

$(NAME): $(OBJ)
	ar rc $(NAME) $(OBJ)
	ranlib $(NAME)

clean:
	@ /bin/rm -f $(OBJ)

fclean: clean
	@ /bin/rm -f $(NAME)

re: fclean all
