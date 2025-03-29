fun fillArray(arr: Array<Double>) {
    arr[0] = 3.14
    arr[1] = 2.718
    arr[2] = 1.414
}

fun main(args: Array<String>) {
    var nums: Array<Double>(3) { 0.0, 0.0, 0.0 }
    fillArray(nums)
    println("First element in nums: " + nums[0])
}