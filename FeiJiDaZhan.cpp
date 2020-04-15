/*
 * @Date: 2020-04-15 08:46:26
 * @LastEditors: MemoryShadow
 * @LastEditTime: 2020-04-15 09:17:51
 * @FilePath: \CLI_GUI_Rendering\FeiJiDaZhan.cpp
 */

#define WINDOWS

#include "CLI_GUI_Rendering.h"
#include <conio.h>

int main(int argc, char const *argv[])
{
    // 在最开始创建主绘制层(窗口层)
    Window_layer *main_layer = new_Window_layer(20, 29);
    //边界图层
    Paint_layer *edge_layer = new_Paint_layer(main_layer);

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
        Paint_layer *FeiJi = new_Paint_layer(main_layer);
        // 绘制飞机
        Write_Point(FeiJi, FeiJi->width / 2 + 1, FeiJi->height - 2, '*');
        Write_Point(FeiJi, FeiJi->width / 2, FeiJi->height - 3, '*');
        Write_Point(FeiJi, FeiJi->width / 2, FeiJi->height - 2, '*');
        Write_Point(FeiJi, FeiJi->width / 2 - 1, FeiJi->height - 2, '*');
        CHAR ch = 0;
        // 消息循环
        while (true)
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
                layer_Move(FeiJi, Up, 1);
                ch = '\0';
                break;
            case 75:
            case 97:
                layer_Move(FeiJi, Left, 1);
                ch = '\0';
                break;
            case 77:
            case 100:
                layer_Move(FeiJi, Right, 1);
                ch = '\0';
                break;
            case 80:
            case 115:
                layer_Move(FeiJi, Down, 1);
                ch = '\0';
                break;
            default:
                ch = ch;
                break;
            }

            // 刷新界面
            WindowDraw(main_layer, 1);
            printf("%d\t", ch);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}
