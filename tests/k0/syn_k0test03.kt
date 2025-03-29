fun shortCountdown(s: Short) {
    var current: Short = s
    while (current > 0) {
        println(current)
        current = (current - 1).toShort()
    }
    do {
        println("Blast off!")
    } while (false)
}

fun main(args: Array<String>) {
    shortCountdown(3)
}