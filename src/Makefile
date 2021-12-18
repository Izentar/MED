all: prefixspan

SRCS = main.cpp PrefixSpan.cpp 
HEADERS_ONLY = utils.h
OUT = prefixspan.out

OBJS = $(SRCS:%.cpp=%.o)

prefixspan:
	g++ -std=c++17 -O3 -Wextra -Wall $(SRCS) $(HEADERS_ONLY) -o $(OUT)


clean:
	rm -f $(OUT) *.o