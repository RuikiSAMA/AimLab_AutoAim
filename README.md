# AimLab_AutoAim
OpenCV based console program, implement auto aiming and shooting in AimLab

Demo pov: https://v.douyin.com/iyopvc9m/

### 使用：
1.修改 CMakeList 中OpenCV 的 include 目录
2.打开 AimLab 中某个训练项目，并暂停游戏
3.编译运行，在控制台中输入 AimLab 的 ProcessID，按下 Enter
4.切换到 AimLab 的暂停页面，按下 Esc 继续游戏，此时开始自动瞄准
5.需要结束自动瞄准时再次按下 Esc 结束程序

注意：
AimLab需要切换至720p分辨率，窗口模式，将游戏内的小球设置为红色
开始自动瞄准后需手动按下Esc结束程序，否则鼠标指针会自动瞄准一切红色的东西
