target:
	g++ -c producer.cpp
	g++ -o producer producer.o
	g++ -c consumer.cpp
	g++ -o consumer consumer.o