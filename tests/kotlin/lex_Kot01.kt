#!/usr/bin/env Kotlin

// Single-line and multi-line comments:
/// This is a single-line comment.
 /* This is a
    multi-line comment.
    /* This is a nested comment. */
 */

fun main() {
    println("Running as a Kotlin script with a shebang!")
    val escapeString = "Line1\nLine2\tTabbed"
    println("Escape sequences test: $escapeString")
}