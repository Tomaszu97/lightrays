.PHONY: build
build:
	echo "> Building lightrays (speed optimized)"
	gcc -Ofast src/main.c  -I src -I lib/cglm-0.7.9/include -o lightrays -lm -lSDL2 -gdwarf-2
	echo "> App built (speed optimized)"

.PHONY: install
install:
	echo "> Installing lightrays"
	cp -r models /usr/share/lightrays_models
	cp lightrays /usr/bin/lightrays
	echo "> Installed lightrays"

.PHONY: uninstall
uninstall:
	echo "> Uninstalling lightrays"
	rm -rf /usr/share/lightrays_models
	rm -f /usr/bin/lightrays
	echo "> Uninstalled lightrays"

.PHONY: run
run: build
	./lightrays

.PHONY: clean
clean:
	rm lightrays
