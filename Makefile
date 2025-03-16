build_dir?=build
src_dir?=src

src_dirs=$(sort $(dir $(wildcard ${src_dir}/**/)))
includes=$(patsubst %,-I%,${src_dirs})
CFLAGS+=-Wall -g -O0 ${includes}
LDFLAGS+=-fsanitize=address

srcs+=$(wildcard ${src_dir}/*.c ${src_dir}/**/*.c)
headers+=$(wildcard ${src_dir}/*.h ${src_dir}/**/*.h)
objs+=$(patsubst ${src_dir}/%.c,${build_dir}/%.o,${srcs})

.PHONY: all
all: build

.PHONY: build
build: controller

controller: ${objs}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

${build_dir}/%.o: ${src_dir}/%.c
	@mkdir -p $(dir $@)
	${CC} -c -o $@ $^ ${CFLAGS}

.PHONY: compile_flags.txt
compile_flags.txt:
	(echo "${CFLAGS} ${LDFLAGS}" | sed 's/ /\n/g') > compile_flags.txt

.PHONY: format
format:
	clang-format -i ${srcs} ${headers}

.PHONY: check_format
check_format:
	clang-format --dry-run --Werror ${srcs} ${headers}

.PHONY: clean
clean:
	${RM} -rf build controller
