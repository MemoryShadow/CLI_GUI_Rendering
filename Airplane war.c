/***
 * @Date         : 2020-04-15 08:46:26
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-27 08:01:05
 * @Description  : 一个能够操控飞机的游戏.
 */

#include "CLI_GUI_Rendering.h"

// 飞机被击中事件
int Airplane_Hit_Event(Event e)
{
    // 清除飞机
    Write_Point(e.layer, 1, 0, ' ');
    Write_Line(e.layer, 0, 1, "   ", 3);
}

// 外边框渲染冲突处理
int edge_Event(Event e)
{
    // 如果要渲染的目标一样就忽略事件
    if (e.layer->Data[e.position.Y][e.position.X] != *e.P_value.old)
    {
        // 如果不一样就设置ch为问号
        *e.P_value.old = '-';
    }
}

int main(int argc, char const *argv[])
{
    clear_screen(); // 清除屏幕
    // 在最开始创建主绘制层(窗口层)
    Window_layer *Base_layer = new_Window_layer(30, 29, NULL);
    Window_layer *main_layer = new_Window_layer(20, 29, Base_layer);
    Window_layer *Info_layer = new_Window_layer(10, 29, Base_layer);
    setlayerStart(Info_layer, 19, 0);
    //边界图层
    Paint_layer *edge_main_layer = new_Paint_layer(main_layer, 0, 0);
    Paint_layer *edge_Info_layer = new_Paint_layer(Info_layer, 0, 0);
    // 文字层
    Paint_layer *Text_layer = new_Paint_layer(Info_layer, 6, 0);
    setlayerStart(Text_layer, 2, 2);
    // 显示文字
    Write_Line(Text_layer, 0, 0, "|play|", 6);
    // 文字层2
    Paint_layer *Text2_layer = new_Paint_layer(Info_layer, 6, 1);
    setlayerStart(Text2_layer, 2, 3);

    // 绘制周围边界
    for (unsigned index = 1; index < main_layer->width - 1; index++)
    {
        Write_Point(edge_main_layer, index, 0, '-');
        Write_Point(edge_main_layer, index, main_layer->height - 1, '-');
    }
    for (unsigned index = 1; index < main_layer->height - 1; index++)
    {
        Write_Point(edge_main_layer, 0, index, '|');
        Write_Point(edge_main_layer, main_layer->width - 1, index, '|');
    }
    // 边缘层绑定碰撞事件
    EventBinding(edge_main_layer, Collision, edge_Event);
    for (unsigned index = 1; index < Info_layer->width - 1; index++)
    {
        Write_Point(edge_Info_layer, index, 0, '-');
        Write_Point(edge_Info_layer, index, Info_layer->height - 1, '-');
    }
    for (unsigned index = 1; index < Info_layer->height - 1; index++)
    {
        Write_Point(edge_Info_layer, 0, index, '|');
        Write_Point(edge_Info_layer, Info_layer->width - 1, index, '|');
    }
    // 游戏代码
    {
        // 玩家飞机层
        Paint_layer *Player_Aircraft_layer = new_Paint_layer(main_layer, 3, 2);
        // 飞机层绑定碰撞事件
        // 绘制飞机
        Write_Point(Player_Aircraft_layer, 1, 0, '*');
        Write_Line(Player_Aircraft_layer, 0, 1, "***", 3);
        // 设置飞机坐标
        setlayerStart(Player_Aircraft_layer, main_layer->width / 2 - 1, main_layer->height - 3);
        // 创建一个层用于储存玩家飞机子弹
        Paint_layer *Player_Aircraft_Bullet_layer = new_Paint_layer(main_layer, 0, 0);
        EventBinding(Player_Aircraft_Bullet_layer, Collision, Airplane_Hit_Event);
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
                    if (Player_Aircraft_layer->start.Y >= 2)
                        MovelayerStart(Player_Aircraft_layer, Up, 1);
                    ch = '\0';
                    key_debug = "| Up |";
                    break;
                case Left:
                    if (Player_Aircraft_layer->start.X >= 2)
                        MovelayerStart(Player_Aircraft_layer, Left, 1);
                    ch = '\0';
                    key_debug = "|Left|";
                    break;
                case Right:
                    if (Player_Aircraft_layer->start.X <= main_layer->width - 5)
                        MovelayerStart(Player_Aircraft_layer, Right, 1);
                    ch = '\0';
                    key_debug = "|Right";
                    break;
                case Down:
                    if (Player_Aircraft_layer->start.Y <= main_layer->height - 4)
                        MovelayerStart(Player_Aircraft_layer, Down, 1);
                    ch = '\0';
                    key_debug = "|Down|";
                    break;
                case ESC:
                    printf("游戏已暂停:再次按下Esc退出游戏");
                    Write_Line(Text_layer, 0, 0, "|pause", 6);
                    WindowDraw(Base_layer, 1);
                    if (ESC == isFunctionSignalKey(getch()))
                    {
                        delete_Window_layer(Base_layer);
                        exit(0);
                    }
                    Write_Line(Text_layer, 0, 0, "|play|", 6);
                    WindowDraw(Base_layer, 1);
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
                    Write_Point(Player_Aircraft_Bullet_layer, Player_Aircraft_layer->start.X + 1, Player_Aircraft_layer->start.Y, '|');
                    ch = '\0';
                    key_debug = "|space";
                }
            }
            // 无论如何,子弹层的内容都会向前移动
            layer_Move(Player_Aircraft_Bullet_layer, Up, 1);
            // 刷新界面
            WindowDraw(Base_layer, 1);
            ch != '\0' ? printf("当前没有任何操作") : Write_Line(Text2_layer, 0, 0, key_debug, 6);
        }
    }
    return 0;
}
