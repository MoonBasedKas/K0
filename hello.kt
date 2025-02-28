const val PI = 3.14159

fun calculateArea(radius: Double): Double {
    return PI * radius * radius
}

fun main() {
    val radius = 5.0
    val area = calculateArea(radius)
    println("The area of the circle with radius $radius is: $area")
}