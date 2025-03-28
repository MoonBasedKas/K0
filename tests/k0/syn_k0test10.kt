import java.util.Random
import java.lang.Math

fun main(args: Array<String>) {
    println("Enter a word:")
    val input: String = readln()

    // Use String.length() and substring(...)
    if (input.length() >= 3) {
        println("Substring(0,3): " + input.substring(0, 3))
    }

    // Compare with "hello" using equals(...)
    if (input.equals("hello")) {
        println("You typed hello!")
    } else {
        println("You didn't type hello.")
    }

    // Nested function that uses Random and Math
    fun doubleRandom(r: Random): Int {
        val randVal: Int = r.nextInt(100) // range 0..99
        return Math.max(randVal, 0) * 2
    }

    val randomGen = Random()
    val dr = doubleRandom(randomGen)
    println("Double random(0..99): " + dr)
}