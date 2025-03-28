fun halfAndAbs(f: Float): Float {
    val half = f / 2.0F
    val doubleVal = half.toDouble()
    val absVal = java.lang.Math.abs(doubleVal)  // returns a Double
    return absVal.toFloat()
}

fun main(args: Array<String>) {
    val original: Float = -6.28F
    val result: Float = halfAndAbs(original)
    println("Half of " + original + " and abs is " + result)
}