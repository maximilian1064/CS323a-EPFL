EXECUTABLES = concurrent_uniq_test child_pids_test descendent_pids_test

.PHONY: build
build:	$(EXECUTABLES)

.PHONY: run
run: build
	chmod +x ./_checker.sh
	./_checker.sh

concurrent_uniq_test: concurrent_uniq_test.c syscalls.h
	gcc -fopenmp concurrent_uniq_test.c -o concurrent_uniq_test

child_pids_test: child_pids_test.c syscalls.h
	gcc child_pids_test.c -o child_pids_test

descendent_pids_test: descendent_pids_test.c syscalls.h
	gcc descendent_pids_test.c -o descendent_pids_test

.PHONY: clean
clean:
	-rm $(EXECUTABLES)
