import kotlin.random.Random



fun main() {
    val listOne = ArrayList<Int>()
    val listTwo = ArrayList<Int>()
    val differences = ArrayList<Int>()
    val temp = 5
    for (i in 1..temp){
        listOne.add(Random.nextInt(10))
        listTwo.add(Random.nextInt(10))
    }
    var i = 0
    var minOne = 101
    var minTwo = 101
    while (listOne.size > 0){
        if (listOne[i] < minOne){
            minOne = listOne[i]
        }
        if (listTwo[i] < minTwo){
            minTwo = listTwo[i]
        }

        i = (i + 1) % listOne.size
        if (i == 0){
            differences.add(minOne - minTwo)
            listOne.remove(minOne)
            listTwo.remove(minTwo)
            minOne = 101
            minTwo = 101
        }
    }
    var total = 0
    for (x in differences){
        total += x
        println(x)
    }
    println("Total offset: $total")
}