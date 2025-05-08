fun random(x: Int) {
   val d20 : Int = 1
   d20 = 1 + d20
//    return d20 != 20
}

fun main() {
	var test1 : Int = 0
	var test2 : Int = 0
        // infinite loop
	while (true) {
		test1 = test1 + 1
	}

	// While loop with incrementing variable
	test1 = 0
	while (test1 < 10) {
		test1 = test1 + 1
	}

	// While loop with different expression
	test2 = 0
	while (test2 != 1) {
		test2 = 1
	}

	// While loop with function call
	random(1)
	while (1 != 1) {
		println("X")
	}

	// For loop
	for (test1 in 0..20) {
		println("X")
	}
}