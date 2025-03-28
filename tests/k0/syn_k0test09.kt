fun firstChar(chars: Array<Char>): Char {
    return chars[0]
}

fun main(args: Array<String>) {
    var letters: Array<Char>(3) { 'A', 'B', 'C' }
    println("First letter is: " + firstChar(letters))
    println("Also, this sucks :()")
}