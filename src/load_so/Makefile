all: dylibs libs bins

dylibs:
	@for subdir in $(shell ls -d */); \
		do\
			make -C $$subdir dylibs;\
		done

bins:
	@for subdir in $(shell ls -d */); \
		do\
			make -C $$subdir bins;\
		done
libs:
	@for subdir in  $(shell ls -d */); \
		do\
			make -C $$subdir libs;\
		done

clean:
	@for subdir in  $(shell ls -d */); \
		do\
			make -C $$subdir clean;\
		done
