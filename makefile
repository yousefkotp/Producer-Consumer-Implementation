target:
	g++ -c producer.cc
	g++ -o producer producer.o
	g++ -c consumer.cc
	g++ -o consumer consumer.o