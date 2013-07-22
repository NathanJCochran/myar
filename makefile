CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall 
PROGS=myar

all: $(PROGS)

myar: myar.o arch_itr.o arch_util.o
	$(CC) $(CFLAGS) -o $@ myar.o arch_itr.o arch_util.o

myar.o: myar.c
	$(CC) $(CFLAGS) -c $^

arch_itr.o: arch_itr.c
	$(CC) $(CFLAGS) -c $^

arch_util.o: arch_util.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(PROGS) *.o *~

tests: testq testt testv

cleantests:
	rm -f ar12345.ar myar12345.ar ar135.ar myar135.ar ar24.ar myar24.ar ar-ctoc.txt myar.ctoc.txt ar-vtoc.txt myar-vtoc.txt

testq: testq12345 testq135 testq24

testq12345:
	rm -f ar12345.ar myar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	myar -q myar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	diff ar12345.ar myar12345.ar
	@echo

testq135:
	rm -f ar135.ar myar135.ar
	ar q ar135.ar 1-s.txt 3-s.txt 5-s.txt
	myar -q myar135.ar 1-s.txt 3-s.txt 5-s.txt
	diff ar135.ar myar135.ar
	@echo

testq24:
	rm -f ar24.ar myar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	myar -q myar24.ar 2-s.txt 4-s.txt
	diff ar24.ar myar24.ar
	@echo

testt: testt12345 testt135 testt24

testt12345:
	rm -f ar12345.ar myar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar t ar12345.ar > ar-ctoc.txt
	myar -t ar12345.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
	@echo

testt135:
	rm -f ar135.ar myar135.ar
	ar q ar135.ar 1-s.txt 3-s.txt 5-s.txt
	ar t ar135.ar > ar-ctoc.txt
	myar -t ar135.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
	@echo

testt24:
	rm -f ar24.ar myar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	ar t ar24.ar > ar-ctoc.txt
	myar -t ar24.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
	@echo

testv: testv12345 testv135 testv24

testv12345:
	rm -f ar12345.ar myar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar tv ar12345.ar > ar-vtoc.txt
	myar -v ar12345.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
	@echo

testv135:
	rm -f ar135.ar myar135.ar
	ar q ar135.ar 1-s.txt 3-s.txt 5-s.txt
	ar tv ar135.ar > ar-vtoc.txt
	myar -v ar135.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
	@echo

testv24:
	rm -f ar24.ar myar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	ar tv ar24.ar > ar-vtoc.txt
	myar -v ar24.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
	@echo

