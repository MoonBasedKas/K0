fun mixNumbers(a: Int, b: Double): Double {
    return a + b;  // Numeric types are compatible
}

fun main(args: Array<String>) {
    val result: Double = mixNumbers(3, 4.5);
    print("Mixed Numbers: " + result);
}