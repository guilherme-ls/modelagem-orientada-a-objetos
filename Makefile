

all:
	g++ -o main *.cpp Controller/*.cpp View/*.cpp Model/*.cpp -I ./Model -I ./View -I ./Controller -g -lraylib -lGL -lm -lpthread -pthread -ldl -lrt -lX11 -pthread
