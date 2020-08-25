/*
 * @Date: 2020-04-15 08:46:26
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-25 14:43:23
 * @FilePath: \CLI_GUI_Rendering\Airplane war.c
 */

#include "CLI_GUI_Rendering.h"

#include <stdlib.h>

#if _WIN32
#include <conio.h>
#endif

#if __linux__

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
            case 'A':
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
            case 'D':
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
            case 'C':
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
            case 'B':
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
                // 在linux下，特殊控制键是以ESC(27)开头的字符串，所以要在这里对它进行解析
                if (kbhit())
                {
                    //检查是否为其他控制建
                    if (getch() == 91)
                    {
                        // 如果为控制按键,就跳出循环,将权限交给上面的控制代码
                        // printf("%d\n\n", getch());
                        continue;
                    }
                    else
                    {
                        // 如果不为其他控制按键,就提示
                        printf("游戏已暂停:再次按下Esc退出游戏");
                        if (getch() == 27)
                        {
                            exit(0);
                        }
                    }
                }
            }

            ch != '\0' ? printf("当前没有任何操作") : printf("%s\t\t\t\t\t", key_debug);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}
