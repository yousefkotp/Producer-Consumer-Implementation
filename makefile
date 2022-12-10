target:
	g++ -c producer.cc
	g++ -o producer producer.o
	g++ -c main.cc
	g++ -o main main.o
	g++ -c semaphore.cc
	g++ -o semaphore semaphore.o