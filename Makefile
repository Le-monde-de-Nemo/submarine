build_dir?=build
src_dir?=src
tst_dir?=tst

# Define here the list of files having main functions in src directory
src_mains+=${src_dir}/main.c

# Define here the list of files having main functions in tst directory
tst_mains+=${tst_dir}/vectors.c
tst_mains+=${tst_dir}/fish.c
tst_mains+=${tst_dir}/vue.c
tst_mains+=${tst_dir}/aqua.c
tst_mains+=${tst_dir}/mobility.c

# You shouldn't need to edit what's next
build_tst_targets=$(patsubst ${tst_dir}/%.c,${build_dir}/%,${tst_mains})

src_dirs=$(sort $(dir $(wildcard ${src_dir}/**/)))
includes=$(patsubst %,-I%,${src_dirs})
CFLAGS+=-Wall -g -O0 ${includes}
LDFLAGS+=-fsanitize=address

tsts+=$(wildcard ${tst}/*.c ${src_dir}/**/*.c)
_tsts=$(filter-out ${tst_mains},${tsts})

srcs+=$(wildcard ${src_dir}/*.c ${src_dir}/**/*.c)
_srcs=$(filter-out ${src_mains},${srcs})

src_objs+=$(patsubst %.c,${build_dir}/%.o,${_srcs})
tst_objs+=$(patsubst %.c,${build_dir}/%.o,${_tsts})

src_headers+=$(wildcard ${src_dir}/*.h ${src_dir}/**/*.h)
tst_headers+=$(wildcard ${tst_dir}/*.h ${tst_dir}/**/*.h)

.PHONY: all
all: build

.PHONY: build
build: controller

controller: ${src_objs} ${src_dir}/main.c
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

${build_dir}/%.o: %.c
	@mkdir -p $(dir $@)
	${CC} -c -o $@ $^ ${CFLAGS}

.PHONY: test
test: build_test
	($(patsubst %,% &,${build_tst_targets}) wait) # | sort

.PHONY:
build_test: ${build_tst_targets}

${build_tst_targets}: ${build_dir}/%: ${tst_dir}/%.c ${src_objs} ${tst_objs}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

.PHONY: compile_flags.txt
compile_flags.txt:
	(echo "${CFLAGS} ${LDFLAGS}" | sed 's/ /\n/g') > compile_flags.txt

.PHONY: format
format:
	clang-format -i ${_srcs} ${_tsts} ${tst_mains} ${src_mains} ${src_headers} ${tst_headers}

.PHONY: check_format
check_format:
	@clang-format --dry-run --Werror ${_srcs} ${_tsts} ${tst_mains} ${src_mains} ${src_headers} ${tst_headers}

.PHONY: clean
clean:
	${RM} -rf build controller
