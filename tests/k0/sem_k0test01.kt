fun add(x: Int, y: Int): Int {
    return x + y;
}

fun main(args: Array<String>) {
    val result: Int = add(5, 10);
    print("Result: " + result);
}

//This is an Int
fun dummy(r: Long): Long {
    return r;
}

//This is a Double
fun dummy2(q: Float): Float {
    var temp: Float = q;
    return temp;
}

//This is a Double
fun dummy3(z: Double): Double {
    var temp: Double = z;
    return temp;
}

//This is an Int
fun dummy4(p: Byte): Byte {
    var temp: Byte = p;
    return temp;
}
