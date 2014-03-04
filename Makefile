DIRS = emu
all:
	for dir in $(DIRS); do (cd $$dir; make $1 || exit 1) || exit 1; done
clean:
	for dir in $(DIRS); do (cd $$dir; make clean $1 || exit 1) || exit 1; done
