## 项目说明

本项目基于 [No-Chicken/OV-Watch](https://github.com/No-Chicken/OV-Watch/tree/main/lv_sim_vscode_win)。在原项目的基础上，我添加了 **菜单拖拽换位功能**，以增强用户体验。

感谢原项目作者的开源贡献！
## 新增功能：菜单拖拽换位

### 功能概述
在原项目的基础上，我实现了菜单项的拖拽换位功能。用户可以通过长按菜单项并拖动来调整菜单项的顺序。该功能支持以下特性：
- **长按触发拖拽**：用户长按菜单项后，菜单项会放大并进入拖拽状态。
- **自动滚动**：当拖拽到屏幕边缘时，菜单会自动滚动以显示更多内容。
- **悬停交换**：当拖拽的菜单项悬停在另一个菜单项上超过一定时间时，两者会自动交换位置。
- **动画效果**：菜单项的交换和复位过程均带有平滑的动画效果。
![image](https://github.com/Yweik/lv_port_pc_vscode-release-v8/blob/main/%E6%97%A0%E6%A0%87%E9%A2%98%E8%A7%86%E9%A2%91%E2%80%94%E2%80%94%E4%BD%BF%E7%94%A8Clipchamp%E5%88%B6%E4%BD%9C.gif)
### 实现细节
该功能通过 `lvgl` 的事件系统实现，主要逻辑在 `drag_event_handler_` 函数中处理。以下是关键实现点：
- **长按事件 (`LV_EVENT_LONG_PRESSED`)**：触发拖拽状态，放大当前菜单项并缩小其他菜单项。
- **拖拽事件 (`LV_EVENT_PRESSING`)**：处理菜单项的移动、自动滚动以及悬停交换逻辑。
- **释放事件 (`LV_EVENT_RELEASED`)**：恢复菜单项大小并重新排列菜单项顺序。

### 代码示例
以下是核心代码片段：

```c
static void drag_event_handler_(lv_event_t *e)
{
    // 详细代码见项目源码
}
## 贡献与反馈

欢迎提交 Issue 或 Pull Request 来改进本项目。如有任何问题或建议，请通过 [GitHub Issues](项目 Issues 链接) 反馈。
