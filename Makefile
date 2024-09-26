# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/07 01:27:20 by ebelfkih          #+#    #+#              #
#    Updated: 2024/09/26 09:02:31 by ybouchra         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= ircserv

SRC				= main.cpp					\
				utils/helpers.cpp			\
				utils/errors.cpp			\
				utils/tools.cpp				\
				classes/Server.cpp			\
				classes/Client.cpp			\
				classes/Message.cpp			\
				classes/Channel.cpp			\
					
DIR_SRC		= src/
DIR_OBJ		= obj/
DIR_INC		= inc/
OBJ			= ${SRC:%.cpp=${DIR_OBJ}%.o}
CXX			= c++ 
DEP			= ${OBJ:%.o=%.d}
CPPFLAGS	= -Wall -Wextra -Werror -MMD -std=c++98 -c -I ${DIR_INC} 
RM 			= rm -f
RMDIR		= rm -rf

all: ${NAME}

${NAME} : ${OBJ}
	${CXX} -Wall -Wextra -Werror  $^ -o $@

${OBJ} : ${DIR_OBJ}%.o: ${DIR_SRC}%.cpp
	mkdir -p ${@D}
	${CXX} ${CPPFLAGS} $< -o $@

-include ${DEP}

clean:
	${RMDIR} ${DIR_OBJ}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re