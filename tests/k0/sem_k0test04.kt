import String.equals

fun combine(a: String, b: String): String {
    return a + b;
}

fun main(args: Array<String>) {
    val s: String = combine("k", "0");
    if (s.equals("k0")) {
         print("String concatenation valid: " + s);
    } else {
         print("Error in string operation");
    }
}