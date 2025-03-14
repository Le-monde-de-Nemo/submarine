build_dir?=build
src_dir?=src

src_dirs=$(sort $(dir $(wildcard ${src_dir}/**/)))
includes=$(patsubst %,-I%,${src_dirs})
CFLAGS+=-g -O0 -std=c99 ${includes}
LDFLAGS+=

srcs+=$(wildcard ${src_dir}/*.c ${src_dir}/**/*.c)
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

test:
	# ${src_dir}
	# ${srcs}
	# ${objs}
	# ${include}

clean:
	${RM} -rf build controller
