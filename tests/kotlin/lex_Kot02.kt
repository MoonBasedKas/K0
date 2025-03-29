package lexicalcheck

import kotlin.math.PI
import kotlin.math.cos
import kotlin.collections.listOf

data class Point(val x: Int, val y: Int)

object Singleton {
    fun showMessage() = "Hello from the singleton object!"
}

class Calculator {
    companion object {
        inline fun add(a: Int, b: Int): Int = a + b
    }
}

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
