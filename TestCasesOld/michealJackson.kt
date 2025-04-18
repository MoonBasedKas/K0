fun main() {
    val name: String = "h"
    var displayName: String = name ?: "Guest"
    displayName = name ?: null
    // displayName = null

    var x : Int? = 1
    var y : Int = 2
    var z : Int
    z = x + y
    println("Display name: $displayName") // Output: Display name: Guest

    // val length: Int? = name?.length ?: null

    println("Length: $length") // Output: Length: 0
}