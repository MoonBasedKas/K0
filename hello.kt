fun main() {
    // Print a welcome message
    println("Welcome to the Kotlin Introductory Class!")

    // Declare and initialize variables
    val name = "John"  // Immutable variable
    var age = 20       // Mutable variable

    // Print the variables
    println("Name: $name")
    println("Age: $age")

    // Ask for user input and store it in a variable
    println("Enter your favorite color:")
    val favoriteColor = readLine()  // Reading input as a string

    // Check the user's favorite color
    if (favoriteColor != null) {
        println("Your favorite color is $favoriteColor!")
    } else {
        println("You didn't enter a color!")
    }

    // A simple conditional statement
    if (age >= 18) {
        println("You are an adult!")
    } else {
        println("You are a minor!")
    }

    // Using a loop to print numbers from 1 to 5
    println("Counting from 1 to 5:")
    for (i in 1..5) {
        println(i)
    }

    // Function to calculate the square of a number
    val square = squareNumber(4)
    println("The square of 4 is $square")

    // Function to calculate the area of a rectangle
    val rectangleArea = calculateAreaOfRectangle(5, 10)
    println("The area of the rectangle is $rectangleArea")

    // Function to find the maximum of three numbers
    val maxNumber = findMaxOfThree(15, 42, 28)
    println("The maximum number is $maxNumber")

    // Function with multiple parameters: calculate the total cost
    val totalCost = calculateTotalCost(100.0, 5, 15.0)
    println("The total cost is $$totalCost")
}

// A function that squares a number
fun squareNumber(number: Int): Int {
    return number * number
}

// A function to calculate the area of a rectangle (length * width)
fun calculateAreaOfRectangle(length: Int, width: Int): Int {
    return length * width
}

// A function to find the maximum of three numbers
fun findMaxOfThree(a: Int, b: Int, c: Int): Int {
    return maxOf(a, maxOf(b, c))
}

// A function to calculate the total cost of items with a price per item, quantity, and discount
fun calculateTotalCost(pricePerItem: Double, quantity: Int, discountPercentage: Double): Double {
    val totalCostBeforeDiscount = pricePerItem * quantity
    val discountAmount = totalCostBeforeDiscount * (discountPercentage / 100)
    return totalCostBeforeDiscount - discountAmount
}

// A function to greet a user with a custom message, using multiple parameters
fun greetUser(name: String, greeting: String, age: Int) {
    println("$greeting, $name! You are $age years old.")
}
