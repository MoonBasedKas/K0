fun computeValue(x: Int) : Int {
    if (x < 10) {
        if (x % 2 == 0) {
            return x * 2;
        } else {
            return x * 3;
        };
    } else {
        when {
            x < 20 -> { return x + 10; };
            x < 30 -> { return x + 20; };
            else   -> { return x + 30; };
        };
    };
}

fun helperFunction() : Int {
    return computeValue(5) + computeValue(15);
}

fun main() : Int {
    return helperFunction();
}