package lexicalcheck

// Importing functions and constants from the Kotlin standard library
import kotlin.math.PI
import kotlin.math.cos
import kotlin.collections.listOf

// Using Kotlin specific keywords
data class Point(val x: Int, val y: Int)

object Singleton {
    fun showMessage() = "Hello from the singleton object!"
}

class Calculator {
    companion object {
        // Inline function using Kotlin's built-in inline modifier
        inline fun add(a: Int, b: Int): Int = a + b
    }
}

// Defining an infix function to check alternative operator syntax
infix fun Int.multiplyBy(other: Int): Int = this * other

fun main() {
    println("Lexical check: Imports and Keywords")
    val point = Point(10, 20)
    println("Point: $point")
    println("Singleton says: ${Singleton.showMessage()}")
    println("Calculator.add(3, 4) = ${Calculator.add(3, 4)}")
    println("Using infix: 5 multiplyBy 6 = ${5 multiplyBy 6}")
    println("Cosine of PI: ${cos(PI)}")
}
