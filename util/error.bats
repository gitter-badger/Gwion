load test_helper/bats-support/load
load test_helper/bats-assert/load
load test_helper/bats-gwion/load
DIR=test/error
@test "warn" {
  diag "testing error messages"
  assert_success
}
@test "non exitant file"                   { gbt $DIR/no_file.gw   "error while opening file"; }
@test "multiple public class"              { gbt $DIR/class_multiple_public.gw   "more than one 'public' class defined"; }
@test "unknown type in declaration"        { gbt $DIR/unknown_type.gw   "unknown in declaration"; }
@test "postfix on non-mutable data"        { gbt $DIR/postfix.gw   "cannot be used on non-mutable data-type"; }
@test "already defined class"              { gbt $DIR/defined_class.gw   "is already defined in namespace"; }
@test "class name reserved"                { gbt $DIR/class_res.gw   "is reserved"; }
@test "template unlnow type"               { gbt $DIR/template_unknown.gw   "unknown in template call"; }
@test "enum already defined"               { gbt $DIR/enum_defined.gw   "already declared"; }
@test "enum defined"                       { gbt $DIR/enum_defined_var.gw   "already declared as variable"; }
@test "enum already defined as var2"       { gbt $DIR/enum_defined2.gw   "already declared"; }
@test "enum already defined as var3"       { gbt $DIR/enum_defined3.gw   "already declared"; }
@test "invalid union declaration."         { gbt $DIR/union_invalid.gw   "invalid union declaration."; }
@test "union unknown type"                 { gbt $DIR/union_unknown.gw   "unknown type"; }
@test "union unknown type"                 { gbt $DIR/union_unknown.gw   "unknown type"; }
@test "union array must be empty"          { gbt $DIR/union_array_empty.gw   "array declaration must be empty in union"; }
@test "pointer unknown ret type"           { gbt $DIR/pointer_unknown.gw   "unknown type"; }
@test "pointer static outside class"       { gbt $DIR/pointer_outside_class.gw   "can't declare func pointer static outside of a class"; }
@test "pointer unknown argument type"      { gbt $DIR/pointer_unknown_arg.gw   "unknown type in argument"; }
@test "destructor outside class"           { gbt $DIR/dtor_outside_class.gw   "dtor must be in class def"; }
@test "function unknown ret type"          { gbt $DIR/func_unknown_ret.gw   "unknown return type"; }
@test "function arg unknown"               { gbt $DIR/func_arg_unknown.gw   "unknown type in argument"; }
@test "operator argument number"           { gbt $DIR/operator_narg.gw   "operators can only have one or two arguments"; }
@test "operator valid"                     { gbt $DIR/operator_valid.gw   "is not a valid operator name"; }

###########
#  Scan 2 #
###########
@test "variable of size zero"              { gbt $DIR/0_size_variable.gw   "cannot declare variables of size '0'"; }
@test "class declared inside itself"       { gbt $DIR/class_inside.gw   "declared inside itself"; }
@test "primitive reference"                { gbt $DIR/prim_ref.gw   "cannot declare references (@) of primitive type"; }
@test "decl reserved"                      { gbt $DIR/decl_res.gw   "... in variable declaration"; }
@test "variable already defined"           { gbt $DIR/var_defined.gw   "has already been defined in the same scope..."; }
@test "ptr already defined"                { gbt $DIR/ptr_defined.gw   "has already been defined in the same scope"; }
@test "ptr no size argument"               { gbt $DIR/ptr_arg_0_size.gw   "cannot declare variables of size '0'"; }
@test "ptr reference primitive"            { gbt $DIR/ptr_ref_prim.gw   "cannot declare references (@) of primitive type"; }
@test "function pointer argument array "   { gbt $DIR/ptr_arg_array.gw   "must be defined with empty []'s"; }
@test "template inexistant"                { gbt $DIR/template_inexistant.gw   "template call of non-existant function"; }
@test "template call non template"         { gbt $DIR/template_non.gw   "template call of non-template function"; }
@test "label already defined"              { gbt $DIR/label_defined.gw   "already defined"; }
@test "enum already declared"              { gbt $DIR/enum_declared.gw   "already declared as variable"; }
@test "nested function"                    {
  skip "this fail at compiler level"
  gbt function_nested.gw   "nested function definitions are not (yet) allowed;";
}
@test "function already used"              { gbt $DIR/function_used.gw   "is already used by another value"; }
@test "function primitive ref return type" { gbt $DIR/function_ret_ref_prim.gw   "cannot declare references (@) of primitive type"; }
@test "function argument of no size"       { gbt $DIR/function_arg_no_size.gw   "cannot declare variables of size"; }
@test "function argument reserved"         { gbt $DIR/function_arg_res.gw   "in function"; }
@test "function argument array empty"      { gbt $DIR/func_arg_array_empty.gw   "must be defined with empty []'s"; }
@test "function different signature"       { gbt $DIR/func_sig_differ.gw   "function signatures differ in return type"; }

##########
# type.c #
##########
@test "invalid plugin" {
  touch /usr/lib/Gwion/plug/invalid.so
  run ./gwion -d dummy
  [ $status -ne 139 ]
  assert_success
  assert_output --partial "error in"
  rm /usr/lib/Gwion/plug/invalid.so
}

@test "invalid plugin2" {
  echo "static int i;" > /usr/lib/Gwion/plug/invalid.c
  cc -shared -fPIC /usr/lib/Gwion/plug/invalid.c -o /usr/lib/Gwion/plug/invalid.so
  rm /usr/lib/Gwion/plug/invalid.c
  gbt "NOFILE" "no import function"
  rm /usr/lib/Gwion/plug/invalid.so
}

@test "compat sub"                           { gbt $DIR/no_compat_sub.gw   "must be of type 'int'"; }
@test "array subscript"                      { gbt $DIR/no_compat_sub2.gw   "incompatible array subscript type"; }
@test "undefined parent class"               { gbt $DIR/undef_parent.gw   "undefined parent class"; }
@test "extend primitive"                     { gbt $DIR/prim_parent.gw   "cannot extend primitive"; }
@test "incomplete parent class"              { gbt $DIR/incomplete_parent.gw   "cannot extend incomplete type"; }
@test "already define in  parent class"      { gbt $DIR/already_parent.gw   "has already been defined in parent class"; }
@test "GACK first"                           { gbt $DIR/gack_decl.gw   "cannot use <<< >>> on variable declarations"; }
@test "GACK2 other"                          { gbt $DIR/gack_decl2.gw   "can't declare in GACK"; }
@test "static variable outside class"        { gbt $DIR/static_out.gw   "static variables must be declared at class scope"; }
@test "init array"                           { gbt $DIR/provide_array.gw   "must provide values/expressions for array [...]"; }
@test "provide array"                        { gbt $DIR/array_incompatible.gw   "array init [...] contains incompatible types..."; }
@test "vector too big"                       { gbt $DIR/vector_too_big.gw   "vector dimensions not supported > 4..."; }
@test "invalid type in vector"               { gbt $DIR/vector_invalid.gw   "in vector value"; }
@test "this outside of class"                { gbt $DIR/this.gw   "keyword 'this' can be used only inside class definition..."; }
@test "this in static function"              { gbt $DIR/this_static.gw   "keyword 'this' cannot be used inside static functions..."; }
@test "complex too short"                    { gbt $DIR/missing_im.gw   "missing imaginary component of complex value..."; }
@test "complex too big"                      { gbt $DIR/complex_too_big.gw   "extraneous component of complex value..."; }
@test "invalid real"                         { gbt $DIR/complex_invalid_re.gw   "in real component of complex value"; }
@test "invalid im"                           { gbt $DIR/complex_invalid_im.gw   "in imaginary component of complex value"; }
@test "polar too short"                      { gbt $DIR/missing_phase.gw   "missing phase component of polar value..."; }
@test "polar too big"                        { gbt $DIR/polar_too_big.gw   "extraneous component of polar value..."; }
@test "polar invalid modulus"                { gbt $DIR/polar_invalid_mod.gw   "in modulus component of polar value..."; }
@test "polar invalid phase"                  { gbt $DIR/polar_invalid_ph.gw   "in phase component of polar value..."; }
@test "array exceed dimension"               { gbt $DIR/array_excess.gw   "exceeds defined dimension"; }
@test "array invalid type"                   { gbt $DIR/array_invalid.gw   "must be of type 'int'"; }
@test "ptr in class"                         { gbt $DIR/ptr_assign.gw   "can't assign function pointer to function pointer for the moment. sorry."; }
@test "assign function ptr"                  { gbt $DIR/ptr_assign_class.gw   "can't assign function pointer in class for the moment. sorry."; }
@test "assign function ptr from other class" { gbt $DIR/ptr_assign_other.gw   "can't assign member function to member function pointer of an other class"; }
@test "assign member ptr"                    { gbt $DIR/ptr_assign_member.gw   "can't assign member function to non member function pointer"; }
@test "assign non member ptr"                { gbt $DIR/ptr_assign_global.gw   "can't assign non member function to member function pointer"; }
@test "not match found for function"         { gbt $DIR/ptr_match.gw   "not match found for function"; }
@test "not match found for operator"         { gbt $DIR/op_match.gw   "not match found for operator"; }
@test "left-side is not mutable"             { gbt $DIR/left_mutable.gw "left-side operand is not mutable"; }
@test "right-side is not mutable"            { gbt $DIR/right_mutable.gw "right-side operand is not mutable"; }
@test "unknown type in cast expression"      { gbt $DIR/cast_unknown.gw   "in cast expression"; }
@test "postfix no match"                     { gbt $DIR/postfix_no_match.gw   "no suitable resolutation for postfix operator"; }
@test "dur prefix"                           { gbt $DIR/dur_prefix.gw   "in prefix of dur expression"; }
@test "dur postfix"                          { gbt $DIR/dur_postfix.gw   "in postfix of dur expression..."; }
@test "non-existing function"                { gbt $DIR/func_exist.gw   "function call using a non-existing function"; }
@test "non function"                         { gbt $DIR/func_non.gw   "function call using a non-function value"; }
#@test "guess"                                { gbt $DIR/guess.gw   "not enough argument"; }
