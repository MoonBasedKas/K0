

// Should error for different return type
fun foo(y: String): String =
    {
        var s: String = "S"
        "meow"
    }

fun main() {
    println(foo().toString() + foo(2) + foo(""))
}
