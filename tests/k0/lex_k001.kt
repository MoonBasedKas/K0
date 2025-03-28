fun main() {
    val value = 42
    var flag = true
    if (flag) {
        for (i in 1..10) {
            continue
        }
    } else {
        return
    }
}