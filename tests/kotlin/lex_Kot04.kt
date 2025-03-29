fun main() {
    // Backtick
    val `fun` = "This identifier uses a reserved word."
    println("Backtick identifier value: ${`fun`}")

    // Numeric literals with underscores for readability.
    val largeNumber = 1_000_000
    println("Large number: $largeNumber")

    val specialString = "Tab\tNewLine\nBackslash\\"
    println("Special escape sequence string: $specialString")

    val multiLineRaw = """
        >Line one with margin.
        >Line two with margin.
    """.trimMargin(">")
    println("Multi-line raw string:\n$multiLineRaw")
}