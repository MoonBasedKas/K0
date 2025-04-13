fun main() {
    val name: String = "h"
    val displayName: String = name ?: "Guest"

    println("Display name: $displayName") // Output: Display name: Guest

    val length: Int? = name?.length ?: null

    println("Length: $length") // Output: Length: 0
}