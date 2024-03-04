SRCS := srcs/main.cpp \
		libs/glad/src/glad.cpp \
		srcs/classes/Shader/Shader.cpp \
		srcs/classes/Utils/Utils.cpp \

OBJS := $(SRCS:.cpp=.o)

NAME := ft_vox

COMPILER ?= c++

RM		:= rm -f

CFLAGS 	:= -Wall -Werror -Wextra -g -std=c++17

LIBRARIES := -Llibs/glfw-3.4/build/src -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

.cpp.o:
			${COMPILER} ${CFLAGS} -c $< -o ${<:.cpp=.o}

all: 		${NAME}

${NAME}:	${OBJS}
			${COMPILER} ${OBJS} -o ${NAME} ${LIBRARIES}

clean:
			${RM} ${OBJS}

fclean: 	clean
			${RM} ${NAME}

re:
			make fclean
			make

.PHONY: 	all clean fclean re
