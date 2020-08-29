/***
 * @Date         : 2020-04-15 08:46:26
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-27 08:01:05
 * @Description  : 一个能够操控飞机的游戏.
 */

#include "CLI_GUI_Rendering.h"

int main(int argc, char const *argv[])
{
    clear_screen(); // 清除屏幕
    // 在最开始创建主绘制层(窗口层)
    Window_layer *main_layer = new_Window_layer(20, 29);
    //边界图层
    Paint_layer *edge_layer = new_Paint_layer(main_layer, 0, 0);

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
        Paint_layer *Aircraft_layer = new_Paint_layer(main_layer, 0, 0);
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
        Paint_layer *Aircraft_Bullet_layer = new_Paint_layer(main_layer, 0, 0);
        ControlSignal ch = 0;
        CHAR *key_debug = NULL;
        FunctionKeys functionKeys = NO_FunctionKeys;
        // 消息循环
        while (1)
        {
            if (kbhit())
            {
                ch = getch();
                functionKeys = isFunctionSignalKeyPlus(Up | Down | Left | Right, "wsad", ch);
            }
            else
            {
                // 用于在没有操作时初始化信号
                functionKeys = NO_FunctionKeys;
            }

            if (functionKeys != NO_FunctionKeys) // 如果是一个控制信号的话
            {
                switch (functionKeys)
                {
                case Up:
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
                case Left:
                    if (Aircraft_Position.X >= 3)
                    {
                        layer_Move(Aircraft_layer, Left, 1);
                        Aircraft_Position.X -= 1;
                        printf("%d", Aircraft_Position.X);
                    }
                    ch = '\0';
                    key_debug = "向左信号";
                    break;
                case Right:
                    if (Aircraft_Position.X <= Aircraft_layer->width - 4)
                    {
                        layer_Move(Aircraft_layer, Right, 1);
                        Aircraft_Position.X += 1;
                    }
                    ch = '\0';
                    key_debug = "向右信号";
                    break;
                case Down:
                    if (Aircraft_Position.Y <= Aircraft_layer->height - 4)
                    {
                        layer_Move(Aircraft_layer, Down, 1);
                        Aircraft_Position.Y += 1;
                    }
                    ch = '\0';
                    key_debug = "向后信号";
                    break;
                case ESC:
                    printf("游戏已暂停:再次按下Esc退出游戏");
                    if (ESC == isFunctionSignalKey(getch()))
                    {
                        exit(0);
                    }
                    break;
                default:
                    ch = ch;
                    break;
                }
            }
            else
            {
                //在这里处理非控制键
                if (ch == 32)
                {
                    // 按下空格时在飞机前方绘制子弹
                    Write_Point(Aircraft_Bullet_layer, Aircraft_Position.X, Aircraft_Position.Y - 1, '|');
                    ch = '\0';
                    key_debug = "空格";
                }
            }
            // 无论如何,子弹都会向前移动
            layer_Move(Aircraft_Bullet_layer, Up, 1);
            // 刷新界面
            WindowDraw(main_layer, 1);
            ch != '\0' ? printf("当前没有任何操作") : printf("%s\t\t\t\t\t", key_debug);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}
