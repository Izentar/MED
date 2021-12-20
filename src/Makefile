all: prefixspan

SRCS = main.cpp PrefixSpan.cpp utils.cpp
OUT = prefixspan.out

OBJS = $(SRCS:%.cpp=%.o)

prefixspan:
	g++ -std=c++17 -O3 -Wextra -Wall -pthread -lpthread $(SRCS) -o $(OUT)

debug:
	g++ -std=c++17 -O3 -Wextra -Wall -g -pthread -lpthread $(SRCS) -o $(OUT)


clean:
	rm -f $(OUT) *.o