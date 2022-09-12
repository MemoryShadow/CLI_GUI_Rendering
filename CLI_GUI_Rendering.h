/***
 * @Date         : 2020-04-14 21:41:50
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-09-08 13:50:40
 * @Description  : 一个用于在命令行中玩耍的GUI库,绘制逻辑和Adobe PhotoShop中的图层类似
 */

#include "CLI_Key_Response_Game.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#if _WIN32
#include <Windows.h>
#endif
// 在linux下重新实现一些依赖
#if __linux__

#include <unistd.h>

#if _UNICODE
typedef char_w CHAR;
#else

typedef char CHAR;
typedef short SHORT;

#endif
// 用于标记屏幕上的某个点. 实现摘自百度百科
typedef struct _COORD
{
    SHORT X; // horizontal coordinate
    SHORT Y; // vertical coordinate
} COORD;

#endif

// 用于记录层的选项
typedef enum
{
    // 用于标识此层的属性,1为窗口层,0为绘制层
    Attributes = 1, // (0000 0001)
    // "不再更新" 选项,设置为1将预渲染此层数据,并置于底层用于加快渲染,设置为0将渲染此层,默认为0
    NotUpdate = 2,  // (0000 0010)
    // "不要渲染" 选项,设置为1将不渲染此层数据,用于加快渲染,设置为0将渲染此层,默认为0
    NotRender = 4,  // (0000 0100)
} _Layer_Flag;

// 用于记录事件列表
typedef enum
{
    // 碰撞事件
    Collision = 1,  // (0000 0001)
} EventTypes;

typedef unsigned char Layer_Flag; // 储存一个8位的控制信号

// 一个绘制层的信息
typedef struct _Layer
{
    // 用于储存此层的起始位置
    struct
    {
        unsigned X;
        unsigned Y;
    } start;
    // 用于储存的层标志
    Layer_Flag flag;
    // 层的宽
    unsigned width;
    // 层的高
    unsigned height;
    struct
    {
        // 碰撞事件的指针
        // @param _Layer: 触发事件的层
        // @param char: 触发事件的值
        int (*onCollision)(Event);
    } event;
    // 记录每个点的内容
    CHAR **Data;
    // 为链表做的准备
    struct _Layer *Next;
} Window_layer, SmartObject_Paint_layer, Paint_layer;

// 事件信息
typedef struct
{
    // 事件类型
    EventTypes type;
    // 事件触发的层
    struct _Layer *layer;
    unsigned layer_index;
    // 事件触发的位置
    COORD position;
    // 事件触发的值
    struct {
        // 碰撞事件的值
        CHAR *old;
        CHAR *new;
    } P_value;
} Event;

// 函数签名
// * 创建一个层
struct _layer *new_layer(unsigned width, unsigned height);
// * 创建窗口层
Window_layer *new_Window_layer(unsigned width, unsigned height, Window_layer *Base);
// * 在指定窗口的最上层创建一个指定大小的绘制层,如果为0就继承窗口层的属性
Paint_layer *new_Paint_layer(Window_layer *Window, unsigned width, unsigned height);
// * 计算某个窗口绘制层数量
unsigned layer_length(const Window_layer *Window);
// * 设置指定层在它相对窗口0,0的位置(窗口层现在可以包裹在上一级窗口层中,被统一认为是绘制层,但是可以单独进行相对响应)
Paint_layer *setlayerStart(Paint_layer *layer, unsigned X, unsigned Y);
// * 删除某个指针指向的层(别忘记先剥离)
Paint_layer *delete_Paint_layer(Paint_layer *layer);
// * 通过索引取得某个层的指针
Paint_layer *layer_index(Window_layer *Window, unsigned index);
// * 将指定索引的层从窗口中剥离出来(拒绝剥离0)
Paint_layer *Remove_layer(Window_layer *Window, unsigned index);
// * 删除某个窗口(会删除其所有绘制层)
void delete_Window_layer(Window_layer *Window);
// * 窗口渲染
Window_layer *WindowRender(Window_layer *Window);
// * 窗口绘制
void WindowDraw(Window_layer *Window, int Convert);
// * 在指定的层中指定的位置,填充指定的字符(成功返回此层指针,失败返回NULL)
Paint_layer *Write_Point(Paint_layer *layer, unsigned x, unsigned y, CHAR Char);
// * 在指定的层中指定的位置,填充指定长度的字符串(成功返回此层指针,失败返回NULL)
Paint_layer *Write_Line(Paint_layer *layer, unsigned x, unsigned y, CHAR *Char, unsigned int length);
// * 在指定的层中获取指定位置的字符(成功返回此字符,失败..就返回\0好了)
CHAR Get_Point(Paint_layer *layer, unsigned x, unsigned y);
// * 移动一个层
Paint_layer *layer_Move(Paint_layer *layer, unsigned Direction, unsigned length);
// * 绑定指定层的指定事件, 返回已经被绑定的层
struct _Layer *EventBinding(struct _Layer *layer, EventTypes eventType, int (*callback)(Event));

/***
 * @description: 创建一个层
 * @param {
 * width 指定此层的宽度
 * height 指定此层的高度}
 * @return {
 * Window_layer * 返回此层的指针
 * }
 */
struct _layer *new_layer(unsigned width, unsigned height)
{
    // 分配内存给层
    Paint_layer *layer = (Paint_layer *)malloc(sizeof(Paint_layer));
    layer->Data = (CHAR **)malloc(sizeof(CHAR *) * height);
    // 进行初始化(顺便对二维数组内容进行界定)
    for (unsigned j = 0; j < height; j++)
    {
        layer->Data[j] = (CHAR *)malloc(sizeof(CHAR) * width);
        for (unsigned i = 0; i < width; i++)
        {
            // layer->Data[j][i] = '　';
            // layer->Data[j][i] = '\u3000';
            layer->Data[j][i] = '\0';
        }
    }
    // 处理层属性
    setlayerStart(layer, 0, 0);      // 设置一个层的默认位置在0,0的位置
    layer->flag = 0;                 // 默认情况下, 都是为0
    layer->event.onCollision = NULL; // 将指针设置为空
    layer->height = height;
    layer->width = width;
    layer->Next = NULL;
    return layer;
}

/**
 * @description: 创建窗口
 * @param {
 * width 指定此窗口的宽度
 * height 指定此窗口的高度
 * Base 指定父窗口层,如果无父窗口,设为NULL即可
 * }
 * @return: Window_layer * 返回此窗口的指针
 */
Window_layer *new_Window_layer(unsigned width, unsigned height, Window_layer *Base)
{
    // 创建一个新层
    Window_layer *Window = (Window_layer *)new_layer(width, height);
    // 标识这是一个窗口层
    Window->flag |= Attributes;
    Window->Next = NULL;
    // 对父窗口的绑定进行处理
    if (Base != NULL) // 只响应Base不为NULL的情况
    {
        // 取得父窗口的末尾窗口指针
        Window_layer *BaseWindow = layer_index(Base, layer_length(Base));
        // 然后将本数据附在此层后面
        Window->Next = BaseWindow->Next;
        BaseWindow->Next = Window;
    }

    return Window;
}

/**
 * @description: 在指定窗口的最上层创建绘制层
 * @param {
 * Window 要指定的父图层
 * width 设置新窗口的宽
 * height 设置新窗口的高
 * }
 * @return: Paint_layer 返回这个新建层的指针
 */
Paint_layer *new_Paint_layer(Window_layer *Window, unsigned width, unsigned height)
{
    // 创建一个新层
    Paint_layer *layer = (Paint_layer *)new_layer((width != 0 ? width : Window->width), (height != 0 ? height : Window->height));
    // 标识这是一个绘制层(这是默认的, 所以不用管)
    // 将层加入链表头
    layer->Next = Window->Next;
    Window->Next = layer;
    return layer;
}

/**
 * @description: 计算某个窗口绘制层数量
 * @param {
 * Window 要计算的窗口
 * }
 * @return: unsigned 返回层的数量(不是下标)
 */
unsigned layer_length(const Window_layer *Window)
{
    // 将指针内容迁移
    const Window_layer *Window_copy = Window;
    // 计算长度
    unsigned length = 0;
    // 只有在Next不为NULL，并且Attributes为0(绘制层)时才计数
    while ((Window_copy->Next != NULL) && (Window_copy->Next->flag & Attributes))
    {
        // 计数+1
        length++;
        // 将当前索引移动到下一个
        Window_copy = Window_copy->Next;
    }
    return length;
}

/***
 * @description: 设置指定层在它相对窗口0,0的位置(窗口层现在可以包裹在上一级窗口层中,被统一认为是绘制层,但是可以单独进行相对响应)
 * @param {
 * layer 要修改的层指针
 * X 要修改到X坐标的点
 * Y 要修改到Y坐标的点
 * }
 * @return {
 * Paint_layer 返回对应的层下标
 * }
 */
Paint_layer *setlayerStart(Paint_layer *layer, unsigned X, unsigned Y)
{
    layer->start.X = X;
    layer->start.Y = Y;
    return layer;
}

/***
 * @description: 设置指定层是否更新
 * @param {
 * layer 要修改的层指针
 * }
 * @return {
 * Paint_layer 返回对应的层下标
 * }
 */
Paint_layer *setNotRender(Paint_layer *layer, unsigned value)
{
    // 如果是1(true)则设置为不渲染
    if (value)
        layer->flag |= NotRender;
    else if (layer->flag & NotRender)
        layer->flag &= ~NotRender;

    return layer;
}

/**
 * @description:  移动一个层的起始位置
 * @param {
 * layer 要进行操作的层
 * Direction 要移动的方向,只响应MoveDirection数据
 * length 要移动的长度
 * }
 * @return: {Paint_layer *}
 */
Paint_layer *MovelayerStart(Paint_layer *layer, MoveDirection Direction, unsigned length)
{
    if ((Direction & Up) == Up)
    {
        // 高度减去步长
        layer->start.Y -= length;
    }
    if ((Direction & Down) == Down)
    {
        // 高度加上步长
        layer->start.Y += length;
    }
    if ((Direction & Left) == Left)
    {
        // 宽度减去步长
        layer->start.X -= length;
    }
    if ((Direction & Right) == Right)
    {
        // 宽度加上步长
        layer->start.X += length;
    }
    return layer;
}

/**
 * @description: 通过索引取得某个层的指针(如果越界下标将会返会NULL)
 * @param {
 * Window 要操作的窗口
 * index 层下标
 * }
 * @return: Paint_layer 返回对应的层下标
 */
Paint_layer *layer_index(Window_layer *Window, unsigned index)
{
    // 确认是否超过最大下标
    if (layer_length(Window) < index)
        return NULL;

    // 将指针内容迁移
    Window_layer *Window_copy = Window;
    // 如果碰到链末或者遇到指定索引值就弹出循环
    for (unsigned length = 0; (Window_copy->Next != NULL) && (length < index); length++)
    {
        // 将当前索引移动到下一个
        Window_copy = Window_copy->Next;
    }
    return Window_copy;
}

/**
 * @description: 将某个层从窗口中剥离出来(拒绝剥离0)
 * @param {
 * Window 要进行操作的主窗口
 * index 指定要剥离的层下标
 * }
 * @return: Paint_layer 返回已经剥离的层的指针
 */
Paint_layer *Remove_layer(Window_layer *Window, unsigned index)
{
    // 确认不超过最大下标或者下标不为0
    if ((index == 0) || (layer_length(Window) < index))
        return NULL;

    // 将指针内容迁移
    Window_layer *Window_copy = Window;
    Paint_layer *Previous_layer = NULL;
    // 如果碰到链末或者遇到指定索引值就弹出循环
    for (unsigned length = 0; (Window_copy->Next != NULL) && (length < index); length++)
    {
        // 在移动前备份上一个
        Previous_layer = Window_copy;
        // 将当前索引移动到下一个
        Window_copy = Window_copy->Next;
    }
    // 剥离出表
    Previous_layer->Next = Window_copy->Next;
    Window_copy->Next = NULL;
    return Window_copy;
}

/**
 * @description: 删除某个指针指向的层(要记得先剥离)
 * @param {
 * layer 要删除的层
 * }
 * @return: Paint_layer 这个层的控制指针
 */
Paint_layer *delete_Paint_layer(Paint_layer *layer)
{
    // 删除行数据
    for (unsigned j = 0; j < layer->height; j++)
    {
        free(layer->Data[j]);
    }
    // 删除Data行索引
    free(layer->Data);
    layer->Data = NULL;
    // 删除层对象
    free(layer);
    layer = NULL;
    return layer;
}

/**
 * @description: 删除某个窗口(会删除其所有绘制层)
 * @param {
 * Window 要进行操作的窗口层
 * }
 * @return: void
 */
void delete_Window_layer(Window_layer *Window)
{
    // 用于备份的层
    Paint_layer *Next_layer = Window->Next;
    // * 释放所有层
    while (Window != NULL)
    {
        // 备份Next
        Next_layer = Window->Next;
        // 释放当前层
        delete_Paint_layer(Window);
        // 将当前索引移动到下一个
        Window = Next_layer;
    }
    Window = NULL;
}

/***
 * @description: 从下向上渲染指定窗口中的所有绘制层, 其中层数越低显示优先级越高, 渲染后的内容将被储存在窗口层中(也可以通过碰撞事件来更改行为)
 * @param {
 * Window 要渲染的窗口层
 * }
 * @return {
 * Window_layer 返回此窗口层的指针
 * }
 */
Window_layer *WindowRender(Window_layer *Window)
{
    // * 渲染窗口
    // 绘制层指针
    Paint_layer *layer = Window->Next;
    // 创建用于储存偏移量的结构
    COORD Window_layer_start = {0, 0}, // 窗口层的起始位置
        CachePoint = {0, 0};           // 缓存的点
    // 记录当前层索引
    unsigned layer_index = 0;
    // 遍历绘制层列表 遍历条件:只要layer不为NULL
    while (layer != NULL)
    {
        // * 将内容渲染到主窗口
        // 检查是否设置“不要更新标记”
        if (layer->flag & NotRender)
        {
            // 将当前索引移动到下一个层
            layer = layer->Next;
            // 如果被设置,就跳过这个层
            continue;
        }

        // 检查当前窗口是否为窗口层
        if (layer->flag & Attributes)
        {
            // * 如果为窗口层
            // 就记录此子窗口层偏移量
            Window_layer_start.X = layer->start.X;
            Window_layer_start.Y = layer->start.Y;
            // 将当前索引移动到下一个层
            layer = layer->Next;
            // 并且跳出
            continue;
        }

        // 遍历此绘制层内容
        for (unsigned height = 0; height < layer->height; height++)
        {
            for (unsigned width = 0; width < layer->width; width++)
            {
                // 提前计算要渲染的位置
                CachePoint.Y = height + layer->start.Y + Window_layer_start.Y;
                CachePoint.X = width + layer->start.X + Window_layer_start.X;
                // 不渲染超出部分
                if ((CachePoint.Y >= 0) && (CachePoint.Y < Window->height) && (CachePoint.X >= 0) && (CachePoint.X < Window->width))
                {
                    // 这里检测渲染事件, 如果抵达触发条件就调用处理函数
                    //*触发条件: 函数不为null, 渲染目标位置非空, 并且要渲染的数据也为非空
                    if ((layer->event.onCollision != NULL) && (Window->Data[CachePoint.Y][CachePoint.X] != '\0') && (layer->Data[height][width] != '\0'))
                    {
                        // 调用该层对应处理函数
                        Event event = {
                            Collision, layer, layer_index,
                            (COORD){width, height},
                            {
                                &Window->Data[CachePoint.Y][CachePoint.X],
                                &layer->Data[height][width]
                            }
                        };
                        // layer->event.onCollision(layer, (COORD){width, height}, &Window->Data[CachePoint.Y][CachePoint.X]);
                        layer->event.onCollision(event);
                    }

                    // 只渲染有更改的空白部分
                    if ((Window->Data[CachePoint.Y][CachePoint.X] != layer->Data[height][width]) && (Window->Data[CachePoint.Y][CachePoint.X] == '\0'))
                    {
                        Window->Data[CachePoint.Y][CachePoint.X] = layer->Data[height][width];
                    }
                }
            }
        }
        // 将当前索引移动到下一个层
        layer = layer->Next;
        layer_index++;
    }
    return Window;
}

// 绘制指定窗口,选项Convert为非0时将会启动转换模式,将半角字符绘制为全角(无论如何都会转换空值为空格)
void WindowDraw(Window_layer *Window, int Convert)
{
    // 缓存的上一次打印
    static Window_layer *Cache_Window = NULL;
    // * 清空窗口层数据(这是上一次计算后的缓存)
    for (unsigned height = 0; height < Window->height; height++)
    {
        for (unsigned width = 0; width < Window->width; width++)
        {
            Write_Point(Window, width, height, '\0');
        }
    }
    // * 渲染窗口
    WindowRender(Window);
    // 如果备份内容为NULL就执行初始化
    if (Cache_Window == NULL)
    {
        Cache_Window = new_Window_layer(Window->width, Window->height, NULL);
    }
    else if ((Cache_Window->width != Window->width) || (Cache_Window->height != Window->height))
    { // 兼容化备份区域 如果宽高不一样(会先删除之前的窗口)
        // * 创建新窗口
        Window_layer *T_Compatible_Window = new_Window_layer(Window->width, Window->height, NULL);
        // * 复制内容
        for (unsigned width = 0; width < Window->width; width++)
        {
            for (unsigned height = 0; height < Window->height; height++)
            {
                Write_Point(T_Compatible_Window, width, height, Get_Point(Cache_Window, width, height));
            }
        }
        // * 删除旧缓存窗口,内容并且把新窗口内容交给旧窗口(缓存窗口只有一层)
        delete_Paint_layer(Cache_Window);
        Cache_Window = T_Compatible_Window;
    }
    // * 绘制窗口 在绘制窗口时会顺便备份窗口
    for (unsigned height = 0; height < Window->height; height++)
    {
        for (unsigned width = 0; width < Window->width; width++)
        {
            // 把将要写入的数据与备份的数据做比对,不一样才写入(此处通过API操作写入)  放行\0
            if (!Get_Point(Window, width, height) || (Get_Point(Window, width, height) != Get_Point(Cache_Window, width, height)))
            {
                // 根据Convert的值来到达下一个值的位置
                // 根据start的值计算偏移量
                COORD pos = {(Convert ? ((width + Window->start.X) * 2) : (width + Window->start.X)), height + Window->start.Y};
// 如果不一样,就移动光标至指定位置,以下是Windows下的代码
#ifdef _WIN32
                // 获取窗口句柄
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                // 设置属性
                CONSOLE_CURSOR_INFO cinfo = {1, 0};
                // 移动光标
                SetConsoleCursorPosition(hOut, pos);
                // 隐藏光标
                SetConsoleCursorInfo(hOut, &cinfo);
#endif
// 以下是linux下的代码
#if __linux__
                // 移动光标
                printf("\033[%d;%dH", pos.Y + (Convert ? 2 : 1), pos.X + (Convert ? 2 : 1));
                // 隐藏光标
                printf("\033[?25l");
#endif
                if (Convert)
                // 如果转换选项被打开,就执行这里的逻辑
                {
                    switch (Window->Data[height][width])
                    {
                    case '\0':
                        printf("\u3000");
                        break;
                    case ' ':
                        printf("\u3000");
                        break;
                    case '*':
                        printf("\uff0a");
                        break;
                    case '-':
                        printf("\uff0d");
                        break;
                    case '|':
                        printf("\uff5c");
                        break;
                    default:
                        printf("%c", Window->Data[height][width]);
                        fflush(stdout);
                        break;
                    }
                }
                else
                // 否则就只转换空值
                {
                    if (Window->Data[height][width] == '\0')
                    {
                        printf("\u3000");
                    }
                    else
                    {
                        printf("%c", Window->Data[height][width]);
                    }
                }
                // 如果数据不同就备份渲染后的数据
                Write_Point(Cache_Window, width, height, Get_Point(Window, width, height));
            }
        }
        printf("\n");
#if __linux__
        // linux下运行的太快啦!在这里设置上休眠周期(每页休眠一段时间)
        usleep(2500);
#endif
    }
}

/**
 * @description: 在指定的层中指定的位置,填充指定的字符(成功返回此层指针,失败返回NULL)
 * @param {
 * layer 要进行操作的层
 * x X轴
 * y Y轴
 * Char 要填充的内容
 * }
 * @return: Paint_layer *
 */
Paint_layer *Write_Point(Paint_layer *layer, unsigned x, unsigned y, CHAR Char)
{
    // 检查是否越界
    if ((layer->height <= y) || (layer->width <= x))
    {
        return NULL;
    }

    // 写入数据
    layer->Data[y][x] = Char;
    return layer;
}

/**
 * @description: 在指定的层中指定的位置起始填充指定长度的字符串(成功返回此层指针,失败返回NULL)
 * @param {
 * layer 要进行操作的层
 * x X轴起始坐标
 * y Y轴起始坐标
 * Char* 要填充的内容
 * length 这条字符串的长度(避免有区域需要填0无法分辨的情况)
 * }
 * @return: Paint_layer *
 */
Paint_layer *Write_Line(Paint_layer *layer, unsigned x, unsigned y, CHAR *Char, unsigned int length)
{
    // 检查是否越界(这里最后那个条目之所以可以不需要等于, 是因为这里是填充,不是写入,所以不需要考虑\0)
    if ((Char == NULL) || (layer->height <= y) || (layer->width <= x) || (layer->width < x + length))
        return NULL;

    // 写入数据
    for (unsigned int i = 0; i < length; i++)
    {
        layer->Data[y][x + i] = Char[i];
    }
    return layer;
}

/**
 * @description: 在指定的层中获取指定位置的字符(成功返回此字符,失败..就返回\0好了)
 * @param {
 * layer 要进行操作的层
 * x X轴
 * y Y轴
 * }
 * @return: CHAR
 */
CHAR Get_Point(Paint_layer *layer, unsigned x, unsigned y)
{
    // 检查是否越界
    if ((layer->height < y) || (layer->width < x))
    {
        return '\0';
    }

    // 写入数据
    char Debug = layer->Data[y][x];
    return (Debug);
}

/**
 * @description:  移动一个层的内容
 * @param {
 * layer 要进行操作的层
 * Direction 要移动的方向,只响应MoveDirection数据
 * length 要移动的长度
 * }
 * @return: {Paint_layer *}
 */
Paint_layer *layer_Move(Paint_layer *layer, MoveDirection Direction, unsigned length)
{
    if ((Direction & Up) == Up)
    {
        // 外层套上宽度
        for (unsigned width = 0; width < layer->width; width++)
        {
            // * 循环取得要被复制的区域length~layer->height
            for (unsigned height = 0 + length; height < (layer->height); height++)
            {
                // 只有两个数据不一样才迁移
                if (layer->Data[height - length][width] != layer->Data[height][width])
                {
                    Write_Point(layer, width, height - length, layer->Data[height][width]);
                }

                // 判断此时取读位置高是否大于最大值减步长,如果小于就顺便填0
                if (height >= (layer->height - length))
                {
                    Write_Point(layer, width, height, '\0');
                }
            }
        }
    }
    if ((Direction & Down) == Down)
    {
        // 外层套上宽度
        for (unsigned width = 0; width < layer->width; width++)
        {
            for (unsigned height = layer->height - 1 - length; (height + length) >= length; height--)
            {
                // 只有两个数据不一样才迁移
                if (layer->Data[height + length][width] != layer->Data[height][width])
                {
                    // 迁移数据
                    Write_Point(layer, width, height + length, layer->Data[height][width]);
                }

                // 判断此时高是否小于length,如果小于就顺便填0
                if (height < length)
                {
                    Write_Point(layer, width, height, '\0');
                }
            }
        }
    }
    if ((Direction & Left) == Left)
    {
        // 外层套上高度
        for (unsigned height = 0; height < layer->height; height++)
        {
            // * 循环取得要被复制的区域length~layer->height
            for (unsigned width = 0 + length; width < (layer->width); width++)
            {
                // 只有两个数据不一样才迁移
                if (layer->Data[height][width - length] != layer->Data[height][width])
                {
                    Write_Point(layer, width - length, height, layer->Data[height][width]);
                }

                // 判断此时取读位置高是否大于最大值减步长,如果小于就顺便填0
                if (width >= (layer->width - length))
                {
                    Write_Point(layer, width, height, '\0');
                }
            }
        }
    }
    if ((Direction & Right) == Right)
    {
        // 外层套上宽度
        for (unsigned height = 0; height < layer->height; height++)
        {
            for (unsigned width = layer->width - 1 - length; (width + length) >= length; width--)
            {
                // 只有两个数据不一样才迁移
                if (layer->Data[height][width + length] != layer->Data[height][width])
                {
                    // 迁移数据
                    Write_Point(layer, width + length, height, layer->Data[height][width]);
                }

                // 判断此时高是否小于length,如果小于就顺便填0
                if (width < length)
                {
                    Write_Point(layer, width, height, '\0');
                }
            }
        }
    }
    return layer;
}

/**
 * @description: {
 * 清除屏幕(兼容多系统的)
 * }
 * @param {
 * void
 * }
 * @return:{
 * void
 * }
 */
void clear_screen(void)
{
#if _WIN32
    system("cls");
#endif
#if __linux__
    printf("\033[2J");
#endif
}

/**
 * @description: {
 * 绑定指定层的指定事件, 返回已经被绑定的层
 * }
 * @param {
 * layer 要绑定的层
 * event 要绑定的事件
 * callback 要绑定的回调函数
 * }
 * @return:{
 * _layer 返回已经被绑定的层
 * }
 */

struct _Layer *EventBinding(struct _Layer *layer, EventTypes eventType, int (*callback)(Event))
{
    switch (eventType)
    {
    case Collision:
        layer->event.onCollision = callback;
        break;
    }

    return layer;
}