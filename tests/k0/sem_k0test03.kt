fun updateArray(arr: Array<Int>): Int {
    arr[0] = 42;
    return arr[0];
}

fun main(args: Array<String>) {
    var arr: Array<Int>(3) { 1, 2, 3 };
    val value: Int = updateArray(arr);
    print("Updated value: " + value);
}