.PHONY: build
build:
	echo "> Building app"
	gcc -O0 src/main.c  -I src -I lib/cglm-0.7.9/include -o app -lm -lSDL2 -gdwarf-2
	echo "> App built"

.PHONY: optbuild
optbuild:
	echo "> Building app (speed optimized)"
	gcc -Ofast src/main.c  -I src -I lib/cglm-0.7.9/include -o app -lm -lSDL2 -gdwarf-2
	echo "> App built (speed optimized)"

.PHONY: run
run: build
	./app

.PHONY: optrun
optrun: optbuild
	./app
