fun f() : Int {
   val thread_local : Int = 5
   return thread_local
}

fun g() : Int {
   return 2*2
}
test2.kt output [expect a syntax tree]
File: test2.kt Line:2 Unknown error
exit status: 1 expected 0
iftst.kt input:
fun f() {
  if (0==1) {
    println("hey");
    }
}