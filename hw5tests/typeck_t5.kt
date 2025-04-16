// Expect type error on line 4
fun main() {
   var j : Int = 0;
   var s : atypename;
   s = "hello";
   j = s.len();
   j = dict();
   println("{j}");
}