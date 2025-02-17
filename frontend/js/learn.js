// 变量声明
const date = Date()
// 建议用let,const，它们的作用域和c语言变量相同
// var作用于全局或整个函数，不建议
// 如果不加任何标识的声明，则为全局变量

// 变量提升：变量的声明和使用不论前后
function f(){
    x = 1
    // ...
    var x = 2;
    // ...

    // 作用与上同
    // var x = 1
    // x = 2
}

// ------------------------------------------------------------

console.log("...")
window.alert("FuckU!!!")
document.getElementById("p1").innerHTML = date
// console.log          控制台输出
// window.alert(...)    弹出警告窗
// document.write(...)  类似宏，直接在html文件写入，不能动态写入
// innerHTML    html内元素的值


// ------------------------------------------------------------
// 数据类型 typeof
// Boolean Number String Null Undefined Symbol Object

// 数组
var arr

1.
arr = new Array() // arr = []
arr[0] = 0
arr[1] = 1
arr[2] = 2

2.
arr = new Array(1, 2, 3)

3.
arr = [1, 2, 3]

// 对象
var obj

obj = new Object()
obj = { A: 1, B: true, C: "str" }

// 两种访问方式
var a = obj.A
obj['A'] = a

obj.F = function () {       // 成员函数
    // code
    this.A = 2 // 用this访问对象成员
}

// 模版字符串
var str = `1+1=${1 + 1}` // "1+1=2"   ${表达式}
str += 1    // str += "1"

// ------------------------------------------------------------
// 比较
var a = 1
var b = "1"

a == b      // true     转换 "1" -> 1
a === b     // false    不转换数据类型
a != b
a !== b
a <= b      // true     会转换

'a' < 'b'   // true     两边字符串，比较ascii

// ------------------------------------------------------------
// 条件和循环
// switch/case, if/else, for(;;), while, do/while 与c语言相同
// for(x in obj) 与python类似
// case 同 ===

outerloop:
while (true) {
    innerloop:
    while (true) {
        break outerloop     // 跳出标签下的代码块
        break innerloop     // 此处等同break，跳出当前代码块
        continue outerloop
        continue innerloop  // continue
    }
}

// ------------------------------------------------------------
// 类型转换

// 字符串
var xxx;

String(xxx)
xxx.toString()

// 数字
Number(xxx)
    + xxx
// 单个 + 是强制转换为数字的运算符，若无法转换，则为NaN

// ------------------------------------------------------------
// 正则表达式

var patten = /a[0-9]c/i
// 可以在str的函数中使用

// ------------------------------------------------------------
// 错误处理

try {
    if (xxx == 1) throw "boom!!!"   // 自定义的throw可以抛任何东西
    xxx /= 0
} catch (err) {
    console.log(err)

    // 也可能是标准错误
    console.log(err.message)

} finally {
    // 无论错误与否都会执行的代码
}

// ------------------------------------------------------------
// 函数

// function实际上是一种常量对象
function func(a, b, c=1){
    let l = arguments.length
    let j = arguments[0]        // arguments，参数数组，实现变长参数

    this.a = a          // 类似static变量
    
    if(b === undefined){
        // 调用时可以不输入全部参数，初始化为undefined，或自定义值
    }

    return 6;
}

func(1) // (1, undefined, 1), func.a = 1

var q = func    // 浅拷贝function对象, q.a = 1
q = func(2)     // 调用一次func（改变其成员），并返回return值
                // func.a = 2,q = 6
q = new func(3) // 返回一个新的func对象，成员初始化, q.a = 3

// 箭头函数

var f1 = (x, y) => {return x * y}
var f2 = (x, y) => x * y    // 简写，去掉{}和return

// ------------------------------------------------------------
// 类

class MyClass{
    static #p = 1       // 在构造函数外声明变量，#代表私有变量
                        // 私有变量即使是子类也不能调用
    constructor(a, b){  // 构造函数
        this.a = a
        this.b = b
    }

    #method(){          // static和#都可以用来修饰变量和方法，有无#是两个函数
        return this.a + this.b
    }
    
    static method(){    // 静态方法，不能通过对象调用，
        return 1        // 只能通过MyClass.method()
    }

    get this_a(){       // getter
        return this.a
    }

    set this_b(b){      // setter
        this.b = b
    }
}

var mc = new MyClass(1, 2)    // 必须用new来初始化

// getter & setter 分别在读、写时不需要()
var mc_a = mc.this_a
mc.this_b = 1

// 继承
class MyClass1 extends MyClass{
    constructor(a, b, c){
        super(a, b)           // 用super调用父类
        this.c = c
    }

    method(){
        return super.method() + this.c
    }
}
