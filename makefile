weatherChecker: main.c utils.c display.c libs/cJSON/cJSON.c
	gcc main.c utils.c display.c libs/cJSON/cJSON.c -o weatherChecker
install:
	cp weatherChecker /usr/bin
clean:
	@rm *.o main
