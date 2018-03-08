#!/bin/bash

xecho() {
  echo "$@" | tee -a /dev/stderr
  sync
}

sudo sysctl -w kernel.watchdog_thresh=1 # force more frequent cpu lock-up checking
./concurrent_uniq_test
./child_pids_test
./descendent_pids_test
