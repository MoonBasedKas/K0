fun operators() {
    var x = 5
    x += 3
    x -= 2
    val a = 10 + 20 - 5 * 3 / 2 % 4;
    val b = a++ // increment operator
    val c = a-- // decrement operator
    val eqTest = (a == b) && (a != c)
    val range1 = 1..100
    val range2 = 1..<100
    val lambda = { x: Int -> x * 2 }
}