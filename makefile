ProcSpace : ProcSpace.c vector.o utils.o generate.o update.o render.o
	cc \
    vector.o utils.o generate.o update.o render.o \
	-lSDL2 -lm \
	ProcSpace.c \
	-o ProcSpace 

vector.o : vector.c 
	cc vector.c -c
utils.o : utils.c
	cc utils.c -c
generate.o : generate.c
	cc generate.c -c
update.o : update.c
	cc update.c -c
render.o : render.c
	cc render.c -c

clean : 
	rm vector.o utils.o generate.o update.o render.o
