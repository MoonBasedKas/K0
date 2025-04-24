// --- foo: takes two Ints, returns an Int ---
fun foo(a: Int, b: Int): Int {
    val x: Int = a + b
    if (x > 0) {
        return x
    }
    return 0
}

// --- main: entry point, no return type annotation here ---
fun main(args: Array<String>) {
    val result: Int = foo(5, -3)
    println("Finished")
}
