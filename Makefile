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

CFLAGS 	:= -Wall -Werror -Wextra -g -std=c++17 -MMD -MP `pkg-config --cflags freetype2`

LIBRARIES := -Llibs/glfw-3.4/build/src -lglfw3 -Llibs/freetype-2.10.1/build/ -lfreetype  -lGL -lX11 -lpthread -lXrandr -lXi -ldl

.cpp.o:
		$(COMPILER) $(CFLAGS)  -c $< -o ${<:.cpp=.o}

all: 		nodebug ${NAME}

${NAME}:	${OBJS}
			${COMPILER} ${OBJS} -o ${NAME} ${LIBRARIES}

-include $(DEPENDS)

clean:
			${RM} ${OBJS} ${DEPENDS}

fclean: 	clean
			${RM} ${NAME}

re:
			make fclean
			make

install:
			sh ./scripts/build.sh
debug:
			sh ./scripts/useDebugMode.sh true
			make ${NAME}

nodebug:
			sh ./scripts/useDebugMode.sh false

.PHONY: 	all clean fclean re installcdebug nodebug
