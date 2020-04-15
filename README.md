# CLI GUI Rendering

|Author|MemoryShadow|
|---|---
|E-mail|MemoryShadow@outlook.com|

## 目录

* [作用](#作用 "点击前往")
* [用法](#用法示例 "点击前往")
* [接口](#接口与调用 "点击前往")

### 作用

命令行中界面的多图层渲染,就像Adobe PhotoShop 中的图层一样对各个图层进行单独的管理,移动.您只需要关注算法而不需要去为简单的测试环境思考界面

### 用法示例

我们已经为您准备好了一些基础示例:

* [弹跳球](./Bouncing%20ball.cpp "点击前往")
* [飞机大战(尚未完成)](./Airplane%20war.cpp "点击前往")

### 接口与调用

`此项正在编辑`

在此处展示这个头文件的相关函数

#### 类型

这个头文件中包含了以下几个类型

|类型名|原始类型|作用|
|---|---|---|
|[Paint_layer](#Paint_layer "点击前往")|struct|中文称谓(在注释中):绘制层 其中储存了有关于本层的绘制信息,可以在相关控制函数的帮助下移动或者绘制新的信息|
|[Window_layer](#Window_layer "点击前往")|struct|中文称谓(在注释中):窗口层 其中标记了一个"界面",不可储存数据,但是可以拥有多个绘制层|
|[MoveDirection](#MoveDirection "点击前往")|enum|中文称谓(在注释中):移动方向 在使用[layer_Move](#layer_Move "点击前往")时可以作为第二参数填入,不同方向可以用\|符号进行隔开同时提交|

##### Paint_layer

原始类型:

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

##### Window_layer

原始类型:

```C
typedef struct Paint_layer Window_layer;
```

##### MoveDirection

原始类型:

```C
enum MoveDirection
{
    Up = 1,
    Down = 2,
    Left = 4,
    Right = 8
};
```

#### 函数

这个头文件中包含了以下几个函数

|函数名|参数列表|返回类型|具体作用|使用示例|
|---|---|---|---|---|
|new_Window_layer|unsigned width<br />unsigned height|Window_layer *|创建一个Window_layer结构,返回新建的指针,您可以理解为面向对象中的构造器|`Window_layer *main_layer = new_Window_layer(20, 15);`|
|new_Paint_layer|Window_layer *Window|Paint_layer *|在指定窗口的顶层创建一个绘制层|` Paint_layer *edge_layer = new_Paint_layer(main_layer);`|
|layer_length|const Window_layer *Window|unsigned|计算某个窗口层的绘制层数量|`示例代码见详解`|
