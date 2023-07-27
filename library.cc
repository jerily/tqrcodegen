#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "qrcodegen.hpp"
#include "library.h"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

#ifdef DEBUG
# define DBG(x) x
#else
# define DBG(x)
#endif

#define CheckArgs(min,max,n,msg) \
                 if ((objc < min) || (objc >max)) { \
                     Tcl_WrongNumArgs(interp, n, objv, msg); \
                     return TCL_ERROR; \
                 }

static int           tqrcodegen_ModuleInitialized;

// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
static std::string toSvgString(const QrCode &qr, int border) {
        if (border < 0)
                throw std::domain_error("Border must be non-negative");
        if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
                throw std::overflow_error("Border too large");

        std::ostringstream sb;
        sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
        sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
        sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
        sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
        sb << "\t<path d=\"";
        for (int y = 0; y < qr.getSize(); y++) {
                for (int x = 0; x < qr.getSize(); x++) {
                        if (qr.getModule(x, y)) {
                                if (x != 0 || y != 0)
                                        sb << " ";
                                sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
                        }
                }
        }
        sb << "\" fill=\"#000000\"/>\n";
        sb << "</svg>\n";
        return sb.str();
}

static int tqrcodegen_EncodeToSvg(ClientData  clientData, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] ) {
    DBG(fprintf(stderr,"GetEncodeToSvg\n"));

    CheckArgs(2,2,1,"text");
    const QrCode::Ecc errCorLvl = QrCode::Ecc::HIGH;  // Error correction level
    const QrCode qr = QrCode::encodeText(Tcl_GetString(objv[1]), errCorLvl);
    std::string svg = toSvgString(qr, 4);
    Tcl_SetObjResult(interp, Tcl_NewStringObj(svg.c_str(), svg.length()));
    return TCL_OK;
}


static void tqrcodegen_ExitHandler(ClientData unused) {
}


void tqrcodegen_InitModule() {
    if (!tqrcodegen_ModuleInitialized) {
        Tcl_CreateThreadExitHandler(tqrcodegen_ExitHandler, NULL);
        tqrcodegen_ModuleInitialized = 1;
    }
}

int Tqrcodegen_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, "8.6", 0) == NULL) {
        return TCL_ERROR;
    }

    tqrcodegen_InitModule();

    Tcl_CreateNamespace(interp, "::tqrcodegen", NULL, NULL);
    Tcl_CreateObjCommand(interp, "::tqrcodegen::encode_to_svg", tqrcodegen_EncodeToSvg, NULL, NULL);

    return Tcl_PkgProvide(interp, "tqrcodegen", "0.1");
}

#ifdef USE_NAVISERVER
int Ns_ModuleInit(const char *server, const char *module) {
    Ns_TclRegisterTrace(server, (Ns_TclTraceProc *) Tqrcodegen_Init, server, NS_TCL_TRACE_CREATE);
    return NS_OK;
}
#endif
