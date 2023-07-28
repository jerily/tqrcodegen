#include <stdbool.h>
#include "qrcodegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "library.h"

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

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[] = {0, 2, 1};


char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}

// Prints the given QR Code to the console.
//static void printQr(const uint8_t qrcode[]) {
//    int size = qrcodegen_getSize(qrcode);
//    int border = 4;
//    for (int y = -border; y < size + border; y++) {
//        for (int x = -border; x < size + border; x++) {
//            fputs((qrcodegen_getModule(qrcode, x, y) ? "##" : "  "), stdout);
//        }
//        fputs("\n", stdout);
//    }
//    fputs("\n", stdout);
//}

// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
static Tcl_Obj *toSvgString(const uint8_t qrcode[], int border) {
    int size = qrcodegen_getSize(qrcode);
    if (border < 0)
        return NULL;
    if (border > INT_MAX / 2 || border * 2 > INT_MAX - size)
        return NULL;

    Tcl_Obj *sb = Tcl_NewObj();
    Tcl_AppendStringsToObj(sb,
                           "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",
                           "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n",
                           "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ",
                           (size + border * 2), " ", (size + border * 2), "\" stroke=\"none\">\n",
                           "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n",
                           "\t<path d=\"");
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcodegen_getModule(qrcode, x, y)) {
                if (x != 0 || y != 0)
                    Tcl_AppendStringsToObj(sb, " ", "M", (x + border), ",", (y + border), "h1v1h-1z");
            }
        }
    }
    Tcl_AppendStringsToObj(sb, "\" fill=\"#000000\"/>\n", "</svg>\n");
    return sb;
}

static int tqrcodegen_EncodeToSvg(ClientData  clientData, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[] ) {
    DBG(fprintf(stderr,"GetEncodeToSvg\n"));

    CheckArgs(2,2,1,"text");
    uint8_t qr0[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText("Hello, world!",
                                   tempBuffer, qr0, qrcodegen_Ecc_MEDIUM,
                                   qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                                   qrcodegen_Mask_AUTO, true);
    if (ok) {
        Tcl_Obj *svg = toSvgString(qr0, 4);
        Tcl_SetObjResult(interp, svg);
        return TCL_OK;
    } {
        return TCL_ERROR;
    }
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
