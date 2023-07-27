set dir [file dirname [info script]]
lappend auto_path $dir
package require tqrcodegen

puts [::tqrcodegen::encode_to_svg "Hello, world!"]