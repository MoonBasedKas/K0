

// Should error for different return type
fun foo(y: String): String = 7

fun main() {
    println(foo().toString() + foo(2) + foo(""))
}
