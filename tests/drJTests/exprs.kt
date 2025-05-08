fun main() {
  var i : Int = 5;
  var j : Int = i + i;
  var k : Int = j + j * i;
  j = k / j;
  i = k / (j + 1);
}