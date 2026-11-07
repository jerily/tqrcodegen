package require tcltest 2
load [file normalize [lindex $argv 0]] Tqrcodegen
::tcltest::configure -singleproc true -testdir [file dirname [info script]]
exit [::tcltest::runAllTests]
