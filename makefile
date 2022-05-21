aal:
	gcc encode.c main.c -o stego
test:
	gcc encode.c main.c -o stego_test -DDBG
clean:
	rm stego && rm stego_test
