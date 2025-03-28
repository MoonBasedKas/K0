fun processArray(arr: Array<Int>): Int {
    arr[1] = 100;
    return arr[1];
}

fun main(): Int {
    val arr: Int(3) { 10, 20, 30 };
    return processArray(arr);
}