/*
 * @Date: 2020-04-15 08:46:26
 * @LastEditors: MemoryShadow
 * @LastEditTime: 2020-08-24 23:44:41
 * @FilePath: \CLI_GUI_Rendering\Airplane war.c
 */

#define WINDOWS

#include "CLI_GUI_Rendering.h"

#if _WIN32
#include <conio.h>
#endif

#if __linux__
#include <termio.h>
// 重新实现getch https://blog.csdn.net/gaopu12345/article/details/30467099
int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;

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
#endif

int main(int argc, char const *argv[])
{
    // 在最开始创建主绘制层(窗口层)
    Window_layer *main_layer = new_Window_layer(20, 29);
    //边界图层
    struct Paint_layer *edge_layer = new_Paint_layer(main_layer);

    // 绘制周围边界
    for (unsigned index = 1; index < main_layer->width - 1; index++)
    {
        Write_Point(edge_layer, index, 0, '-');
        Write_Point(edge_layer, index, main_layer->height - 1, '-');
    }
    for (unsigned index = 1; index < main_layer->height - 1; index++)
    {
        Write_Point(edge_layer, 0, index, '|');
        Write_Point(edge_layer, main_layer->width - 1, index, '|');
    }
    // 游戏代码
    {
        // 飞机层
        struct Paint_layer *Aircraft_layer = new_Paint_layer(main_layer);
        // 绘制飞机
        Write_Point(Aircraft_layer, Aircraft_layer->width / 2 + 1, Aircraft_layer->height - 2, '*');
        Write_Point(Aircraft_layer, Aircraft_layer->width / 2, Aircraft_layer->height - 3, '*');
        Write_Point(Aircraft_layer, Aircraft_layer->width / 2, Aircraft_layer->height - 2, '*');
        Write_Point(Aircraft_layer, Aircraft_layer->width / 2 - 1, Aircraft_layer->height - 2, '*');
        // 储存飞机位置
        COORD Aircraft_Position = {
            Aircraft_layer->width / 2,
            Aircraft_layer->height - 3};
        // 创建一个层用于储存飞机子弹
        struct Paint_layer *Aircraft_Bullet_layer = new_Paint_layer(main_layer);
        CHAR ch = 0;
        CHAR *key_debug = NULL;
        // 消息循环
        while (1)
        {
            if (kbhit())
                ch = getch();
            switch (ch)
            {
            // 处理特殊信号
            case -32:
                continue;
            case 32:
                // 按下空格时在飞机前方绘制子弹
                Write_Point(Aircraft_Bullet_layer, Aircraft_Position.X, Aircraft_Position.Y - 1, '|');
                ch = '\0';
                key_debug = "空格";
                break;
            case 72:
            case 119:
                // 当飞机不在边界时才进行移动(上边界:2)
                if (Aircraft_Position.Y >= 2)
                {
                    layer_Move(Aircraft_layer, Up, 1);
                    // 更新飞机坐标值
                    Aircraft_Position.Y -= 1;
                }
                ch = '\0';
                key_debug = "向前信号";
                break;
            case 75:
            case 97:
                if (Aircraft_Position.X >= 3)
                {
                    layer_Move(Aircraft_layer, Left, 1);
                    Aircraft_Position.X -= 1;
                    printf("%d", Aircraft_Position.X);
                }
                ch = '\0';
                key_debug = "向左信号";
                break;
            case 77:
            case 100:
                if (Aircraft_Position.X <= Aircraft_layer->width - 4)
                {
                    layer_Move(Aircraft_layer, Right, 1);
                    Aircraft_Position.X += 1;
                }
                ch = '\0';
                key_debug = "向右信号";
                break;
            case 80:
            case 115:
                if (Aircraft_Position.Y <= Aircraft_layer->height - 4)
                {
                    layer_Move(Aircraft_layer, Down, 1);
                    Aircraft_Position.Y += 1;
                }
                ch = '\0';
                key_debug = "向后信号";
                break;
            default:
                ch = ch;
                break;
            }
            // 无论如何,子弹都会向前移动
            layer_Move(Aircraft_Bullet_layer, Up, 1);
            // 刷新界面
            WindowDraw(main_layer, 1);
            if (ch == 27)
            {
                printf("游戏已暂停:再次按下Esc退出游戏");
                if (getch() == 27)
                {
                    exit(0);
                }
            }

            ch != NULL ? printf("当前没有任何操作") : printf("%s\t\t\t\t\t", key_debug);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}
