CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin
LIB_DIR := $(CUR_DIR)/lib
LIB_STATIC_DIR := $(LIB_DIR)/static
LIB_SHARED_DIR := $(LIB_DIR)/shared

create_obj:
	gcc -c ${SRC_DIR}/main.c -o ${OBJ_DIR}/main.o -I $(INC_DIR)
	gcc -c -fPIC ${SRC_DIR}/myMath.c -o ${OBJ_DIR}/myMath.o

create_static:
	ar rcs ${LIB_STATIC_DIR}/libmymath.a ${OBJ_DIR}/myMath.o

link_static:
	gcc    ${OBJ_DIR}/main.o -L${LIB_STATIC_DIR} -lmymath -o ${BIN_DIR}/statically-linked

create_shared:
	gcc -shared ${OBJ_DIR}/myMath.o -o ${LIB_SHARED_DIR}/libmymath.so

linking:
	gcc  ${OBJ_DIR}/main.o -L${LIB_SHARED_DIR} -lmymath -o ${BIN_DIR}/use-shared-library

static: create_obj create_static link_static

shared: create_obj create_shared linking

clean:
	rm -rf ${BIN_DIR}/* ${LIB_SHARED_DIR}/* ${OBJ_DIR}/*