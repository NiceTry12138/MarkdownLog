"use strict";

var foo = function foo() {
  return "hello world";
};
var arr = [1, 2, 3];
foo();
console.log(arr.at(1));
module["export"] = {
  foo: foo
};