# Demo2Plus
The real PBR with IBL
PBR光照实现： 如果需要运行请切换至x86编译， 使用opengl4.6标准编写，如果遇到fail to create GLFW window请检查opengl版本
在场景中wasd移动，鼠标右键加速，x键退出程序。 Image text 实现了基于Cook-Torrance BRDF的PBR光照模型
，可以通过调整菲涅尔系数freh和粗糙度rough来模拟各种材质
使用IBL来模拟间接光照，载入不同的环境贴图时会自动预计算相应的辐照度贴图，BRDF贴图，和镜面反射贴图。
可以通过更换不同的贴图和给材质球加上材质贴图来产生各种各样令人惊异的效果。
部分预览：
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/1.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/2.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/3.png)
