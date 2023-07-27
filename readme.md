# tqrcodegen

TCL/C extension for generating QR codes as SVG images

## Tcl Commands
```
# Generate an svg image of the QR code of the given text
# ::tqrcodegen::encode_to_svg text 
::tqrcodegen::encode_to_svg "Hello, world!"
```


## Build the library 
```
git clone https://github.com/jerily/tqrcodegen.git
cd tqrcodegen
mkdir build
cd build
cmake ..
make
make install
```