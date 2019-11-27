#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = imageFormation

all : $(PROG)

# Variables for file compilation
CC        =  gcc -m64
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

#pxmtopxm : pxmtopxm.o Util.o

imageFormation : imageFormation.o imageFormationUtils.o
imageFormationUtils.o : imageFormationUtils.c imageFormationUtils.h

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)
