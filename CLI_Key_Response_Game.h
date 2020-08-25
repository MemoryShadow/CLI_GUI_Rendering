/*** 
 * @Date         : 2020-08-25 05:14:28
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-25 16:44:10
 * @Description  : 一个用于响应玩家按键的工具,它致力于将按键转化为游戏中的按键信号,便于处理
 */

// 包含了控制信号的结构
typedef enum
{
    NO_FunctionKeys = 0,
    Up = 1,    // (0001)
    Down = 2,  // (0010)
    Left = 4,  // (0100)
    Right = 8, // (1000)
    ESC = 27
} MoveDirection,
    FunctionKeys;

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
 */
FunctionKeys isFunctionSignalKey(unsigned int keySignal)
{
    FunctionKeys Info = NO_FunctionKeys;
#if _WIN32
    // 首先检查是否为控制键
    if ((Info == NO_FunctionKeys) && ((keySignal == 0) || (keySignal == 0xE0)))
    {
        // 如果是控制键，就再次从输入缓冲区取读
        keySignal = getch();
        // 然后对这个值进行判定，并且返回指定的值
        switch (keySignal)
        {
        case 72:
            Info = Up;
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
