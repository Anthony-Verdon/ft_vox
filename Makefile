SRCS := srcs/main.cpp \
		srcs/App.cpp \
		srcs/classes/VkInstance/VkInstanceWrapper.cpp \
		srcs/classes/VkDebugMessenger/VkDebugMessengerWrapper.cpp \
		srcs/classes/VkPhysicalDevice/VkPhysicalDeviceWrapper.cpp \
		srcs/classes/VkLogicalDevice/VkLogicalDeviceWrapper.cpp

OBJS := $(SRCS:.cpp=.o)

NAME := ft_vox

COMPILER := c++

RM		:= rm -f

CFLAGS 	:= -Wall -Werror -Wextra -g -std=c++17

ifdef DEBUG
	CFLAGS += -D DEBUG
endif

LIBRARIES := -Lincludes/glfw/build/src -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

.cpp.o:
			${COMPILER} ${CFLAGS} -c $< -o ${<:.cpp=.o}

all: 		${NAME}

${NAME}:	${OBJS}
			${COMPILER} ${OBJS} -o ${NAME} ${LIBRARIES}

debug:		
			make DEBUG=1

re_debug:		
			make re DEBUG=1

clean:
			${RM} ${OBJS}

fclean: 	clean
			${RM} ${NAME}

re:
			make fclean
			make

.PHONY: 	all clean fclean re debug
