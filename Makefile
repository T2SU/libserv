# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/24 12:21:35 by smun              #+#    #+#              #
#    Updated: 2022/03/24 16:14:46 by smun             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++-11
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm
RMFLAGS = -f

SRCS = main \
		channel \
		context \
		log \
		session
INC = -I./includes

FINAL_SRCS = $(addprefix srcs/, $(addsuffix .cpp, $(SRCS)))
FINAL_OBJS = $(FINAL_SRCS:.cpp=.o)

NAME = libserv.out

all: $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME): $(FINAL_OBJS)
ifneq ($(CC), c++)
	@echo "\033[33mWarning: \033[31mYour compiler is currently not c++. (current: "$(CC)")\033[0m"
endif
ifneq ($(CFLAGS), -Wall -Wextra -Werror -std=c++98)
	@echo "\033[33mWarning: \033[31mYour compiler flags is not adequate. (current: "$(CFLAGS)")\033[0m"
endif
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(RMFLAGS) $(FINAL_OBJS)

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)

re: clean all

.PHONY: all clean fclean re
