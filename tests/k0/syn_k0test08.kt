fun nestedLoops() : Int {
    var total: Int = 0;
    for (i: Int in 1..3) {
        for (j: Int in 1..3) {
            total = total + (i * j);
        };
    };
    return total;
}

fun main() : Int {
    return nestedLoops();
}