CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -pthread

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))
TARGET = $(BINDIR)/can_communication

all: $(TARGET)

$(TARGET): $(OBJFILES)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean

