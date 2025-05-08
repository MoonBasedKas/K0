fun itoa(_i : Int) : String {
   return "not implemented" + ""
}

fun main() {
  var i : Int = 5;
  var j : Int = 0;
  var k : Int = 0;
   while (i>0) {
   i = i-1;
   }
   for (i in 0..5) {
      for (j in 0..5) {
        if (i<j) {
	k = k + 1;
	}
        if (i==j) {
	k = k + 2;
	} else {
	k = k - 1;
	}
      }
      }
   println("k $k")
}