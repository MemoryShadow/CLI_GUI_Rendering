# CLI GUI Rendering

|Author|MemoryShadow|
|---|---
|E-mail|MemoryShadow@outlook.com|

## 目录

* [作用](#作用 "点击前往")
* [用法](#用法示例 "点击前往")
* [接口](#接口与调用 "点击前往")

### 作用

命令行中界面的多图层渲染,就像Adobe Photoshop 中的图层一样对各个图层进行单独的管理,移动.您只需要关注算法而不需要去为简单的测试环境思考界面

### 用法示例

我们已经为您准备好了一些基础示例:

* [弹跳球](./Bouncing%20ball.cpp "点击前往")
* [飞机大战(尚未完成)](./Airplane%20war.cpp "点击前往")

### 接口与调用

`此项正在编辑`

在此处展示这个头文件的相关函数与相关类型

#### 类型

这个头文件中包含了以下几个类型

|类型名|原始类型|作用|
|---|---|---|
|[Paint_layer](#Paint_layer "点击访问详解")|struct|中文称谓(在注释中):绘制层 其中储存了有关于本层的绘制信息,可以在相关控制函数的帮助下移动或者绘制新的信息|
|[Window_layer](#Window_layer "点击访问详解")|struct|中文称谓(在注释中):窗口层 其中标记了一个"界面",不可储存数据,但是可以拥有多个绘制层|
|[MoveDirection](#MoveDirection "点击访问详解")|enum|中文称谓(在注释中):移动方向 在使用[layer_Move](#layer_Move "点击前往")时可以作为第二参数填入,不同方向可以用\|符号进行隔开同时提交|

##### Paint_layer

<details>
<summary>声明结构</summary>

```C
struct Paint_layer
{
    // 层的宽
    unsigned width;
    // 层的高
    unsigned height;
    // 记录每个点的内容
    CHAR **Data;
    // 为链表做的准备
    Paint_layer *Next;
};
```

</details>

##### Window_layer

<details>
<summary>声明结构</summary>

```C
typedef struct Paint_layer Window_layer;
```

</details>

##### MoveDirection

<details>
<summary>声明结构</summary>

```C
enum MoveDirection
{
    Up = 1,
    Down = 2,
    Left = 4,
    Right = 8
};
```

</details>

#### 函数

这个头文件中包含了以下几个函数

|函数名|参数列表|返回类型|具体作用|使用示例|
|---|---|---|---|---|
|[new_Window_layer](#new_Window_layer "点击访问详解")|`unsigned width`<br />`unsigned height`|Window_layer *|创建一个Window_layer结构,返回新建的指针`您可以理解为面向对象语言中的new操作符`|`Window_layer *main_layer = new_Window_layer(20, 15);`|
|[new_Paint_layer](#new_Paint_layer "点击访问详解")|`Window_layer *Window`|Paint_layer *|在指定窗口的顶层创建一个绘制层`您可以理解为面向对象语言中的new操作符`|`Paint_layer *edge_layer = new_Paint_layer(main_layer);`|
|[layer_length](#layer_length "点击访问详解")|`const Window_layer *Window`|unsigned|计算某个窗口层的绘制层数量|`示例代码见详解`|
|[Remove_layer](#Remove_layer "点击访问详解")|`Window_layer *Window`<br />`unsigned index`|Paint_layer *|将某个绘制层从所属的窗口层中剥离出来(会拒绝剥离0)|`Remove_layer(main_layer,1)`|
|[delete_Paint_layer](#delete_Paint_layer "点击访问详解")|`Paint_layer *layer`|Paint_layer *|释放某个窗口层的内容,`您可以理解为面向对象语言中的delete操作符`(别忘记先将其从窗口层中剥离)|`delete_Paint_layer(Window)`|
|[delete_Window_layer](#delete_Window_layer "点击访问详解")|`Window_layer *Window`|void|释放某个窗口层(会删除其所有绘制层)`您可以理解为面向对象语言中的delete操作符`|`delete_Window_layer(main_layer)`|
|[layer_index](#layer_index "点击访问详解")|`Window_layer *Window`<br />`unsigned index`|Paint_layer *|取得某个窗口层中指定索引的绘制层指针|`layer_index(main_layer,1)`|
|[Write_Point](#Write_Point "点击访问详解")|`Paint_layer *layer`<br />`unsigned x`<br />`unsigned y`<br />`CHAR Char`|Paint_layer *|在指定的层中指定的位置,填充指定的字符(成功返回此层指针,失败返回NULL)|`示例代码见详解`|
|[Get_Point](#Get_Point "点击访问详解")|`Paint_layer *layer`<br />`unsigned x`<br />`unsigned y`|CHAR|在指定的层中获取指定位置的字符(成功返回此字符,失败..就返回\0好了)|`示例代码见详解`|
|[WindowDraw](#WindowDraw "点击访问详解")|`Window_layer *Window`<br />`int Convert = 0`|void|绘制指定的窗口,参数Convert控制是否对于特定的半角字符转换为全角进行渲染|`WindowDraw(main_layer)`|
|[layer_Move](#layer_Move "点击访问详解")|`Paint_layer *layer`<br />`unsigned Direction`<br />`unsigned length`|Paint_layer *|将一个绘制层按照指定的方向移动指定的步长|`layer_Move(layer_length(main_layer,1),Up,1)`|

****

##### new_Window_layer

函数原型:

```C
Window_layer *new_Window_layer(unsigned width, unsigned height);
```

对于它的两个参数:

`unsigned width`:标记了这个窗口层的最大宽度

`unsigned height`:标记了这个窗口层的最大高度

创建一个Window_layer结构,返回新建的指针

`您可以理解为面向对象语言中的new操作符`

示例代码:

```C
Window_layer *main_layer = new_Window_layer(20, 15);
```

它看起来像极了[C++中的new语法](#参考资料\:C\+\+中的new语法 "点击查看")对吗,是的,这就是沿用了C++的语法,但是这个工具目标是在C下运行的,所以千万不要忽略了new后面的下划线

<details>
<summary>参考资料:C++中的new语法</summary>

###### 参考资料:C++中的new语法

上面所提到的,沿用了C++的语法,此处是参考,对于尚未接触面向对象语言,或者接触过但是尚未接触C++语言的开发者,可能这部分资料会对你有一些帮助.

```C++
Window_layer *main_layer = new Window_layer(20, 15);
```

这段代码将会调用类型Window_layer的构造器`Window_layer::Window_layer(unsigned,unsigned)`新增一个Window_layer类型的实例,这个实例的指针会被`Window_layer的构造器`返回
并且放到`Window_layer *main_layer`所声明的Window_layer类型指针main_layer中去.

如果想要了解更多,请参阅C++文档

</details>
