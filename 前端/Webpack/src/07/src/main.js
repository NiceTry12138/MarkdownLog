// import * as add from "./util/add"
// const data = require("./util/data")

const add = require('./util/add')
import * as add1 from "./util/add"
import * as data from "./util/data"
const data1 = require('./util/data')

import "./test"

console.log(add.add(1, 2));
console.log(add1.add(1, 2));
console.log(data.today())
console.log(data1.today())
