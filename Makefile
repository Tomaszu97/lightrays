build:
	echo "> Building app"
	gcc -O2 src/main.c  -I include -I lib/cglm-0.7.9/include -o app -lm -lSDL2 -gdwarf-2
	echo "> App built"

run: build
	./app
