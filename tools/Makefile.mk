filename:=$(shell pwd)
filedir=$(shell basename $(filename))

CROSS_DIR ?= /usr/bin
CROSS ?= arm-linux-gnueabihf-
PUSH_FILE?=dist
PUSH_DIR?=/root
HOST_IP?=192.168.12.1
MSSHF?=-o StrictHostKeychecking=no
SSH_PASSWORLD?=void
SSH_USER?=root

define foo2
    @echo "my name is $(0)"
    @echo "param => $(1)"
endef




target:
	python3 project.py build
	@if [ -f "./build_after.sh" ] ; then ./build_after.sh;fi;

release:
	python3 project.py build --build-type=Release
	@if [ -f "./build_after.sh" ] ; then ./build_after.sh;fi;

minisize:
	python3 project.py build --build-type=MinRelSize
	@if [ -f "./build_after.sh" ] ; then ./build_after.sh;fi;


verbose:
	python3 project.py build --verbose
	@if [ -f "./build_after.sh" ] ; then ./build_after.sh;fi;

menuconfig:
	python3 project.py menuconfig

run:
	cd dist && ./${filedir}

push_run:
	make push
	expect ../../tools/ssh.exp ${HOST_IP} ${SSH_USER} ${SSH_PASSWORLD} "${MSSHF}" "cd ${PUSH_DIR}/dist;./${filedir}"
	
clean:
	python3 project.py clean
distclean:
	make clean
	$(RM) -r build/ dist
	$(RM) -r .config.mk

arm:
	make set_arm
	make target

set_arm:
	python3 project.py --toolchain $(CROSS_DIR) --toolchain-prefix $(CROSS) config
	TMPFILE=`mktemp`;cat .config.mk $(DEFAULTS_UNITV3) > $${TMPFILE} ; awk '!a[$$0]++' $${TMPFILE} > .config.mk ; rm $${TMPFILE}
	
push:
	@expect ../../tools/push.exp ${HOST_IP} ${SSH_USER} ${SSH_PASSWORLD} "${MSSHF}" ${PUSH_FILE} ${PUSH_DIR}

shell:
	@python3 ../../tools/ssh.py ${HOST_IP} ${SSH_USER} ${SSH_PASSWORLD} "${MSSHF}"