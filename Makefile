SRCS := srcs/main.cpp \
		libs/glad/src/glad.cpp \
		srcs/classes/Shader/Shader.cpp \
		srcs/classes/Utils/Utils.cpp \
		srcs/classes/Camera/Camera.cpp \
		srcs/classes/Texture/Texture.cpp \
		srcs/classes/Time/Time.cpp \
		srcs/classes/WindowManager/WindowManager.cpp \
		srcs/classes/BlockClasses/BlockData/BlockData.cpp \
		srcs/classes/BlockClasses/BlockMesh/BlockMesh.cpp \
		srcs/classes/ChunkClasses/ChunkData/ChunkData.cpp \
		srcs/classes/ChunkClasses/ChunkMesh/ChunkMesh.cpp \
		srcs/classes/ChunkClasses/ChunkRenderer/ChunkRenderer.cpp \
		srcs/classes/WorldClasses/WorldData/WorldData.cpp \
		srcs/classes/WorldClasses/WorldUpdater/WorldUpdater.cpp \

OBJS := $(SRCS:.cpp=.o)
DEPENDS := $(SRCS:.cpp=.d)

NAME := ft_vox

COMPILER ?= c++

RM		:= rm -f

CFLAGS 	:= -Wall -Werror -Wextra -g -std=c++17 -MMD -MP

ifdef DEBUG_MODE
	CFLAGS += -DDEBUG_MODE=true
endif

LIBRARIES := -Llibs/glfw-3.4/build/src -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

.cpp.o:
		$(COMPILER) $(CFLAGS)  -c $< -o ${<:.cpp=.o}

all: 		${NAME}

${NAME}:	${OBJS}
			${COMPILER} ${OBJS} -o ${NAME} ${LIBRARIES}

-include $(DEPENDS)

clean:
			${RM} ${OBJS} ${DEPENDS}

debug:
			make DEBUG_MODE=1

fclean: 	clean
			${RM} ${NAME}

re:
			make fclean
			make

.PHONY: 	all clean fclean re
