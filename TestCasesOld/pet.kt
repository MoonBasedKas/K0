fun play() {
   var hunger : Int = 0
   var happy : Int = 0
   println("What should we play?\n");
   println( "Fetch (1) \n Roll over (2)\n" )
   val choice : Int = 2
   if (choice == 1) {
      println("Fetch, eh")
      happy = happy+10
   } 
   else {
      if (choice == 2) {
         println("Roll over?  Really?")
         hunger=hunger+ 1
      }
   }
}

fun main() {
  play()
  }