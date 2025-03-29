fun main(args : Array<String>) {
    var x : Int = 5;
    x += 10;
    x -= 3;
    x++;
    x--;
    
    if (x == 12 && x != 0) {
        println("x equals 12 and is nonzero");
    }
    if (x < 20 || x >= 10) {
        println("x is between 10 and 20 or greater");
    }
    
    // Using range operators in expressions (the tokens ".." and "..<")
    println("Range: " + (1..10));
    println("Range Until: " + (1..<10));
    
    // Test unary operators
    if (!(x < 0)) {
        println("x is non-negative");
    }
}