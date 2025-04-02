fun assignments() : Int  {
        val a : Int = 1;
        val b : Int = 2;
        val c : Int = - 3;
}

fun Operators() : Int {
   var w : Int = 0;
   var x : Int;
   var y : Int = 0;
   val z : Int =0;
   val q : Int =0;
   x = -1;
   if ((x < y) && (y <= z) || (z < q) && (q != x)) 
   {
      x = y;
      y = z
   } 
   else 
   if (1 == 1) 
   {
      w = x + y * z / q - 4
   } 
   else 
   {
      return 
   }

   val logicalValue : Boolean = w > 20;
}

fun WhileStuff() : Int  {
   var x : Int = 0;
   while (x < 20) 
   {
      x = 6;
      return 
   }
}

fun RollTheDice() : Int  {
   val d6Var : Int = 6;
   val y45 : Int = d6Var;
}

fun main() : Int {
        RollTheDice();
        println("stuff and things\n");
        val p0var : Int = 2;
}

// class variable declaration
fun MakeAPet()  : Int {
   // local scope variable declaration
   val happy : Int = 1;
   println("{happy}\n")
}

fun stringOps() : String {
        val S1 : String = "One";
        val S2 : String = S1 + "Two"; // uh oh
        return S2
}

fun listOps() : Array<Int> {
        var L : Array<Int> = Array<Int>(3)
        L[0] = 1; L[1] = 2; L[2] = 3;
        return L // uh oh
}