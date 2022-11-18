.PHONY: build

CFLAGS= -I include -I lib/cglm-0.7.9/include -o app -Werror -lm -lSDL2
CC= gcc

build:
	echo "> Building app"
	$(CC) src/*.c $(CFLAGS) -gdwarf-2 -O0
	echo "> App built"

.PHONY: optbuild
optbuild:
	echo "> Building app (speed optimized)"
	$(CC) src/*.c $(CFLAGS) -O3
	echo "> App built (speed optimized)"

.PHONY: run
run: build
	./app < scene-desc

.PHONY: optrun
optrun: optbuild
	./app < scene-desc

.PHONY: clean
clean:
	rm -f app
