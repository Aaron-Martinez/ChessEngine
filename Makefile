all:
	g++ src/*.cpp -o Hoodwink


pins:
	g++ src/*.cpp -o PinTest


clean:
	rm ./Hoodwink