fun helper() : Int {
    if (true) { 
        return 10; 
    } else { 
        return 20; 
    };
}

fun compute() : Int {
    var sum: Int = 0;
    for (i: Int in 1..5) {
        if (i % 2 == 0) { 
            sum = sum + i; 
        } else { 
            sum = sum + helper(); 
        };
    };
    return sum;
}

fun main() : Int {
    return compute();
}