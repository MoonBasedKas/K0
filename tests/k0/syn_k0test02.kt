fun checkPositive(b: Byte): Boolean {
    if (b > 0) {
        return true
    } else {
        return false
    }
}

fun main(args: Array<String>) {
    val x: Byte = 5
    if (checkPositive(x)) {
        println("Byte is positive")
    } else {
        println("Byte is non-positive")
    }
}