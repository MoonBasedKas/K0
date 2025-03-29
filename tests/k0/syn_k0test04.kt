fun sumRange(n: Int): Int {
    var total: Int = 0
    for (i in 1..n) {
        total = total + i
    }
    return total
}

fun main(args: Array<String>) {
    println("Sum of 1..5: " + sumRange(5))
}