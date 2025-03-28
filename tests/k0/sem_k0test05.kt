fun outer(x: Int): Int {
    fun inner(y: Int): Int {
        return y * 2;
    }
    return inner(x) + 1;
}

fun main(args: Array<String>) {
    val value: Int = outer(10);
    print("Nested function result: " + value);
}