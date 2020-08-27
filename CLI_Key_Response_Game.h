/*** 
 * @Date         : 2020-08-25 05:14:28
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-27 11:54:35
 * @Description  : 一个用于响应玩家按键的工具,它致力于将按键转化为游戏中的按键信号,便于处理
 */

// 包含了控制信号的结构(最大支持32位,但是建议大小在一个字节8位)
typedef enum
{
    NO_FunctionKeys = 0, // (0000 0000)
    Up = 1,              // (0000 0001)
    Down = 2,            // (0000 0010)
    Left = 4,            // (0000 0100)
    Right = 8,           // (0000 1000)
    Delete = 16,         // (0001 0000)
    PageUp = 32,         // (0010 0000)
    PageDown = 64,       // (0100 0000)
    ESC = 128            // (1000 0000)
} MoveDirection,
    FunctionKeys;

typedef unsigned char ControlSignal; // 储存一个8位的控制信号

// typedef struct {
// };

#if _WIN32
#include <conio.h>
#endif

#if __linux__

#include <termio.h>
// 重新实现getch https://blog.csdn.net/gaopu12345/article/details/30467099
int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch = '\0';

    if (tcgetattr(fd, &tm) < 0)
    { //保存现在的终端设置
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm); //更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(fd, TCSANOW, &tm) < 0)
    { //设置上更改之后的设置
        return -1;
    }

    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
    { //更改设置为最初的样子
        return -1;
    }

    return ch;
}

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
// kbhit的linux再实现, https://blog.csdn.net/dongshibo12/article/details/76208482?utm_source=blogxgwz0
int kbhit(void)
{
    struct termios oldTermios, newt;
    int ch;
    int oldFcntl;
    tcgetattr(STDIN_FILENO, &oldTermios);
    newt = oldTermios;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // 这里的F_GETFL,F_SETFL,O_NONBLOCK都能找到定义,但是不知道为什么一直报错
    oldFcntl = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldFcntl | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
    fcntl(STDIN_FILENO, F_SETFL, oldFcntl);
    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
#endif

/*** 
 * @Description: 此函数对按键进行判断，检查它是否为一个方向控制键
 * @param {
 * keySignal 传入的按键.
 * } 
 * @return {
 * FunctionKeys 如果是一个控制信号，就返回控制信号，否则返回NO_FunctionKeys(0)
 * } 
 * 
 */
FunctionKeys isFunctionSignalKey(char keySignal)
{
    FunctionKeys Info = NO_FunctionKeys;
#if _WIN32
    // 首先检查是否为控制键
    if ((Info == NO_FunctionKeys) && ((keySignal == 0) || (keySignal == 0xE0)))
    {
        // 如果是控制键，就再次从输入缓冲区取读
        if (kbhit())
            keySignal = getch();
        else
            return NO_FunctionKeys; // 如果输入缓冲区为空就忽略并报告这并非是一个真正的控制键
        // 然后对这个值进行判定，并且返回指定的值
        switch (keySignal)
        {
        case 72:
            Info = Up;
            break;
        case 73:
            Info = PageUp;
            break;
        case 75:
            Info = Left;
            break;
        case 77:
            Info = Right;
            break;
        case 80:
            Info = Down;
            break;
        case 81:
            Info = PageDown;
            break;
        case 83:
            Info = Delete;
            break;

        default:
            return keySignal;
            break;
        }
    }
    // 然后检查是否为ESC
    if ((Info == NO_FunctionKeys) && (keySignal == 27))
        Info = ESC;
#endif

#if __linux__
    // 首先检查是否为控制键
    if (keySignal == 27)
    {
        // 如果是控制键
    }
#endif
    return Info;
}

/*** 
 * @Description: 此函数允许将普通按键也转换为指定的控制键信号，例如按下W会返回Up，同时并不会使原本的监控失去效果(可以完全代替isFunctionSignalKey)
 * @param {
 * FunctionSignal 等待转换的控制信号,可以使用|提交多个,但是Key的顺序始终是按照FunctionKeys的顺序进行排列
 * Key* 要监视的按键列表,按照FunctionKeys的顺序进行排列,不会响应FunctionSignal中为0的项
 * returnKey 用户按下的按键，直接填写getch()就好
 * } 
 * @return {
 * FunctionKeys 如果是一个控制信号，就返回控制信号，否则返回NO_FunctionKeys(0)
 * } 
 */
FunctionKeys isFunctionSignalKeyPlus(ControlSignal FunctionSignal, char *Key, char returnKey)
{
    FunctionKeys Info = NO_FunctionKeys; // 要返回的信息
    int FunctionSignal_Number = 0;       // 用于记录FunctionSignal最大设置位数的索引

    // 调用控制键检查函数,查看是否本身为一个控制键,如果本身是一个控制键就按照控制键的来,否则才进行兼容性处理
    Info = isFunctionSignalKey(returnKey);
    if (Info == NO_FunctionKeys)
    {
        // 扫描并且标记位数  0x80 = 1000 0000   从1出发,坐等溢位变成0,然后就可以确认循环结束了
        for (ControlSignal scanningMask = 1; scanningMask != 0; scanningMask <<= 1) // 循环检查每一位的值
        {
            // 检查该位是否被设置为1,如果被设置就与相匹配的位置进行判断
            if (FunctionSignal & scanningMask)
            {
                // 如果符合指定位的要求
                if (returnKey == Key[FunctionSignal_Number])
                {
                    // 就设置Info为当前掩码
                    Info = scanningMask;
                    // 并且跳出循环
                    break;
                }
                // 移动索引
                FunctionSignal_Number++;
            }
        }
    }

    // 返回对应的信号
    return Info;
}