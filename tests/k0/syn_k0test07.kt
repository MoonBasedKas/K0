fun fillArray(arr: Array<Double>) {
    var arr2: Array<Double>(3) { 0.0, 0.0, 0.0 }
    arr2[0] = 3.14
    arr2[1] = 2.718
    arr2[2] = 1.414
}

fun main(args: Array<Double>) {
    var nums: Array<Double>(3) { 0.0, 0.0, 0.0 }
    fillArray(nums)
    println("First element in nums: " + nums[0])
}