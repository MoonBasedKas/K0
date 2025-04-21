// Expect nullable error on line 6
fun elvis() : Int {
   var x : Int? = 2
   var y : Int = 2
   var z : Int
   z = x + y
   return z
}