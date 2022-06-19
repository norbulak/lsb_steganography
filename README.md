# Stego

Implementation of a command line bitmap **steganography** tool through **LSB** encoding (rf. https://en.wikipedia.org/wiki/Steganography https://en.wikipedia.org/wiki/BMP_file_format). 
                                                         
## Install 
```
make
```
Binary is in `Release` directory
## usage
```
Usage:
stego: Encoding: stego -e <.bmp file> <secret file> [output file] # For encoding
stego: Decoding: stego -d <.bmp file> [output file] # For decoding
```



