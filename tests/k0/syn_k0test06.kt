fun halfAndAbs(f: Float): Float {
    // Must have type mentioned
    val half = f / 2.0F
    val doubleVal = half.toDouble() // Not in standard library
    val absVal = java.lang.Math.abs(doubleVal)  // returns a Double
    return absVal.toFloat() //Not in standard library
}

fun main(args: Array<String>) {
    val original: Float = -6.28F
    val result: Float = halfAndAbs(original)
    println("Half of " + original + " and abs is " + result)
}