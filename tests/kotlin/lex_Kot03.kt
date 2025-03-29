package lexicalcheck

fun main() {
    print("Using print() without newline. ")
    println("Now using println() to end the line.")

    val numbers = listOf(1, 2, 3, 4, 5)
    val squared = numbers.map { it * it }
    println("Squared numbers: $squared")

    val evenNumbers = numbers.filter { it % 2 == 0 }
    println("Even numbers: $evenNumbers")

    // Testing various numeric literal forms
    val intLiteral = 42
    val longLiteral = 42L
    val hexLiteral = 0xFF
    val binaryLiteral = 0b101010
    println("Int: $intLiteral, Long: $longLiteral, Hex: $hexLiteral, Binary: $binaryLiteral")

    // Raw string literal with trimMargin and interpolation
    val rawString = """
        |This is a raw string literal.
        |It spans multiple lines.
        |It supports interpolation: ${'$'}intLiteral
    """.trimMargin()
    println("Raw string literal:\n$rawString")
}
