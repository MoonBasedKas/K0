fun allTrue(flags: Array<Boolean>): Boolean {
    for (i in 0..2) {
        if (!flags[i]) {
            return false
        }
    }
    return true
}

fun main(args: Array<String>) {
    var bools: Array<Boolean>(3) { true, true, true }
    bools[1] = false
    if (allTrue(bools)) {
        println("All are true.")
    } else {
        println("Not all true.")
    }
}