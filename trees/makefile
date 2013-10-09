CC= g++
COMPILERFLAGS = -m32 -Wall -O9 -DNDEBUG
CFLAGS=  $(COMPILERFLAGS)
SRCDIR=src
OBJDIR=obj

##############################
#  Objetos  (.o)             #
##############################

OBJS_rank_select =  static_bitsequence.o table_offset.o static_bitsequence_rrr02.o  static_bitsequence_brw32.o 

OBJS_BP = balanced_parentheses.o bp_rp.o hash.o bp_hb.o darray.o bp.o bpcore.o bp_rmm.o

OBJ_DF = dfuds.o ff_dfuds.o

OBJS = $(OBJS_rank_select) $(OBJS_BP) $(OBJ_DF) tree_rp.o tree_ff.o tree_hb.o tree_louds.o tree_dfuds.o tree_ff_dfuds.o 

##############################
# make all                   #
##############################

all: rank_select Ba_Pa  DFUDS Trees TESTS

Ba_Pa: darray bp bpcore bp_rmm hash bp_hb bp_rp balanced_parentheses

DFUDS: dfuds ff_dfuds

Trees: tree_rp tree_ff tree_hb tree_louds tree_dfuds tree_ff_dfuds

rank_select:  table_offset static_bitsequence_brw32 static_bitsequence_rrr02 static_bitsequence

TESTS: 
###############################
# crear objetos y ejecutables #
###############################

static_bitsequence:
	$(CC) $(CFLAGS) -c -o static_bitsequence.o static_bitsequence.cpp

table_offset:
	  $(CC) $(CFLAGS) -c -o table_offset.o table_offset.cpp

static_bitsequence_rrr02:
	$(CC) $(CFLAGS) -c -o static_bitsequence_rrr02.o static_bitsequence_rrr02.cpp
	
static_bitsequence_brw32:
	$(CC) $(CFLAGS) -c -o static_bitsequence_brw32.o static_bitsequence_brw32.cpp

darray:
	  $(CC) $(CFLAGS) -c -o darray.o darray.cpp

bp:
	  $(CC) $(CFLAGS) -c -o bp.o bp.cpp

bpcore:
	    $(CC) $(CFLAGS) -c -o bpcore.o bpcore.cpp

bp_rmm:
	$(CC) $(CFLAGS) -c -o bp_rmm.o bp_rmm.cpp

hash:
	$(CC) $(CFLAGS) -c -o hash.o hash.cpp

dfuds:
	$(CC) $(CFLAGS) -c -o dfuds.o dfuds.cpp

ff_dfuds:
	  $(CC) $(CFLAGS) -c -o ff_dfuds.o ff_dfuds.cpp

bp_hb:
	  $(CC) $(CFLAGS) -c -o bp_hb.o bp_hb.cpp

bp_rp:
	$(CC) $(CFLAGS) -c -o bp_rp.o bp_rp.cpp

balanced_parentheses:
	$(CC) $(CFLAGS) -c -o balanced_parentheses.o balanced_parentheses.cpp

tree_rp:
	  $(CC) $(CFLAGS) -c -o tree_rp.o tree_rp.cpp

tree_ff:
	    $(CC) $(CFLAGS) -c -o tree_ff.o tree_ff.cpp

tree_hb:
	      $(CC) $(CFLAGS) -c -o tree_hb.o tree_hb.cpp

tree_louds:
	$(CC) $(CFLAGS) -c -o tree_louds.o tree_louds.cpp

tree_dfuds:
	  $(CC) $(CFLAGS) -c -o tree_dfuds.o tree_dfuds.cpp

tree_ff_dfuds:
	$(CC) $(CFLAGS) -c -o tree_ff_dfuds.o tree_ff_dfuds.cpp

################################
# TESTS                        #
################################

clean:
	rm -f *.o *.cpp~ *.h~ makefile~ 

