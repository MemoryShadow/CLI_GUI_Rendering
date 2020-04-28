/*
 * @Date: 2020-04-14 21:41:50
 * @LastEditors: MemoryShadow
 * @LastEditTime: 2020-04-28 13:39:16
 * @FilePath: \CLI_GUI_Rendering\CLI_GUI_Rendering.h
 */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>

// 一个绘制层的信息
typedef struct Paint_layer
{
    // 层的宽
    unsigned width;
    // 层的高
    unsigned height;
    // 记录每个点的内容
    CHAR **Data;
    // 为链表做的准备
    struct Paint_layer *Next;
} Window_layer;

enum MoveDirection
{
    Up = 1,
    Down = 2,
    Left = 4,
    Right = 8
};

/* 
* 绘制层规则很简单
* 可以通过四个信号控制绘制层向四个方向移动
* 移动过程中会将超出的部分截断(数据会丢失),而新的地方将会填充空白
* 数字越小的绘制层将会越在上层(即处理后续层将只会填充空区域)
*/

// 函数签名
// * 创建窗口
Window_layer *new_Window_layer(unsigned width, unsigned height);
// * 在指定窗口的最上层创建绘制层
struct Paint_layer *new_Paint_layer(Window_layer *Window);
// * 计算某个窗口绘制层数量
unsigned layer_length(const Window_layer *Window);
// * 删除某个指针指向的层(别忘记先剥离)
struct Paint_layer *delete_Paint_layer(struct Paint_layer *layer);
// * 通过索引取得某个层的指针
struct Paint_layer *layer_index(Window_layer *Window, unsigned index);
// * 将某个层从窗口中剥离出来(拒绝剥离0)
struct Paint_layer *Remove_layer(Window_layer *Window, unsigned index);
// * 删除某个窗口(会删除其所有绘制层)
void delete_Window_layer(Window_layer *Window);
// * 窗口绘制
void WindowDraw(Window_layer *Window, int Convert);
// * 在指定的层中指定的位置,填充指定的字符(成功返回此层指针,失败返回NULL)
struct Paint_layer *Write_Point(struct Paint_layer *layer, unsigned x, unsigned y, CHAR Char);
// * 在指定的层中获取指定位置的字符(成功返回此字符,失败..就返回\0好了)
CHAR Get_Point(struct Paint_layer *layer, unsigned x, unsigned y);
// * 移动一个层
struct Paint_layer *layer_Move(struct Paint_layer *layer, unsigned Direction, unsigned length);

/**
 * @description: 创建窗口
 * @param {
 * width 指定此窗口的宽度
 * height 指定此窗口的高度
 * } 
 * @return: Window_layer * 返回此窗口的指针
 */
Window_layer *new_Window_layer(unsigned width, unsigned height)
{
    // 分配内存给层
    Window_layer *Window = (Window_layer *)malloc(sizeof(Window_layer));
    Window->Data = (CHAR **)malloc(sizeof(CHAR *) * height);
    // 进行初始化(顺便对二维数组内容进行界定)
    for (unsigned j = 0; j < height; j++)
    {
        Window->Data[j] = (CHAR *)malloc(sizeof(CHAR) * width);
        for (unsigned i = 0; i < width; i++)
        {
            // Window->Data[j][i] = '　';
            // Window->Data[j][i] = '\u3000';
            Window->Data[j][i] = '\0';
        }
    }
    Window->height = height;
    Window->width = width;
    Window->Next = NULL;
    return Window;
}

/**
 * @description: 在指定窗口的最上层创建绘制层
 * @param {
 * Window 要指定的父图层
 * } 
 * @return: Paint_layer 返回这个新建层的指针
 */
struct Paint_layer *new_Paint_layer(Window_layer *Window)
{
    // 将窗口初始化转换为一个层
    struct Paint_layer *layer = (struct Paint_layer *)new_Window_layer(Window->width, Window->height);
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
    // 检查Next是不是NULL,如果是就马上返回,如果不是就计数+1并移动到下一个
    while (Window_copy->Next != NULL)
    {
        // 计数+1
        length++;
        // 将当前索引移动到下一个
        Window_copy = Window_copy->Next;
    }
    return length;
}

/**
 * @description: 通过索引取得某个层的指针(如果越界下标将会返会NULL)
 * @param {
 * Window 要操作的窗口
 * index 层下标
 * } 
 * @return: Paint_layer 返回对应的层下标
 */
struct Paint_layer *layer_index(Window_layer *Window, unsigned index)
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
struct Paint_layer *Remove_layer(Window_layer *Window, unsigned index)
{
    // 确认不超过最大下标或者下标不为0
    if ((index == 0) || (layer_length(Window) < index))
        return NULL;

    // 将指针内容迁移
    Window_layer *Window_copy = Window;
    struct Paint_layer *Previous_layer = NULL;
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
struct Paint_layer *delete_Paint_layer(struct Paint_layer *layer)
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
    struct Paint_layer *Next_layer = Window->Next;
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

// TODO 准备优化 优化思路是对内容进行缓存,使用OS API对于写入的点进行处理
// 绘制指定窗口,选项Convert为非0时将会启动转换模式,将半角字符绘制为全角(无论如何都会转换空值)
void WindowDraw(Window_layer *Window, int Convert)
{
    // 缓存的上一次打印
    static Window_layer *Cache_Window = NULL;
    // * 清空窗口
    for (unsigned height = 0; height < Window->height; height++)
    {
        for (unsigned width = 0; width < Window->width; width++)
        {
            Write_Point(Window, width, height, '\0');
        }
    }
    // * 渲染窗口
    // 绘制层指针
    struct Paint_layer *layer = Window->Next;
    // 遍历绘制层列表
    while (layer != NULL)
    {
        // * 将内容渲染到主窗口
        // 遍历内容
        for (unsigned height = 0; height < Window->height; height++)
        {
            for (unsigned width = 0; width < Window->width; width++)
            {
                // 只填充窗口空白部分
                if (Window->Data[height][width] == '\0')
                {
                    Window->Data[height][width] = layer->Data[height][width];
                }
            }
        }
        // 将当前索引移动到下一个
        layer = layer->Next;
    }
    // 如果备份内容为NULL就执行初始化
    if (Cache_Window == NULL)
    {
        Cache_Window = new_Window_layer(Window->width, Window->height);
    }
    else if ((Cache_Window->width != Window->width) || (Cache_Window->height != Window->height))
    { // 兼容化备份区域 如果宽高不一样(会先删除之前的窗口)
        // * 创建新窗口
        Window_layer *T_Compatible_Window = new_Window_layer(Window->width, Window->height);
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
// 如果不一样,就移动光标至指定位置,以下是Windows下的代码
#ifdef WINDOWS
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                COORD pos = {(Convert ? width * 2 : width), height};
                CONSOLE_CURSOR_INFO cinfo = {1, 0};
                // 移动光标
                SetConsoleCursorPosition(hOut, pos);
                // 隐藏光标
                SetConsoleCursorInfo(hOut, &cinfo);
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
struct Paint_layer *Write_Point(struct Paint_layer *layer, unsigned x, unsigned y, CHAR Char)
{
    // 检查是否越界
    if ((layer->height < y) || (layer->width < x))
    {
        return NULL;
    }

    // 写入数据
    layer->Data[y][x] = Char;
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
CHAR Get_Point(struct Paint_layer *layer, unsigned x, unsigned y)
{
    // 检查是否越界
    if ((layer->height < y) || (layer->width < x))
    {
        return '\0';
    }

    // 写入数据

    return (layer->Data[y][x]);
}

/** 
 * @description:  移动一个层
 * @param {
 * layer 要进行操作的层
 * Direction 要移动的方向,只响应MoveDirection数据
 * length 要移动的长度
 * } 
 * @return: Paint_layer *
 */
struct Paint_layer *layer_Move(struct Paint_layer *layer, unsigned Direction, unsigned length)
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