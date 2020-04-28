/*
 * @Date: 2020-04-15 08:46:26
 * @LastEditors: MemoryShadow
 * @LastEditTime: 2020-04-28 13:46:26
 * @FilePath: \CLI_GUI_Rendering\Airplane war.c
 */

#define WINDOWS

#include "CLI_GUI_Rendering.h"
#include <conio.h>

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
        CHAR ch = 0;
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
            case 72:
            case 119:
                layer_Move(Aircraft_layer, Up, 1);
                ch = '\0';
                break;
            case 75:
            case 97:
                layer_Move(Aircraft_layer, Left, 1);
                ch = '\0';
                break;
            case 77:
            case 100:
                layer_Move(Aircraft_layer, Right, 1);
                ch = '\0';
                break;
            case 80:
            case 115:
                layer_Move(Aircraft_layer, Down, 1);
                ch = '\0';
                break;
            default:
                ch = ch;
                break;
            }

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

            printf("%d\t\t\t\t\t", ch);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}
