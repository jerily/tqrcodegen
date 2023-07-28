set dir [file dirname [info script]]

package ifneeded tqrcodegen 0.1 [list load [file join $dir libtqrcodegen.so]]
