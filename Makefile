CC = clang++
FLAGS = -O0

all: addition_benchmark subtraction_benchmark multiplication_benchmark axpy_benchmark fused_multiply_add_benchmark matrix_multiplication_benchmark

addition_benchmark: addition_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

subtraction_benchmark: subtraction_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

multiplication_benchmark: multiplication_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

axpy_benchmark: axpy_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

fused_multiply_add_benchmark: fused_multiply_add_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

matrix_multiplication_benchmark: matrix_multiplication_benchmark.cpp
	${CC} ${FLAGS} $^ -o $@.out;

clean:
	rm -rf *.out;