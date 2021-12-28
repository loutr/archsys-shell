TMPFILES:= lex.c parse.c
MODULES	:= main parse output
OBJECTS := $(MODULES:=.o)
CC 	:= gcc -g -Wall
LINK 	:= $(CC)
LIBS 	:= -lreadline

shell: $(OBJECTS)
	$(LINK) $^ -o $@ $(LIBS)


parse.o: parse.c lex.c

parse.c: parse.y global.h
	bison parse.y -o $@

lex.c: lex.l
	flex -o$@ lex.l


.PHONY: clean

clean: 
	@rm -f shell $(OBJECTS) $(TMPFILES)
