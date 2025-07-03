CC = gcc
CFLAGS = -Wall -g -Iincludes
TARGET = analisador

SDIR = src
ODIR = obj
VPATH = $(SDIR)

SOURCES_C = $(notdir $(wildcard $(SDIR)/*.c) main.c)
SOURCES_C := $(filter-out codegen.c vm.c, $(SOURCES_C))

OBJECTS = $(addprefix $(ODIR)/, $(SOURCES_C:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "A ligar o analisador..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Analisador construído: ./${TARGET}"

$(ODIR)/%.o: %.c
	@mkdir -p $(ODIR)
	@echo "A compilar $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "A limpar..."
	rm -f $(TARGET)
	rm -rf $(ODIR)

.PHONY: all clean
