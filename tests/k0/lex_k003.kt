fun main(args : Array<String>) {
    // Numeric literals
    val dec : Int = 123456;
    val longNum : Int = 98765L;
    val dbl : Double = 3.14159;
    val flt : Double = 2.71828F; // This will be ignored, and just be a double

    // Character literals with escape sequences
    val ch1 : Char = 'A';
    val ch2 : Char = '\n';

    // String literal with escape sequences and a field identifier ($foo)
    val str : String = "Line1\nLine2\twith tab and a dollar sign: \$foo";
    
    // Multi-line string literal using triple quotes
    val multi : String = """This is a multi-line
string literal with "quotes" and a backslash \\ in it."""
    
    println("Literals tested:");
    println("dec: " + dec);
    println("longNum: " + longNum);
    println("dbl: " + dbl);
    println("flt: " + flt);
    println("hex: " + hex);
    println("Character A: " + ch1);
    println("Newline char: " + ch2);
    println("String: " + str);
    println("Multi-line string: " + multi);
}