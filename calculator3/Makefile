# James' Universal Makefile for simple C programs
# Version 1.0
# by James Swineson, 2014-11-15

SRC = main.c
PROGRAM = ./calc
OBJ = $(SRC:.c=.o)
CC = gcc 
CFLAGS = -O -Wall -Wno-misleading-indentation
LINKPARAM = -lm

: $(PROGRAM)

# 链接
$(PROGRAM): $(OBJ)
	@echo "> Linking $(OBJ)..."
	@$(CC) -o $(PROGRAM) $(LINKPARAM) $(OBJ)

# 编译
%.o: %.c
	@echo "> Compiling $<..."
	@$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

# 运行
exec: $(PROGRAM)
	@echo "> Executing $(PROGRAM)"
	@echo "> =================================================="
	@$(EXEC_PREFIX) $(PROGRAM) $(ARG)
	@echo ""
	@echo "> =================================================="
	@echo "> Program finished."

# 清理所有编译器产生的文件
clean-all:
	@echo "> Cleaning all binary files..."
	@echo "> List of files to be deleted:"
	@echo "> $(OBJ) $(PROGRAM)"
	@rm $(RM_FLAGS) $(OBJ) $(PROGRAM)

# 清理所有中间文件
clean:
	@echo "> Cleaning all object files (*.o)..."
	@echo "> List of files to be deleted:"
	@echo "> $(OBJ)"
	@rm $(RM_FLAGS) $(OBJ)

# 懒人操作
debug: clean-all $(PROGRAM) exec
