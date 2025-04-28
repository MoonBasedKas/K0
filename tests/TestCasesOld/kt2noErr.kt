// single line comment

fun assignments() {
        val a : Int = 1;
        val b : Int = 2;
        val c : Int = - 3;
}

fun Operators() {
   var w : Int = 0;
   var x : Int = -1;
   var y : Int = 0;
   var z : Int =0;
   var q : Int =0;

   if ((x < y) && (y <= z) || (z < q) && (q != x)) {
           x = y;
           y = z
           }
   else {
           w = x + y * z / q - 4
   }

   var logicalValue : Boolean = w > 20;
}

fun WhileStuff() {
   var x : Int = 0;
   while (x < 20) {
      x = 6;
      return
   }
}

fun RollTheDice() {
   var d6Var : Int = 6;
   var y45 : Int = d6Var;
}

fun main() {
        RollTheDice();
        println("stuff and things\n");
        var p0var : Int = 2;
}

// // class variable declaration
// fun MakeAPet() {
//    //local scope variable declaration. err here on line 49 (no pet!)
//   var p : pet = pet();
//   p.play();
//   println("{p}\n")
// }

fun stringOps() : String {
        var S1 : String = "One";
        var S2 : String = S1 + "Two"; // uh oh
        return S2
}

fun listOps() : Array<Int> {
        var L : Array<Int> = Array<Int>( 3) { 0 };
        L[0] = 1; L[1] = 2; L[2] = 3;
        return L // uh oh
}

fun elvis() {
   var x : Int? = 2
   var y : Int = 2
   var z : Int
   z = (x?:0) + y
}