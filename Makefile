CC 			= g++-4.9 -std=c++11
CXXFLAG 	= -O2 -Wall -Wextra -W -ggdb3 -I. -pthread -lX11 -lm 
SRC = $(wildcard *.cpp)
SRC_OBJ = test.o

test: $(SRC_OBJ)
	$(CC) $^ -o $@ $(CXXFLAG)

%.o: %.cpp
	$(CC) $< -o $@ $(CXXFLAG) -c
	
clean:
	rm -f test *.o

rebuild:	clean test

.PHONY		:	clean rebuild
