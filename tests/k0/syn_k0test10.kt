import java.util.Random.nextInt
import java.lang.Math.max

fun main(args: Array<String>) {
    println("Enter a word:")
    val x : Int = 10
    val input: String = readln()

    if (input.length() >= 3) {
        println("Substring(0,3): " + input.substring(0, 3))
    }

    if (input.equals("hello")) {
        println("You typed hello!")
    } else {
        println("You didn't type hello.")
    }

    fun doubleRandom(r: Int): Int {
        val randVal: Int = r.nextInt(100) // range 0..99
        return max(randVal, 0) * 2
    }

    val randomGen : Int = x.nextInt(100)
    val dr : Int = doubleRandom(randomGen)
    println("Double random(0..99): " + dr)
}