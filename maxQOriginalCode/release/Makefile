#
#  Makefile
#      ILIB is the directory where the include files will be found
#

# State Type: This must be set manually for the different domains.
# You should also delete all .o files and do a clean make.
STATEFLAG = -DDELTATAXISTATE


ILIB    =./lib 

CC     = g++
CFLAGS = -g -O2 -I$(ILIB)  $(STATEFLAG)

LIBS =  -lm
RM     = rm -fr
GETOPT = getopt.o
OSTYPE = WINDOWS98

#
# programs
#

buddlib:
	(cd lib; make all)

fq-agent: taxi.o state.o atom.o flat-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o state.o atom.o flat-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o fq-agent

fq-fuel-agent: taxi.o fueltaxi.o state.o atom.o flat-fuel-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-fuel-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o fueltaxi.o state.o atom.o flat-fuel-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-fuel-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o fq-fuel-agent

fq-delta-agent: taxi.o delta-taxi.o state.o atom.o flat-delta-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-delta-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o delta-taxi.o state.o atom.o flat-delta-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-delta-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o fq-delta-agent

hq-agent: taxi.o state.o atom.o h-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o state.o atom.o h-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o hq-agent

hq-fuel-agent: taxi.o fueltaxi.o state.o atom.o h-fuel-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-fuel-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o fueltaxi.o state.o atom.o h-fuel-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-fuel-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o hq-fuel-agent

hq-delta-agent: taxi.o delta-taxi.o state.o atom.o h-delta-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-delta-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT)
	$(CC) $(CFLAGS) taxi.o delta-taxi.o state.o atom.o h-delta-maxq.o hq.o hps.o psweep.o lib/clstring.o parameter.o predicate.o hq-features.o snet.o net.o hq-delta-agent.o lib/clrandom.o hq-parameters.o point.o ma.o $(GETOPT) $(LIBS) -o hq-delta-agent

ma-file: ma-file.o ma.o 
	$(CC) $(CFLAGS) $(LIBS) ma-file.o ma.o -o ma-file

#
# modules
#

atom.o: atom.c atom.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c atom.c

delta-taxi.o: delta-taxi.c delta-taxi.h taxi.h atom.h state.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c delta-taxi.c

flat-maxq.o: flat-maxq.c predicate.h taxi.h parameter.h hq-features.h hq.h ma.h global.h point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c flat-maxq.c

flat-fuel-maxq.o: flat-fuel-maxq.c predicate.h taxi.h fueltaxi.h parameter.h hq-features.h hq.h ma.h global.h point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c flat-fuel-maxq.c

fueltaxi.o: fueltaxi.h fueltaxi.c taxi.h state.h atom.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c fueltaxi.c

getopt.o: getopt.c 
	$(CC) -c getopt.c

h-maxq.o: h-maxq.c predicate.h taxi.h atom.h state.h parameter.h hq-features.h hq.h  ma.h global.h point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c h-maxq.c

h-fuel-maxq.o: h-fuel-maxq.c predicate.h taxi.h fueltaxi.h parameter.h hq-features.h hq.h  ma.h global.h point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c h-fuel-maxq.c

h-delta-maxq.o: h-delta-maxq.c predicate.h taxi.h delta-taxi.h parameter.h hq-features.h hq.h  ma.h global.h point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c h-delta-maxq.c

hps.o: hps.c psweep.h global.h state.h get-state.h hq.h parameter.h hq-features.h hq-parameters.h lib/list.h lib/table.h lib/clrandom.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c hps.c -o hps.o

hq.o: global.h point.h taxi-maxq.h hq.h  ma.h parameter.h hq-features.h snet.h hq-parameters.h  hq.c point.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c hq.c

hq-agent.o: taxi.h hq.h  ma.h parameter.h hq-parameters.h taxi-maxq.h hq-agent.c point.h 
	$(CC) $(CFLAGS) -D$(OSTYPE) -DTAXISTATE -c hq-agent.c -o hq-agent.o

hq-fuel-agent.o: taxi.h fueltaxi.h hq.h  ma.h parameter.h hq-parameters.h taxi-maxq.h hq-agent.c point.h 
	$(CC) $(CFLAGS) -D$(OSTYPE) -DFUELTAXISTATE -c hq-agent.c -o hq-fuel-agent.o

hq-delta-agent.o: taxi.h delta-taxi.h hq.h  ma.h parameter.h hq-parameters.h taxi-maxq.h hq-agent.c point.h 
	$(CC) $(CFLAGS) -D$(OSTYPE) -DDELTATAXISTATE -c hq-agent.c -o hq-delta-agent.o

hq-features.o: hq-features.h hq-features.c
	$(CC) $(CFLAGS) -D$(OSTYPE) -c hq-features.c

hq-parameters.o: hq-parameters.h hq-parameters.c 
	$(CC) $(CFLAGS) -D$(OSTYPE) -c hq-parameters.c

ma.o: lib/vector.h ma.c ma.h
	$(CC) $(CFLAGS) -c ma.c

ma-file.o: lib/vector.h ma-file.c ma.h
	$(CC) $(CFLAGS) -c ma-file.c

net.o: net.h net.c
	$(CC) $(CFLAGS) -D$(OSTYPE) -c net.c

parameter.o: parameter.h parameter.c 
	$(CC) $(CFLAGS) -D$(OSTYPE) -c parameter.c

point.o: point.h point.c
	$(CC) $(CFLAGS) -D$(OSTYPE) -c point.c

predicate.o: predicate.h predicate.c state.h get-state.h parameter.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c predicate.c

psweep.o: psweep.h hq.h get-state.h parameter.h predicate.h snet.h hq-parameters.h ma.h psweep.c
	$(CC) $(CFLAGS) -D$(OSTYPE) -c psweep.c -o psweep.o

snet.o: snet.h net.h snet.c
	$(CC) $(CFLAGS) -D$(OSTYPE) -c snet.c

state.o: state.h state.c atom.h parameter.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c state.c

taxi.o: taxi.h taxi.c global.h point.h state.h atom.h
	$(CC) $(CFLAGS) -D$(OSTYPE) -c taxi.c

clean:
	$(RM) *.o ptrepository/*
         

