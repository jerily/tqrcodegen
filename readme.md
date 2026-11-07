# tqrcodegen

Generate QR codes as SVG from Tcl 8.6 or 9.x using
[Nayuki's QR encoder](https://github.com/nayuki/QR-Code-generator).

```tcl
package require tqrcodegen 1.1
puts [::tqrcodegen::encode_to_svg "Hello, world!"]
```

`encode_to_svg text` uses high error correction and a four-module quiet zone.
It returns an error when the text exceeds QR capacity.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/path/to/install \
  -DTCL_INCLUDE_DIR=/path/to/tcl/include \
  -DTCL_LIBRARY_DIR=/path/to/tcl/lib
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

Shared builds link only the Tcl stub archive. Build separately against Tcl 8.6
or Tcl 9 headers and stubs for the corresponding runtime. `TCL_INCLUDE_PATH`,
`TCL_STUB_LIBRARY`, and `TCL_TCLSH` can also select explicit paths.

Use `-DBUILD_SHARED_LIBS=OFF` for a static library. The embedding application
links Tcl and calls `Tqrcodegen_Init`; the header installs under
`include/tqrcodegen`. Static builds do not install a loadable package index.
Use `-DBUILD_TESTING=OFF` when the target Tcl runtime is unavailable during builds.

For NaviServer, the existing `make NAVISERVER=/path/to/naviserver` build remains
available.

![QR code example](example.svg)
