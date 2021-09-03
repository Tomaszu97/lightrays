.PHONY: build
build:
	echo "> Building app"
	gcc src/materials.c src/main.c src/objects.c src/rays.c src/scene.c  -I include -I lib/cglm-0.7.9/include -o app -Wall -lm -lSDL2 -gdwarf-2 -O0
	echo "> App built"

.PHONY: optbuild
optbuild:
	echo "> Building app (speed optimized)"
	gcc src/materials.c src/main.c src/objects.c src/rays.c src/scene.c  -I include -I lib/cglm-0.7.9/include -o app -Wall -lm -lSDL2 -gdwarf-2 -Os
	echo "> App built (speed optimized)"

.PHONY: run
run: build
	./app

.PHONY: optrun
optrun: optbuild
	./app
