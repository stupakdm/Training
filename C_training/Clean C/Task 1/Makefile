all:
	gcc -ffast-math -O3 -o avx matrix_avx.c -march=haswell
	gcc -ffast-math -O3 -o sse matrix_sse.c -march=haswell
	gcc -ffast-math -O3 -o clean matrix_clean.c -march=haswell
clean:
	rm avx
	rm sse
	rm clean
