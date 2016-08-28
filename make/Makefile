#Makefile

export PRJ_PATH=$(shell readlink -f ..)
export SRC_PATH=${PRJ_PATH}/src
export SRC_SUB_PATH=$(shell ls ${SRC_PATH})
export SRC_SUB_FULL_PATH=$(addprefix ${SRC_PATH}/, ${SRC_SUB_PATH})
export BUILD_PATH=${PRJ_PATH}/build
export OBJ_PATH=${BUILD_PATH}/objs
export LIB_PATH=${BUILD_PATH}/libs
export BIN_PATH=${BUILD_PATH}/bins

all: libs bins

dirs:
	@mkdir -p ${LIB_PATH}
	@mkdir -p ${BIN_PATH}

libs: dirs
	@echo "----"
	@for subdir in ${SRC_SUB_FULL_PATH}; \
	do\
		#echo $${subdir}\
		make -C $${subdir} libs;\
	done

bins: dirs
	@echo "----"
	@for subdir in ${SRC_SUB_FULL_PATH}; \
	do\
		#echo $${subdir}\
		make -C $${subdir} bins;\
	done

echo_vars:
	@echo SRC_PATH=${SRC_PATH}
	@echo SRC_SUB_PATH=${SRC_SUB_PATH}

${SRC_SUB_PATH}: dirs
	@echo  ++++++++++ \"$@\" begin +++++++++++
	@make -C  ${SRC_PATH}/$@
	@echo  ++++++++++ \"$@\" finished +++++++++++
