fun process(x: Int) : Int {
    if (x > 0) { 
        when {
            x == 1 -> { return 100; };
            x == 2 -> { return 200; };
            else   -> { return 300; };
        };
    } else {
        if (x == 0) { 
            return 0; 
        } else { 
            return -1; 
        };
    };
}

fun main() : Int {
    val a: Int = process(1);
    val b: Int = process(2);
    val c: Int = process(0);
    return a + b + c;
}