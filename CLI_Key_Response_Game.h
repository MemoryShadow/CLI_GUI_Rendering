/***
 * @Date         : 2020-08-25 05:14:28
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-28 12:40:36
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

#include <stdio.h>
#include <termio.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
// 此函数用于关闭或打开终端按键回显(如果没有提交, 则会在终端中显示按键)
// 当state为1时, 关闭终端回显信息, 当为0时打开终端回显信息
int close_termios_echo(int state)
{
    struct termios tm;
    int fd = 0;
    static struct termios tm_old;
    static int back_tm = 0;
    if (back_tm == 0 && state == 1)
    {
        //保存现在的终端设置
        if (tcgetattr(fd, &tm) < 0)
            return -1;
        tm_old = tm;
        back_tm = 1;
        //更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
        cfmakeraw(&tm);
        //设置上更改之后的设置
        if (tcsetattr(fd, TCSANOW, &tm) < 0)
            return -1;
        return 0;
    }
    if (back_tm == 1 && state == 0)
    {
        //更改设置为最初的样子
        if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
            return -1;
        back_tm = 0;
        return 0;
    }
    return -1;
}

// 重新实现getch https://blog.csdn.net/gaopu12345/article/details/30467099
int getch(void)
{
    int close_termios_echo_status = close_termios_echo(1);
    int ch = '\0';
    ch = getchar();
    // 检查是否成功关闭回显, 如果成功关闭回显, 则在程序退出时打开回显
    if (close_termios_echo_status == 0)
        close_termios_echo(0);
    return ch;
}

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
 * @description: {
 * 此函数对按键进行判断，检查它是否为一个方向控制键
 * }
 * @param {
 * keySignal 传入的按键.
 * }
 * @return {
 * FunctionKeys 如果是一个控制信号，就返回控制信号，否则返回NO_FunctionKeys(0)
 * }
 *
 */
FunctionKeys isFunctionSignalKey(ControlSignal keySignal)
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
            Info = keySignal;
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
        if (kbhit())
        {
            if (getch() == '\133')
            {
                // 如果是控制键，就再次从输入缓冲区取读
                if (kbhit())
                    keySignal = getch();
                // 然后对这个值进行判定，并且返回指定的值
                switch (keySignal)
                {
                case 65:
                    Info = Up;
                    break;
                case 53:
                    Info = PageUp;
                    break;
                case 68:
                    Info = Left;
                    break;
                case 67:
                    Info = Right;
                    break;
                case 66:
                    Info = Down;
                    break;
                case 54:
                    Info = PageDown;
                    break;
                case 51:
                    Info = Delete;
                    break;

                default:
                    Info = keySignal;
                    break;
                }
            }
            else
            {
                Info = NO_FunctionKeys; // 如果输入缓冲区为空就忽略并报告这并非是一个真正的控制键
            }
        }
        else
        {
            Info = ESC; // 如果输入缓冲区为空就忽略并报告这真的是一个ESC
        }
    }
#endif
    return Info;
}

/***
 * @description: 此函数允许将普通按键也转换为指定的控制键信号，例如按下W会返回Up，同时并不会使原本的监控失去效果(可以完全代替isFunctionSignalKey)
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