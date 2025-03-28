fun arrayTest() : Int {
    val arr: Int(10) { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    arr[1] = 42;
    return arr[1];
}