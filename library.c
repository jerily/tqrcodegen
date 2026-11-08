#include "library.h"
#include "qrcodegen.h"
#include <stdio.h>

#define XSTR(s) STR(s)
#define STR(s) #s
#ifndef PROJECT_VERSION
#define PROJECT_VERSION 1.1.1
#endif

#if TCL_MAJOR_VERSION < 9
#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
#endif
#define MIN_TCL_VERSION "8.6"
#else
#define MIN_TCL_VERSION "9.0"
#endif

static Tcl_Obj *toSvgString(const uint8_t qrcode[]) {
    const int border = 4;
    int size = qrcodegen_getSize(qrcode);
    char buffer[128];
    Tcl_Obj *svg = Tcl_NewObj();
    Tcl_AppendStringsToObj(svg,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",
        "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n",
        "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ", NULL);
    snprintf(buffer, sizeof(buffer), "viewBox=\"0 0 %d %d\" stroke=\"none\">\n",
        size + border * 2, size + border * 2);
    Tcl_AppendToObj(svg, buffer, -1);
    Tcl_AppendStringsToObj(svg,
        "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n",
        "\t<path d=\"", NULL);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcodegen_getModule(qrcode, x, y)) {
                snprintf(buffer, sizeof(buffer), "M%d,%dh1v1h-1z ", x + border, y + border);
                Tcl_AppendToObj(svg, buffer, -1);
            }
        }
    }
    Tcl_AppendStringsToObj(svg, "\" fill=\"#000000\"/>\n</svg>\n", NULL);
    return svg;
}

static int tqrcodegen_EncodeToSvg(ClientData clientData, Tcl_Interp *interp,
        Tcl_Size objc, Tcl_Obj *const objv[]) {
    (void)clientData;
    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "text");
        return TCL_ERROR;
    }
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    if (!qrcodegen_encodeText(Tcl_GetString(objv[1]), tempBuffer, qrcode,
            qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
            qrcodegen_Mask_AUTO, true)) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("text is too long for a QR code", -1));
        Tcl_SetErrorCode(interp, "TQRCODEGEN", "CAPACITY", NULL);
        return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, toSvgString(qrcode));
    return TCL_OK;
}

DLLEXPORT int Tqrcodegen_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, MIN_TCL_VERSION, 0) == NULL) {
        return TCL_ERROR;
    }
    if (Tcl_FindNamespace(interp, "::tqrcodegen", NULL, TCL_GLOBAL_ONLY) == NULL
            && Tcl_CreateNamespace(interp, "::tqrcodegen", NULL, NULL) == NULL) {
        return TCL_ERROR;
    }
    Tcl_CreateObjCommand(interp, "::tqrcodegen::encode_to_svg", tqrcodegen_EncodeToSvg, NULL, NULL);
    return Tcl_PkgProvide(interp, "tqrcodegen", XSTR(PROJECT_VERSION));
}

#ifdef USE_NAVISERVER
int Ns_ModuleInit(const char *server, const char *module) {
    (void)module;
    Ns_TclRegisterTrace(server, (Ns_TclTraceProc *)Tqrcodegen_Init, server, NS_TCL_TRACE_CREATE);
    return NS_OK;
}
#endif
