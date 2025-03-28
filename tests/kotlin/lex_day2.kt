import java.io.File
import java.io.InputStream
import kotlin.math.abs

fun main(){
    val file = File("data/raindeer.txt")
    val inputStream = file.inputStream()
    val inputString = inputStream.reader().use {it.readText()}
    val inputs = inputString.split('\n')

    val data = ArrayList<ArrayList<Int>>()

    for (i in inputs){
        data.add(breakDown(i))
    }

    for (x in data){
        print(x)
        if(validate(x)){
            println("GOOD SAFE EVEN!")
        }else{
            println("WE'RE DOOMED!")
        }
    }

    println(inputs)
}

fun breakDown(arr: String): ArrayList<Int>{
    val numbers = arr.split(" ")
    val finalResult = ArrayList<Int>()
    var i = 0
    while(i < numbers.size){
        finalResult.add(numbers[i].toInt())
        i++
    }
    return finalResult
}


fun validate(arr: ArrayList<Int>): Boolean{
    var change = 0
    var lastChange = 0
    var increasing = true
    var good = true
    var i = 1
    while(i < arr.size){
        lastChange = change
        change = arr[i] - arr[i - 1]
//        println(change)

        if (increasing && i == 1 && change < 0){
            increasing = false
        }

        if (increasing){
            if (change == 0){
                return false
            }
            if (abs(change) <= 2 && abs(change) > 0){
                good = true
            } else if (abs(change) <= 3 && abs(change) > 0){
                good = true
            }else{
                return false
            }

            if(abs(change + lastChange) == 0){
                return false
            }

            if (change < 0){
                return false
            }
        } else {
            if (change == 0){
                return false
            }
            if (abs(change) <= 2 && abs(change) > 0){
                good = true
            } else if (abs(change + lastChange) <= 3 && abs(change + lastChange) > 0){
                good = true
            }else{
                return false
            }

            if(change > 0){
                return false
            }
        }

        i += 1
    }
    return true
}