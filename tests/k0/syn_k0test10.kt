fun sumArray(arr: Array<Int>) : Int {
    var total: Int = 0;
    // Assume the array is of length 5.
    for (i: Int in 0..4) {
        total = total + arr[i];
    };
    return total;
}

fun complexFunction(arr: Array<Int>) : Int {
    var result: Int = 0;
    for (i: Int in 1..3) {
        if (i == 2) {
            result = result + sumArray(arr);
        } else {
            result = result + i;
        };
    };
    return result;
}

fun main() : Int 
    {
    // Array declaration using the arrayDeclaration syntax.
    val arr: Int(5) { 1, 2, 3, 4, 5 };
    // Pass the array as an argument to complexFunction.
    val res: Int = complexFunction(arr);
    if (res > 10) { 
        return res; 
    } else { 
        return 0; 
    };
}