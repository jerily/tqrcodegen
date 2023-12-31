# tqrcodegen

TCL/C extension for generating QR codes as SVG images

## Tcl Commands
```
# Generate an svg image of the QR code of the given text
# ::tqrcodegen::encode_to_svg text 
::tqrcodegen::encode_to_svg "Hello, world!"
```


## Build the library 
For Tcl
```
git clone https://github.com/jerily/tqrcodegen.git
cd tqrcodegen
mkdir build
cd build
cmake .. \
  -DTCL_LIBRARY_DIR=/usr/local/lib \
  -DTCL_INCLUDE_DIR=/usr/local/include
make
make install
```
For NaviServer (using Makefile)
```
git clone https://github.com/jerily/tqrcodegen.git
cd tqrcodegen
make
make install
```

## Example

![SVG image of the qr code of the text Hello World!](example.svg "SVG image of the qr code of the text Hello World!")
