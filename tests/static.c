#include "library.h"
#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    Tcl_FindExecutable(argv[0]);
    Tcl_Interp *interp = Tcl_CreateInterp();
    int result = Tcl_Init(interp);
    if (result == TCL_OK) result = Tqrcodegen_Init(interp);
    if (result == TCL_OK) {
        result = Tcl_Eval(interp,
            "package require tqrcodegen 1.1; "
            "if {![string match {*M4,4h1v1h-1z*} [::tqrcodegen::encode_to_svg HELLO]]} "
            "{error {missing finder module}}");
    }
    if (result != TCL_OK) fprintf(stderr, "%s\n", Tcl_GetStringResult(interp));
    Tcl_DeleteInterp(interp);
    Tcl_Finalize();
    return result == TCL_OK ? 0 : 1;
}
