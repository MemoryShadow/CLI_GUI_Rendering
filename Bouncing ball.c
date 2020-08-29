/***
 * @Date         : 2020-04-14 21:41:50
 * @Author       : MemoryShadow
 * @LastEditors  : MemoryShadow
 * @LastEditTime : 2020-08-27 07:59:55
 * @Description  : 一个看着小球弹跳的测试游戏
 */

#include "CLI_GUI_Rendering.h"

int main(int argc, char const *argv[])
{
    clear_screen(); // 清除屏幕
    // 在最开始创建主绘制层(窗口层)
    Window_layer *main_layer = new_Window_layer(20, 15);
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
    // 小球层
    Paint_layer *ball_layer = new_Paint_layer(main_layer, 1, 1);
    // 绘制小球
    Write_Point(ball_layer, 0, 0, '*');
    // 设置小球初始位置
    setlayerStart(ball_layer, 2, 2);
    // 小球运动代码
    {
        // 运动轨迹标记
        int width_flag = 1, height_flag = 1;
        while (1)
        {
            // 撞墙判断
            if ((ball_layer->start.X >= (main_layer->width - 2)) || (ball_layer->start.X <= 1))
            {
                printf("W:%d H:%d WF:%d HF:%d\t\n", ball_layer->start.X, ball_layer->start.Y, width_flag, height_flag);
                width_flag = -width_flag;
            }
            if ((ball_layer->start.Y >= (main_layer->height - 2)) || (ball_layer->start.Y <= 1))
            {
                printf("W:%d H:%d WF:%d HF:%d\t\n", ball_layer->start.X, ball_layer->start.Y, width_flag, height_flag);
                height_flag = -height_flag;
            }
            WindowDraw(main_layer, 1);
            // 移动小球
            MovelayerStart(ball_layer, (width_flag < 0 ? Left : Right) | (height_flag < 0 ? Up : Down), 1);
        }
    }
    delete_Window_layer(main_layer);
    return 0;
}