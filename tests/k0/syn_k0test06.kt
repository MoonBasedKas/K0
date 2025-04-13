fun main(args: Array<String>) {
    val original: Float = -6.28F
    val result: Float = halfAndAbs(original)
    println("Half of " + original + " and abs is " + result)
}

fun halfAndAbs(original: Float): Float {
    return original / 2.0F
}